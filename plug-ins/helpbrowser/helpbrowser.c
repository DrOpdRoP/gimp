/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * The GIMP Help Browser
 * Copyright (C) 1999-2002 Sven Neumann <sven@gimp.org>
 *                         Michael Natterer <mitch@gimp.org>
 *
 * Some code & ideas stolen from the GNOME help browser.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <libgtkhtml/gtkhtml.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "queue.h"

#include "libgimp/stdplugins-intl.h"


/*  defines  */

#ifdef __EMX__
#define chdir _chdir2
#endif

#define GIMP_HELP_EXT_NAME      "extension_gimp_help_browser"
#define GIMP_HELP_TEMP_EXT_NAME "extension_gimp_help_browser_temp"

#define GIMP_HELP_PREFIX        "help"

enum {
  CONTENTS,
  INDEX,
  HELP
};

enum {
  URL_UNKNOWN,
  URL_NAMED, /* ??? */
  URL_JUMP,
  URL_FILE_LOCAL,

  /* aliases */
  URL_LAST = URL_FILE_LOCAL
};

/*  structures  */

typedef struct 
{
  gint       index;
  gchar     *label;
  Queue     *queue;
  gchar     *current_ref;
  GtkWidget *html;
  gchar     *home;
} HelpPage;

typedef struct
{
  gchar *title;
  gchar *ref;
  gint   count;
} HistoryItem;

/*  constant strings  */

static gchar *doc_not_found_format_string =
N_("<html><head><title>Document not found</title></head>"
   "<body bgcolor=\"#ffffff\">"
   "<center>"
   "<p>"
   "%s"
   "<h3>Couldn't find document</h3>"
   "<tt>%s</tt>"
   "</center>"
   "<p>"
   "<small>This either means that the help for this topic has not been written "
   "yet or that something is wrong with your installation. "
   "Please check carefully before you report this as a bug.</small>" 
   "</body>"
   "</html>");

static gchar *dir_not_found_format_string =
N_("<html><head><title>Directory not found</title></head>"
   "<body bgcolor=\"#ffffff\">"
   "<center>"
   "<p>"
   "%s"
   "<h3>Couldn't change to directory</h3>"
   "<tt>%s</tt>"
   "<h3>while trying to access</h3>"
   "<tt>%s</tt>"
   "</center>"
   "<p>"
   "<small>This either means that the help for this topic has not been written "
   "yet or that something is wrong with your installation. "
   "Please check carefully before you report this as a bug.</small>" 
   "</body>"
   "</html>");

static gchar *eek_png_tag = "<h1>Eeek!</h1>";


/*  the three help notebook pages  */

static HelpPage pages[] =
{
  {
    CONTENTS,
    N_("Contents"),
    NULL,
    NULL,
    NULL,
    "contents.html"
  },

  {
    INDEX,
    N_("Index"),
    NULL,
    NULL,
    NULL,
    "index.html"
  },

  {
    HELP,
    NULL,
    NULL,
    NULL,
    NULL,
    "introduction.html"
  }
};

static gchar     *gimp_help_root = NULL;

static HelpPage  *current_page = &pages[HELP];
static GList     *history = NULL;

static GtkWidget *back_button;
static GtkWidget *forward_button;
static GtkWidget *notebook;
static GtkWidget *combo;

static GtkTargetEntry help_dnd_target_table[] =
{
  { "_NETSCAPE_URL", 0, 0 },
};

/*  GIMP plugin stuff  */

static void query (void);
static void run   (gchar      *name,
		   gint        nparams,
		   GimpParam  *param,
		   gint       *nreturn_vals,
		   GimpParam **return_vals);

GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,  /* init_proc  */
  NULL,  /* quit_proc  */
  query, /* query_proc */
  run,   /* run_proc   */
};

static gboolean temp_proc_installed = FALSE;

/*  forward declaration  */

