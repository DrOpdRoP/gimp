/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
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

#include <gtk/gtk.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "actions-types.h"

#include "core/gimp.h"

#include "widgets/gimptexteditor.h"
#include "widgets/gimphelp-ids.h"
#include "widgets/gimpuimanager.h"
#include "widgets/gimpwidgets-utils.h"

#include "text-editor-commands.h"

#include "gimp-intl.h"


/*  local function prototypes  */

static void   text_editor_load_response (GtkWidget      *dialog,
                                         gint            response_id,
                                         GimpTextEditor *editor);


/*  public functions  */

void
text_editor_load_cmd_callback (GtkAction *action,
                               gpointer   data)
{
  GimpTextEditor *editor = GIMP_TEXT_EDITOR (data);
  GtkFileChooser *chooser;

  if (editor->file_dialog)
    {
      gtk_window_present (GTK_WINDOW (editor->file_dialog));
      return;
    }

  editor->file_dialog =
    gtk_file_chooser_dialog_new (_("Open Text File (UTF-8)"),
                                 GTK_WINDOW (editor),
                                 GTK_FILE_CHOOSER_ACTION_OPEN,

                                 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                 GTK_STOCK_OPEN,   GTK_RESPONSE_OK,

                                 NULL);

  chooser = GTK_FILE_CHOOSER (editor->file_dialog);

  gtk_dialog_set_alternative_button_order (GTK_DIALOG (editor->file_dialog),
                                           GTK_RESPONSE_OK,
                                           GTK_RESPONSE_CANCEL,
                                           -1);

  g_object_add_weak_pointer (G_OBJECT (chooser),
                             (gpointer) &editor->file_dialog);

  gtk_window_set_role (GTK_WINDOW (chooser), "gimp-text-load-file");
  gtk_window_set_position (GTK_WINDOW (chooser), GTK_WIN_POS_MOUSE);
  gtk_window_set_destroy_with_parent (GTK_WINDOW (chooser), TRUE);

  g_signal_connect (chooser, "response",
                    G_CALLBACK (text_editor_load_response),
                    editor);
  g_signal_connect (chooser, "delete-event",
                    G_CALLBACK (gtk_true),
                    NULL);

  gtk_widget_show (GTK_WIDGET (chooser));
}

void
text_editor_clear_cmd_callback (GtkAction *action,
                                gpointer   data)
{
  GimpTextEditor *editor = GIMP_TEXT_EDITOR (data);
  GtkTextBuffer  *buffer;

  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->view));

  gtk_text_buffer_set_text (buffer, "", 0);
}

void
text_editor_direction_cmd_callback (GtkAction *action,
                                    GtkAction *current,
                                    gpointer   data)
{
  GimpTextEditor *editor = GIMP_TEXT_EDITOR (data);
  gint            value;

  value = gtk_radio_action_get_current_value (GTK_RADIO_ACTION (action));

  gimp_text_editor_set_direction (editor, (GimpTextDirection) value);
}


/*  private functions  */

static void
text_editor_load_response (GtkWidget      *dialog,
                           gint            response_id,
                           GimpTextEditor *editor)
{
  if (response_id == GTK_RESPONSE_OK)
    {
      GtkTextBuffer *buffer;
      gchar         *filename;
      GError        *error = NULL;

      buffer   = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->view));
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      if (! gimp_text_buffer_load (buffer, filename, &error))
        {
          gimp_message (editor->ui_manager->gimp, G_OBJECT (dialog),
                        GIMP_MESSAGE_ERROR,
                        _("Could not open '%s' for reading: %s"),
                        gimp_filename_to_utf8 (filename),
                        error->message);
          g_clear_error (&error);
          g_free (filename);
          return;
        }

      g_free (filename);
    }

  gtk_widget_destroy (dialog);
}