static gint load_page (HelpPage    *source_page,
		       HelpPage    *dest_page,
		       const gchar *ref,
		       gint         pos, 
		       gboolean     add_to_queue,
		       gboolean     add_to_history);

/*  functions  */

static void
close_callback (GtkWidget *widget,
		gpointer   user_data)
{
  gtk_main_quit ();
}

static void
update_toolbar (HelpPage *page)
{
  if (back_button)
    gtk_widget_set_sensitive (back_button, queue_isprev (page->queue));
  if (forward_button)
    gtk_widget_set_sensitive (forward_button, queue_isnext (page->queue));
}

#if 0
static void
jump_to_anchor (HelpPage *page, 
		gchar    *anchor)
{
  gint pos;

  g_return_if_fail (page != NULL && anchor != NULL);

  html_view_jump_to_anchor (HTML_VIEW (page->html), anchor);

  pos = gtk_xmhtml_get_topline (GTK_XMHTML (page->html));
  queue_add (page->queue, page->current_ref, pos);

  update_toolbar (page);
}
#endif

static void
forward_callback (GtkWidget *widget,
		  gpointer   data)
{
  gchar *ref;
  gint pos;

  if (!(ref = queue_next (current_page->queue, &pos)))
    return;

  load_page (current_page, current_page, ref, pos, FALSE, FALSE);
  queue_move_next (current_page->queue);

  update_toolbar (current_page);
}

static void
back_callback (GtkWidget *widget,
	       gpointer   data)
{
  gchar *ref;
  gint pos;

  if (!(ref = queue_prev (current_page->queue, &pos)))
    return;

  load_page (current_page, current_page, ref, pos, FALSE, FALSE);
  queue_move_prev (current_page->queue);

  update_toolbar (current_page);
}

static void 
entry_changed_callback (GtkWidget *widget,
			gpointer   data)
{
  GList       *list;
  HistoryItem *item;
  const gchar *entry_text;
  gchar       *compare_text;
  gboolean     found = FALSE;

  entry_text = gtk_entry_get_text (GTK_ENTRY (widget));

  for (list = history; list && !found; list = list->next)
    {
      item = (HistoryItem *) list->data;

      if (item->count)
	compare_text = g_strdup_printf ("%s <%i>",
					item->title,
					item->count + 1);
      else
	compare_text = item->title;

      if (strcmp (compare_text, entry_text) == 0)
	{
	  load_page (&pages[HELP], &pages[HELP], item->ref, 0, TRUE, FALSE);
	  found = TRUE;
	}

      if (item->count)
	g_free (compare_text);
    }
}

static gint
entry_button_press_callback (GtkWidget      *widget,
			     GdkEventButton *bevent,
			     gpointer        data)
{
  if (current_page != &pages[HELP])
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), HELP);

  return FALSE;
}

static void
history_add (gchar *ref,
	     gchar *title)
{
  GList       *list;
  GList       *found = NULL;
  HistoryItem *item;
  GList       *combo_list = NULL;
  gint         title_found_count = 0;

  for (list = history; list && !found; list = list->next)
    {
      item = (HistoryItem *) list->data;

      if (strcmp (item->title, title) == 0)
	{
	  if (strcmp (item->ref, ref) != 0)
	    {
	      title_found_count++;
	      continue;
	    }

	  found = list;	}
    }

  if (found)
    {
      item = (HistoryItem *) found->data;
      history = g_list_remove_link (history, found);
    }
  else
    {
      item = g_new (HistoryItem, 1);
      item->ref = g_strdup (ref);
      item->title = g_strdup (title);
      item->count = title_found_count;
    }

  history = g_list_prepend (history, item);

  for (list = history; list; list = list->next)
    {
      gchar* combo_title;

      item = (HistoryItem *) list->data;

      if (item->count)
	combo_title = g_strdup_printf ("%s <%i>",
				       item->title,
				       item->count + 1);
      else
	combo_title = g_strdup (item->title);

      combo_list = g_list_prepend (combo_list, combo_title);
    }

  combo_list = g_list_reverse (combo_list);

  g_signal_handlers_block_by_func (G_OBJECT (GTK_COMBO (combo)->entry),
                                   entry_changed_callback, combo);
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), combo_list);
  g_signal_handlers_unblock_by_func (G_OBJECT (GTK_COMBO (combo)->entry),
                                     entry_changed_callback, combo);

  for (list = combo_list; list; list = list->next)
    g_free (list->data);

  g_list_free (combo_list);
}

static void
update_state (HelpPage *page,
              gchar    *ref,
              gint      pos,
              gboolean  add_to_queue,
              gboolean  add_to_history)
{
  gchar *title = NULL;
  
  g_return_if_fail (page != NULL && ref != NULL);
  
  if (add_to_queue) 
    queue_add (page->queue, ref, pos);
  
  if (page->index == HELP)
    {
#if 0
      title = XmHTMLGetTitle (page->html);
#endif
      if (!title)
	title = (_("<Untitled>"));
      
      if (add_to_history)
	history_add (ref, title);

      g_signal_handlers_block_by_func (G_OBJECT (GTK_COMBO (combo)->entry),
                                       entry_changed_callback, combo);
      gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (combo)->entry), title);
      g_signal_handlers_unblock_by_func (G_OBJECT (GTK_COMBO (combo)->entry),
                                         entry_changed_callback, combo);
    }

  update_toolbar (page);
}

static gboolean
load_page (HelpPage    *source_page,
	   HelpPage    *dest_page,
	   const gchar *ref,	  
	   gint         pos,
	   gboolean     add_to_queue,
	   gboolean     add_to_history)
{
  HtmlDocument *doc;
  FILE     *fp = NULL;
  gchar     buf[8192];
  gchar    *old_dir;
  gchar    *new_dir;
  gchar    *new_base;
  gchar    *new_ref;
  gchar    *tmp;
  gsize     bytes_read;
  gboolean  page_valid  = FALSE;
  gboolean  filters_dir = FALSE;

  g_return_val_if_fail (source_page != NULL && dest_page != NULL, FALSE);
  g_return_val_if_fail (ref != NULL, FALSE);

  doc = HTML_VIEW (dest_page->html)->document;

  old_dir  = g_path_get_dirname (source_page->current_ref);
  new_dir  = g_path_get_dirname (ref);
  new_base = g_path_get_basename (ref);

  /* return value is intentionally ignored */
  chdir (old_dir);

  if (chdir (new_dir) == -1)
    {
      gchar *msg;

      if (g_path_is_absolute (ref))
	new_ref = g_strdup (ref);
      else
	new_ref = g_build_filename (old_dir, ref, NULL);

      msg = g_strdup_printf (gettext (dir_not_found_format_string),
                             eek_png_tag, new_dir, new_ref);

      html_document_clear (doc);
      html_document_open_stream (doc, "text/html");
      html_document_write_stream (doc, msg, strlen (msg));
      html_document_close_stream (doc);
      
      g_free (msg);
      
      goto FINISH;
    }

  tmp = new_dir;
  new_dir = g_path_get_basename (tmp);
  g_free (tmp);

  if (strcmp (new_dir, "filters") == 0)
    filters_dir = TRUE;

  g_free (new_dir);
  new_dir = g_get_current_dir ();

  new_ref = g_build_filename (new_dir, new_base, NULL);

  if (strcmp (dest_page->current_ref, new_ref) == 0)
    {
#if 0      
      gtk_xmhtml_set_topline (GTK_XMHTML (dest_page->html), pos);
#endif

      if (add_to_queue)
	queue_add (dest_page->queue, new_ref, pos);

      goto FINISH;
    }

  html_document_clear (doc);
  html_document_open_stream (doc, "text/html");

  /*
   *  handle basename like: filename.html#11111 -> filename.html
   */ 
  g_strdelimit (new_base, "#", '\0');

  fp = fopen (new_base, "rt");

  if (fp != NULL)
    {
      while ((bytes_read = 
              fread (buf, sizeof (gchar), sizeof (buf), fp)) > 0)
	html_document_write_stream (doc, buf, bytes_read);
    }
  else if (filters_dir)
    {
      gchar *undocumented_filter = 
        g_build_filename (new_dir, "undocumented_filter.html", NULL);

      fp = fopen (undocumented_filter, "rt");

      if (fp != NULL)
	{
          while ((bytes_read = 
                  fread (buf, sizeof (gchar), sizeof (buf), fp)) > 0)
            html_document_write_stream (doc, buf, bytes_read);
	}

      g_free (undocumented_filter);
    }

  if (fp)
    {
      fclose (fp);
      page_valid = TRUE;
    }
  else
    {
      gchar *msg = g_strdup_printf (gettext (doc_not_found_format_string),
                                    eek_png_tag, ref);
      
      chdir (old_dir);
      html_document_write_stream (doc, msg, strlen (msg));
      g_free (msg);
    }

  html_document_close_stream (doc);

 FINISH:
  
  g_free (dest_page->current_ref);
  dest_page->current_ref = new_ref;

  g_free (old_dir);
  g_free (new_dir);
  g_free (new_base);

  update_state (dest_page, new_ref, 0,
                add_to_queue, add_to_history && page_valid);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), dest_page->index);

  return (page_valid);
}

static void
link_clicked (HtmlDocument *doc,
              const gchar  *url,
              gpointer      data)
{
  load_page (current_page, &pages[HELP], url, 0, TRUE, TRUE);

#if 0
  GimpParam *return_vals;
  gint       nreturn_vals;

  switch (cbs->url_type)
    {
    case URL_JUMP:
      jump_to_anchor (current_page, cbs->href);
      break;

    case URL_FILE_LOCAL:
      load_page (current_page, &pages[HELP], cbs->href, 0, TRUE, TRUE);
      break;

    default:
      /*  try to call netscape through the web_browser interface */
      return_vals = gimp_run_procedure ("extension_web_browser",
					&nreturn_vals,
					GIMP_PDB_INT32,  GIMP_RUN_NONINTERACTIVE,
					GIMP_PDB_STRING, cbs->href,
					GIMP_PDB_INT32,  FALSE,
					GIMP_PDB_END);
       gimp_destroy_params (return_vals, nreturn_vals);
      break;
    }
#endif
}

static void 
notebook_switch_callback (GtkNotebook     *notebook,
			  GtkNotebookPage *page,
			  gint             page_num,
			  gpointer         user_data)
{
  g_return_if_fail (page_num >= 0 && page_num < 3);

  /*  Set the new page  */
  current_page = &pages[page_num];

  update_toolbar (current_page);
}

static gint
notebook_label_button_press_callback (GtkWidget *widget,
				      GdkEvent  *event,
				      gpointer   data)
{
  guint i = GPOINTER_TO_UINT (data);

  if (current_page != &pages[i])
    gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), i);
  
  return TRUE;
}

static void
combo_drag_begin (GtkWidget *widget,
		  gpointer   data)
{ 
}

static void
combo_drag_handle (GtkWidget        *widget, 
		   GdkDragContext   *context,
		   GtkSelectionData *selection_data,
		   guint             info,
		   guint             time,
		   gpointer          data)
{
  HelpPage *page = (HelpPage*)data;

  if (! page->current_ref)
    return;

  gtk_selection_data_set (selection_data,
                          selection_data->target,
                          8, 
                          page->current_ref, 
                          strlen (page->current_ref));
}

static gboolean
set_initial_history (gpointer data)
{
  gint   add_to_history = GPOINTER_TO_INT (data);
  gchar *title;

#if 0
  title = XmHTMLGetTitle (pages[HELP].html);
#endif
  title = "???";

  if (add_to_history)
    history_add (pages[HELP].current_ref, title);

  g_signal_handlers_block_by_func (G_OBJECT (GTK_COMBO (combo)->entry),
                                   entry_changed_callback, combo);
  gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (combo)->entry), title);
  g_signal_handlers_unblock_by_func (G_OBJECT (GTK_COMBO (combo)->entry),
                                     entry_changed_callback, combo);

  return FALSE;
}

gboolean
open_browser_dialog (gchar *help_path,
		     gchar *locale,
		     gchar *help_file)
{
  GtkWidget *window;
  GtkWidget *vbox, *hbox, *bbox, *html_box;
  GtkWidget *button;
  GtkWidget *title;
  GtkWidget *drag_source;
  GtkWidget *label;

  gchar   *initial_dir;
  gchar   *initial_ref;
  gchar   *root_dir;
  gchar   *eek_png_path;
  gint     success;
  guint    i;

  gimp_ui_init ("helpbrowser", TRUE);

  root_dir = g_strdup (gimp_help_root);

  if (chdir (root_dir) == -1)
    {
      g_message (_("GIMP Help Browser Error.\n\n"
		   "Couldn't find my root html directory.\n"
		   "(%s)"), root_dir);
      return FALSE;
    }

  eek_png_path = g_build_filename (root_dir, "images", "eek.png", NULL);

  if (access (eek_png_path, R_OK) == 0)
    eek_png_tag = g_strdup_printf ("<img src=\"%s\">", eek_png_path);

  g_free (eek_png_path);

  if (chdir (help_path) == -1)
    {
      g_message (_("GIMP Help Browser Error.\n\n"
		   "Couldn't find my root html directory.\n"
		   "(%s)"), help_path);
      return FALSE;
    }

  initial_dir = g_get_current_dir ();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (close_callback),
                    NULL);
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (close_callback),
                    NULL);
  gtk_window_set_wmclass (GTK_WINDOW (window), "helpbrowser", "Gimp");
  gtk_window_set_title (GTK_WINDOW (window), _("GIMP Help Browser"));

  gimp_help_connect (window, gimp_standard_help_func, "dialogs/help.html");

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  bbox = gtk_hbutton_box_new ();
  gtk_box_set_spacing (GTK_BOX (bbox), 0);
  gtk_box_pack_start (GTK_BOX (hbox), bbox, FALSE, FALSE, 0);

  back_button =  gtk_button_new_from_stock (GTK_STOCK_GO_BACK);
  gtk_button_set_relief (GTK_BUTTON (back_button), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (bbox), back_button);
  gtk_widget_set_sensitive (GTK_WIDGET (back_button), FALSE);
  g_signal_connect (G_OBJECT (back_button), "clicked",
                    G_CALLBACK (back_callback),
                    NULL);
  gtk_widget_show (back_button);

  forward_button = gtk_button_new_from_stock (GTK_STOCK_GO_FORWARD);
  gtk_button_set_relief (GTK_BUTTON (forward_button), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (bbox), forward_button);
  gtk_widget_set_sensitive (GTK_WIDGET (forward_button), FALSE);
  g_signal_connect (G_OBJECT (forward_button), "clicked",
                    G_CALLBACK (forward_callback),
                    NULL);
  gtk_widget_show (forward_button);

  gtk_widget_show (bbox);

  bbox = gtk_hbutton_box_new ();
  gtk_box_pack_end (GTK_BOX (hbox), bbox, FALSE, FALSE, 0);

  button = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
  gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
  gtk_container_add (GTK_CONTAINER (bbox), button);
  g_signal_connect (G_OBJECT (button), "clicked",
                    G_CALLBACK (close_callback),
                    NULL);
  gtk_widget_show (button);

  gtk_widget_show (bbox);
  gtk_widget_show (hbox);

  notebook = gtk_notebook_new ();
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

  for (i = 0; i < 3; i++)
    {
      pages[i].index = i;
      pages[i].html  = html_view_new ();
      pages[i].queue = queue_new ();

      gtk_widget_set_size_request (GTK_WIDGET (pages[i].html), -1, 300);

      switch (i)
	{
	case CONTENTS:
	case INDEX:
	  pages[i].current_ref = 
            g_build_filename (root_dir, locale, ".", NULL);

	  title = drag_source = gtk_event_box_new ();
	  label = gtk_label_new (gettext (pages[i].label));
	  gtk_container_add (GTK_CONTAINER (title), label);
	  gtk_widget_show (label);
	  break;

	case HELP:
	  pages[i].current_ref = 
            g_build_filename (initial_dir, locale, ".", NULL);

	  title = combo = gtk_combo_new ();
	  drag_source = GTK_COMBO (combo)->entry;
	  gtk_widget_set_size_request (GTK_WIDGET (combo), 300, -1);
	  g_object_set (G_OBJECT (GTK_COMBO (combo)->entry),
                        "editable", FALSE,
                        NULL); 
	  gtk_combo_set_use_arrows (GTK_COMBO (combo), TRUE);
	  g_signal_connect (G_OBJECT (GTK_COMBO (combo)->entry), 
                            "changed",
                            G_CALLBACK (entry_changed_callback), 
                            combo);
	  g_signal_connect (G_OBJECT (GTK_WIDGET (GTK_COMBO (combo)->entry)), 
                            "button-press-event",
                            G_CALLBACK (entry_button_press_callback), 
                            NULL);
	  gtk_widget_show (combo);
	  break;

	default:
	  title = drag_source = NULL;     /* to please the compiler */
	  break;
	}	  

      /*  connect to the button_press signal to make notebook switching work */ 
      g_signal_connect (G_OBJECT (title), "button_press_event",
                        G_CALLBACK (notebook_label_button_press_callback), 
                        GUINT_TO_POINTER (i));

      /*  dnd source  */
      gtk_drag_source_set (GTK_WIDGET (drag_source),
			   GDK_BUTTON1_MASK,
			   help_dnd_target_table,
                           G_N_ELEMENTS (help_dnd_target_table), 
			   GDK_ACTION_MOVE | GDK_ACTION_COPY);
      g_signal_connect (G_OBJECT (drag_source), "drag_begin",
                        G_CALLBACK (combo_drag_begin),
                        &pages[i]);
      g_signal_connect (G_OBJECT (drag_source), "drag_data_get",
                        G_CALLBACK (combo_drag_handle),
                        &pages[i]);

      html_box = gtk_scrolled_window_new
        (gtk_layout_get_hadjustment (GTK_LAYOUT (pages[i].html)),
         gtk_layout_get_vadjustment (GTK_LAYOUT (pages[i].html)));

      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (html_box),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);
      gtk_container_add (GTK_CONTAINER (html_box), pages[i].html);

      gtk_notebook_append_page (GTK_NOTEBOOK (notebook), html_box, title);
      gtk_widget_show (title);

      if (i == HELP && help_file)
	{
	  initial_ref = 
            g_build_filename (initial_dir, locale, help_file, NULL);
	}
      else
	{
	  initial_ref = 
            g_build_filename (root_dir, locale, pages[i].home, NULL);
	}

      html_view_set_document (HTML_VIEW (pages[i].html), html_document_new ());

      success = load_page (&pages[i], &pages[i], initial_ref, 0, TRUE, FALSE);
      g_free (initial_ref);

      gtk_widget_show (pages[i].html);
      gtk_widget_show (html_box);

      g_signal_connect (G_OBJECT (HTML_VIEW (pages[i].html)->document),
                        "link_clicked",
                        G_CALLBACK (link_clicked),
                        &pages[i]);
    }

  g_free (root_dir);

  g_signal_connect (G_OBJECT (notebook), "switch-page",
                    G_CALLBACK (notebook_switch_callback),
                    NULL);

  gtk_notebook_set_current_page (GTK_NOTEBOOK (notebook), HELP);
  gtk_widget_show (notebook);

  gtk_widget_show (vbox);
  gtk_widget_show (window);

  g_idle_add (set_initial_history, GINT_TO_POINTER (success));

  g_free (initial_dir);

  return TRUE;
}

static gboolean
idle_load_page (gpointer data)
{
  gchar *path = data;

  load_page (&pages[HELP], &pages[HELP], path, 0, TRUE, TRUE);
  g_free (path);

  return FALSE;
}

static void
run_temp_proc (gchar      *name,
	       gint        nparams,
	       GimpParam  *param,
	       gint       *nreturn_vals,
	       GimpParam **return_vals)
{
  static GimpParam  values[1];
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  gchar *help_path;
  gchar *locale;
  gchar *help_file;
  gchar *path;

  /*  set default values  */
  help_path = g_strdup (gimp_help_root);
  locale    = g_strdup ("C");
  help_file = g_strdup ("introduction.html");

  /*  Make sure all the arguments are there!  */
  if (nparams == 3)
    {
      if (param[0].data.d_string && strlen (param[0].data.d_string))
	{
	  g_free (help_path);
	  help_path = g_strdup (param[0].data.d_string);
	  g_strdelimit (help_path, "/", G_DIR_SEPARATOR);
	}
      if (param[1].data.d_string && strlen (param[1].data.d_string))
	{
	  g_free (locale);
	  locale    = g_strdup (param[1].data.d_string);
	}
      if (param[2].data.d_string && strlen (param[2].data.d_string))
	{
	  g_free (help_file);
	  help_file = g_strdup (param[2].data.d_string);
	  g_strdelimit (help_file, "/", G_DIR_SEPARATOR);
	}
    }

  path = g_build_filename (help_path, locale, help_file, NULL);

  g_free (help_path);
  g_free (locale);
  g_free (help_file);

  g_idle_add (idle_load_page, path);

  *nreturn_vals = 1;
  *return_vals = values;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;
}

/*  from libgimp/gimp.c  */
void
gimp_run_temp (void);

static gboolean
input_callback (GIOChannel   *channel,
                GIOCondition  condition,
                gpointer      data)
{
  /* We have some data in the wire - read it */
  /* The below will only ever run a single proc */
  gimp_run_temp ();

  return TRUE;
}

static void
install_temp_proc (void)
{
  static GimpParamDef args[] =
  {
    { GIMP_PDB_STRING, "help_path", "" },
    { GIMP_PDB_STRING, "locale",    "Langusge to use" },
    { GIMP_PDB_STRING, "help_file", "Path of a local document to open. "
                                    "Can be relative to GIMP_HELP_PATH." }
  };

  gimp_install_temp_proc (GIMP_HELP_TEMP_EXT_NAME,
			  "DON'T USE THIS ONE",
			  "(Temporary procedure)",
			  "Sven Neumann <sven@gimp.org>, "
			  "Michael Natterer <mitch@gimp.org>",
			  "Sven Neumann & Michael Natterer",
			  "1999",
			  NULL,
			  "",
			  GIMP_TEMPORARY,
			  G_N_ELEMENTS (args), 0,
			  args, NULL,
			  run_temp_proc);

  /* Tie into the gdk input function */
  g_io_add_watch (_readchannel, G_IO_IN | G_IO_PRI, input_callback, NULL);

  temp_proc_installed = TRUE;
}

static gboolean
open_url (gchar *help_path,
	  gchar *locale,
	  gchar *help_file)
{
  if (! open_browser_dialog (help_path, locale, help_file))
    return FALSE;

  install_temp_proc ();
  gtk_main ();

  return TRUE;
}

MAIN ()

static void
query (void)
{
  static GimpParamDef args[] =
  {
    { GIMP_PDB_INT32,  "run_mode",  "Interactive" },
    { GIMP_PDB_STRING, "help_path", "" },
    { GIMP_PDB_STRING, "locale",    "Language to use" },
    { GIMP_PDB_STRING, "help_file", "Path of a local document to open. "
                                    "Can be relative to GIMP_HELP_PATH." }
  };

  gimp_install_procedure (GIMP_HELP_EXT_NAME,
                          "Browse the GIMP help pages",
                          "A small and simple HTML browser optimzed for "
			  "browsing the GIMP help pages.",
                          "Sven Neumann <sven@gimp.org>, "
			  "Michael Natterer <mitch@gimp.org>",
			  "Sven Neumann & Michael Natterer",
                          "1999",
                          NULL,
                          "",
                          GIMP_EXTENSION,
                          G_N_ELEMENTS (args), 0,
                          args, NULL);
}

static void
run (gchar      *name,
     gint        nparams,
     GimpParam  *param,
     gint       *nreturn_vals,
     GimpParam **return_vals)
{
  static GimpParam  values[1];
  GimpRunMode   run_mode;
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  const gchar *env_root_dir = NULL;
  gchar       *help_path    = NULL;
  gchar       *locale       = NULL;
  gchar       *help_file    = NULL;

  run_mode = param[0].data.d_int32;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  *nreturn_vals = 1;
  *return_vals = values;

  INIT_I18N_UI ();

  if (strcmp (name, GIMP_HELP_EXT_NAME) == 0)
    {
      switch (run_mode)
        {
        case GIMP_RUN_INTERACTIVE:
        case GIMP_RUN_NONINTERACTIVE:
	case GIMP_RUN_WITH_LAST_VALS:
	  /*  set default values  */
	  env_root_dir = g_getenv ("GIMP_HELP_ROOT");

	  if (env_root_dir)
	    {
	      if (chdir (env_root_dir) == -1)
		{
		  g_message (_("GIMP Help Browser Error.\n\n"
			       "Couldn't find GIMP_HELP_ROOT html directory.\n"
			       "(%s)"), env_root_dir);

		  status = GIMP_PDB_EXECUTION_ERROR;
		  break;
		}

	      gimp_help_root = g_strdup (env_root_dir);
	    }
	  else
	    {
	      gimp_help_root = g_build_filename (gimp_data_directory(),
                                                 GIMP_HELP_PREFIX, NULL);
	    }

	  help_path = g_strdup (gimp_help_root);
	  locale    = g_strdup ("C");
	  help_file = g_strdup ("introduction.html");
	  
	  /*  Make sure all the arguments are there!  */
	  if (nparams == 4)
	    {
	      if (param[1].data.d_string && strlen (param[1].data.d_string))
		{
		  g_free (help_path);
		  help_path = g_strdup (param[1].data.d_string);
		  g_strdelimit (help_path, "/", G_DIR_SEPARATOR);
		}
	      if (param[2].data.d_string && strlen (param[2].data.d_string))
		{
		  g_free (locale);
		  locale = g_strdup (param[2].data.d_string);
		}
	      if (param[3].data.d_string && strlen (param[3].data.d_string))
		{
		  g_free (help_file);
		  help_file = g_strdup (param[3].data.d_string);
		  g_strdelimit (help_file, "/", G_DIR_SEPARATOR);
		}
	    }
          break;

        default:
	  status = GIMP_PDB_CALLING_ERROR;
          break;
        }

      if (status == GIMP_PDB_SUCCESS)
        {
       	  if (!open_url (help_path, locale, help_file))
            values[0].data.d_status = GIMP_PDB_EXECUTION_ERROR;
	  else
	    values[0].data.d_status = GIMP_PDB_SUCCESS;

	  g_free (help_path);
	  g_free (locale);
	  g_free (help_file);
        }
      else
        values[0].data.d_status = status;
    }
  else
    values[0].data.d_status = GIMP_PDB_CALLING_ERROR;
}
