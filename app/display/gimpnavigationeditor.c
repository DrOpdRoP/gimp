/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpnavigationeditor.c
 * Copyright (C) 2001 Michael Natterer <mitch@gimp.org>
 *
 * partly based on app/nav_window
 * Copyright (C) 1999 Andy Thomas <alt@gimp.org>
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

#include "libgimpwidgets/gimpwidgets.h"

#include "display-types.h"

#include "config/gimpdisplayconfig.h"

#include "core/gimp.h"
#include "core/gimpcontext.h"
#include "core/gimpimage.h"

#include "widgets/gimpdocked.h"
#include "widgets/gimphelp-ids.h"
#include "widgets/gimpnavigationview.h"
#include "widgets/gimpviewrenderer.h"

#include "gimpdisplay.h"
#include "gimpdisplayshell.h"
#include "gimpdisplayshell-scale.h"
#include "gimpdisplayshell-scroll.h"
#include "gimpnavigationeditor.h"

#include "gimp-intl.h"


#define MAX_SCALE_BUF 20


static void   gimp_navigation_editor_class_init (GimpNavigationEditorClass *klass);
static void   gimp_navigation_editor_init       (GimpNavigationEditor      *editor);

static void   gimp_navigation_editor_docked_iface_init   (GimpDockedInterface *docked_iface);
static void   gimp_navigation_editor_set_context         (GimpDocked       *docked,
                                                           GimpContext      *context);

static void   gimp_navigation_editor_destroy             (GtkObject          *object);

static GtkWidget * gimp_navigation_editor_new_private    (GimpDisplayShell   *shell,
                                                          GimpDisplayConfig  *config,
                                                          gboolean            popup);

static gboolean gimp_navigation_editor_button_release    (GtkWidget          *widget,
                                                          GdkEventButton     *bevent,
                                                          GimpDisplayShell   *shell);
static void   gimp_navigation_editor_marker_changed      (GimpNavigationView   *view,
                                                          gdouble               x,
                                                          gdouble               y,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_zoom                (GimpNavigationView   *view,
                                                          GimpZoomType          direction,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_scroll              (GimpNavigationView   *view,
                                                          GdkScrollDirection    direction,
                                                          GimpNavigationEditor  *editor);

static void   gimp_navigation_editor_zoom_adj_changed    (GtkAdjustment        *adj,
                                                          GimpNavigationEditor *editor);

static void   gimp_navigation_editor_zoom_out_clicked    (GtkWidget            *widget,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_zoom_in_clicked     (GtkWidget            *widget,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_zoom_100_clicked    (GtkWidget            *widget,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_zoom_fit_in_clicked (GtkWidget            *widget,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_zoom_fit_to_clicked (GtkWidget            *widget,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_shrink_clicked      (GtkWidget            *widget,
                                                          GimpNavigationEditor *editor);

static void   gimp_navigation_editor_shell_scaled        (GimpDisplayShell     *shell,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_shell_scrolled      (GimpDisplayShell     *shell,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_shell_reconnect     (GimpDisplayShell     *shell,
                                                          GimpNavigationEditor *editor);
static void   gimp_navigation_editor_update_marker       (GimpNavigationEditor *editor);


static GimpEditorClass *parent_class = NULL;

GType
gimp_navigation_editor_get_type (void)
{
  static GType type = 0;

  if (! type)
    {
      static const GTypeInfo editor_info =
      {
        sizeof (GimpNavigationEditorClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) gimp_navigation_editor_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_navigation */
        sizeof (GimpNavigationEditor),
        0,              /* n_preallocs */
        (GInstanceInitFunc) gimp_navigation_editor_init,
      };
      static const GInterfaceInfo docked_iface_info =
      {
        (GInterfaceInitFunc) gimp_navigation_editor_docked_iface_init,
        NULL,           /* iface_finalize */
        NULL            /* iface_data     */
      };

      type = g_type_register_static (GIMP_TYPE_EDITOR,
                                     "GimpNavigationEditor",
                                     &editor_info, 0);

      g_type_add_interface_static (type, GIMP_TYPE_DOCKED,
                                   &docked_iface_info);
    }

  return type;
}

static void
gimp_navigation_editor_class_init (GimpNavigationEditorClass *klass)
{
  GObjectClass   *object_class;
  GtkObjectClass *gtk_object_class;

  object_class     = G_OBJECT_CLASS (klass);
  gtk_object_class = GTK_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  gtk_object_class->destroy = gimp_navigation_editor_destroy;
}

static void
gimp_navigation_editor_init (GimpNavigationEditor *editor)
{
  GtkWidget *frame;

  editor->shell = NULL;

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (editor), frame, TRUE, TRUE, 0);
  gtk_widget_show (frame);

  editor->view = gimp_view_new_by_types (GIMP_TYPE_NAVIGATION_VIEW,
                                         GIMP_TYPE_IMAGE,
                                         GIMP_PREVIEW_SIZE_MEDIUM, 0, TRUE);
  gtk_container_add (GTK_CONTAINER (frame), editor->view);
  gtk_widget_show (editor->view);

  g_signal_connect (editor->view, "marker_changed",
                    G_CALLBACK (gimp_navigation_editor_marker_changed),
                    editor);
  g_signal_connect (editor->view, "zoom",
                    G_CALLBACK (gimp_navigation_editor_zoom),
                    editor);
  g_signal_connect (editor->view, "scroll",
                    G_CALLBACK (gimp_navigation_editor_scroll),
                    editor);

  gtk_widget_set_sensitive (GTK_WIDGET (editor), FALSE);
}

static void
gimp_navigation_editor_docked_iface_init (GimpDockedInterface *docked_iface)
{
  docked_iface->set_context = gimp_navigation_editor_set_context;
}

static void
gimp_navigation_editor_context_changed (GimpContext          *context,
                                        GimpDisplay          *gdisp,
                                        GimpNavigationEditor *editor)
{
  GimpDisplayShell *shell = NULL;

  if (gdisp)
    shell = GIMP_DISPLAY_SHELL (gdisp->shell);

  gimp_navigation_editor_set_shell (editor, shell);
}

static void
gimp_navigation_editor_set_context (GimpDocked  *docked,
                                    GimpContext *context)
{
  GimpNavigationEditor *editor  = GIMP_NAVIGATION_EDITOR (docked);
  GimpDisplay          *gdisp = NULL;
  GimpDisplayShell     *shell = NULL;

  if (editor->context)
    {
      g_signal_handlers_disconnect_by_func (editor->context,
                                            gimp_navigation_editor_context_changed,
                                            editor);
    }

  editor->context = context;

  if (context)
    {
      g_signal_connect (context, "display_changed",
                        G_CALLBACK (gimp_navigation_editor_context_changed),
                        editor);

      gdisp = gimp_context_get_display (context);
    }

  if (gdisp)
    shell = GIMP_DISPLAY_SHELL (gdisp->shell);

  gimp_navigation_editor_set_shell (editor, shell);
}

static void
gimp_navigation_editor_destroy (GtkObject *object)
{
  GimpNavigationEditor *editor = GIMP_NAVIGATION_EDITOR (object);

  if (editor->shell)
    gimp_navigation_editor_set_shell (editor, NULL);

  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}


/*  public functions  */

GtkWidget *
gimp_navigation_editor_new (GimpDisplayShell  *shell,
                            GimpDisplayConfig *config)
{
  return gimp_navigation_editor_new_private (shell, config, FALSE);
}

void
gimp_navigation_editor_set_shell (GimpNavigationEditor *editor,
                                  GimpDisplayShell   *shell)
{
  g_return_if_fail (GIMP_IS_NAVIGATION_EDITOR (editor));
  g_return_if_fail (! shell || GIMP_IS_DISPLAY_SHELL (shell));

  if (shell == editor->shell)
    return;

  if (editor->shell)
    {
      g_signal_handlers_disconnect_by_func (editor->shell,
                                            gimp_navigation_editor_shell_scaled,
                                            editor);
      g_signal_handlers_disconnect_by_func (editor->shell,
                                            gimp_navigation_editor_shell_scrolled,
                                            editor);
      g_signal_handlers_disconnect_by_func (editor->shell,
                                            gimp_navigation_editor_shell_reconnect,
                                            editor);
    }
  else if (shell)
    {
      gtk_widget_set_sensitive (GTK_WIDGET (editor), TRUE);
    }

  editor->shell = shell;

  if (editor->shell)
    {
      gimp_view_set_viewable (GIMP_VIEW (editor->view),
                              GIMP_VIEWABLE (shell->gdisp->gimage));

      g_signal_connect (editor->shell, "scaled",
                        G_CALLBACK (gimp_navigation_editor_shell_scaled),
                        editor);
      g_signal_connect (editor->shell, "scrolled",
                        G_CALLBACK (gimp_navigation_editor_shell_scrolled),
                        editor);
      g_signal_connect (editor->shell, "reconnect",
                        G_CALLBACK (gimp_navigation_editor_shell_reconnect),
                        editor);

      gimp_navigation_editor_shell_scaled (editor->shell, editor);
    }
  else
    {
      gimp_view_set_viewable (GIMP_VIEW (editor->view), NULL);
      gtk_widget_set_sensitive (GTK_WIDGET (editor), FALSE);
    }
}

void
gimp_navigation_editor_popup (GimpDisplayShell *shell,
                              GtkWidget        *widget,
                              gint              click_x,
                              gint              click_y)
{
  GimpNavigationEditor *editor;
  GimpNavigationView   *view;
  GdkScreen            *screen;
  gint                  x, y;
  gint                  x_org, y_org;

  g_return_if_fail (GIMP_IS_DISPLAY_SHELL (shell));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  if (! shell->nav_popup)
    {
      GimpDisplayConfig *config;
      GtkWidget         *frame;

      shell->nav_popup = gtk_window_new (GTK_WINDOW_POPUP);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
      gtk_container_add (GTK_CONTAINER (shell->nav_popup), frame);
      gtk_widget_show (frame);

      config = GIMP_DISPLAY_CONFIG (shell->gdisp->gimage->gimp->config);

      editor =
        GIMP_NAVIGATION_EDITOR (gimp_navigation_editor_new_private (shell,
                                                                    config,
                                                                    TRUE));
      gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET (editor));
      gtk_widget_show (GTK_WIDGET (editor));

      g_signal_connect (editor->view, "button_release_event",
                        G_CALLBACK (gimp_navigation_editor_button_release),
                        shell);
    }
  else
    {
      editor = GIMP_NAVIGATION_EDITOR (GTK_BIN (GTK_BIN (shell->nav_popup)->child)->child);
    }

  screen = gtk_widget_get_screen (widget);

  gtk_window_set_screen (GTK_WINDOW (shell->nav_popup), screen);

  view = GIMP_NAVIGATION_VIEW (editor->view);

  /* decide where to put the popup */
  gdk_window_get_origin (widget->window, &x_org, &y_org);

#define BORDER_PEN_WIDTH  3

  x = (x_org + click_x -
       view->p_x -
       0.5 * (view->p_width  - BORDER_PEN_WIDTH) -
       2   * widget->style->xthickness);

  y = (y_org + click_y -
       view->p_y -
       0.5 * (view->p_height - BORDER_PEN_WIDTH) -
       2   * widget->style->ythickness);

  /* If the popup doesn't fit into the screen, we have a problem.
   * We move the popup onscreen and risk that the pointer is not
   * in the square representing the viewable area anymore. Moving
   * the pointer will make the image scroll by a large amount,
   * but then it works as usual. Probably better than a popup that
   * is completely unusable in the lower right of the screen.
   *
   * Warping the pointer would be another solution ...
   */

  x = CLAMP (x, 0, (gdk_screen_get_width (screen)  -
                    GIMP_VIEW (view)->renderer->width  -
                    4 * widget->style->xthickness));
  y = CLAMP (y, 0, (gdk_screen_get_height (screen) -
                    GIMP_VIEW (view)->renderer->height -
                    4 * widget->style->ythickness));

  gtk_window_move (GTK_WINDOW (shell->nav_popup), x, y);
  gtk_widget_show (shell->nav_popup);

  gdk_flush ();

  /* fill in then grab pointer */
  view->motion_offset_x = 0.5 * (view->p_width  - BORDER_PEN_WIDTH);
  view->motion_offset_y = 0.5 * (view->p_height - BORDER_PEN_WIDTH);

#undef BORDER_PEN_WIDTH

  gimp_navigation_view_grab_pointer (view);
}


/*  private functions  */

static GtkWidget *
gimp_navigation_editor_new_private (GimpDisplayShell  *shell,
                                    GimpDisplayConfig *config,
                                    gboolean           popup)
{
  GimpNavigationEditor *editor;

  g_return_val_if_fail (! shell || GIMP_IS_DISPLAY_SHELL (shell), NULL);
  g_return_val_if_fail (GIMP_IS_DISPLAY_CONFIG (config), NULL);
  g_return_val_if_fail (! popup || (popup && shell), NULL);

  editor = g_object_new (GIMP_TYPE_NAVIGATION_EDITOR, NULL);

  if (popup)
    {
      GimpView *view = GIMP_VIEW (editor->view);

      gimp_view_renderer_set_size (view->renderer,
                                   config->nav_preview_size * 3,
                                   view->renderer->border_width);
    }
  else
    {
      GtkWidget *hscale;

      gtk_widget_set_size_request (editor->view,
                                   GIMP_PREVIEW_SIZE_HUGE,
                                   GIMP_PREVIEW_SIZE_HUGE);
      gimp_view_set_expand (GIMP_VIEW (editor->view), TRUE);

      /* the editor buttons */

      editor->zoom_out_button =
        gimp_editor_add_button (GIMP_EDITOR (editor),
                                GTK_STOCK_ZOOM_OUT, _("Zoom out"),
                                GIMP_HELP_VIEW_ZOOM_OUT,
                                G_CALLBACK (gimp_navigation_editor_zoom_out_clicked),
                                NULL,
                                editor);

      editor->zoom_in_button =
        gimp_editor_add_button (GIMP_EDITOR (editor),
                                GTK_STOCK_ZOOM_IN, _("Zoom in"),
                                GIMP_HELP_VIEW_ZOOM_IN,
                                G_CALLBACK (gimp_navigation_editor_zoom_in_clicked),
                                NULL,
                                editor);

      editor->zoom_100_button =
        gimp_editor_add_button (GIMP_EDITOR (editor),
                                GTK_STOCK_ZOOM_100, _("Zoom 1:1"),
                                GIMP_HELP_VIEW_ZOOM_100,
                                G_CALLBACK (gimp_navigation_editor_zoom_100_clicked),
                                NULL,
                                editor);

      editor->zoom_fit_in_button =
        gimp_editor_add_button (GIMP_EDITOR (editor),
                                GTK_STOCK_ZOOM_FIT, _("Fit Image in Window"),
                                GIMP_HELP_VIEW_ZOOM_FIT_IN,
                                G_CALLBACK (gimp_navigation_editor_zoom_fit_in_clicked),
                                NULL,
                                editor);

      editor->zoom_fit_to_button =
        gimp_editor_add_button (GIMP_EDITOR (editor),
                                GTK_STOCK_ZOOM_FIT, _("Fit Image to Window"),
                                GIMP_HELP_VIEW_ZOOM_FIT_TO,
                                G_CALLBACK (gimp_navigation_editor_zoom_fit_to_clicked),
                                NULL,
                                editor);

      editor->shrink_wrap_button =
        gimp_editor_add_button (GIMP_EDITOR (editor),
                                GTK_STOCK_ZOOM_FIT, _("Shrink Wrap"),
                                GIMP_HELP_VIEW_SHRINK_WRAP,
                                G_CALLBACK (gimp_navigation_editor_shrink_clicked),
                                NULL,
                                editor);

      /* the zoom scale */

      editor->zoom_adjustment =
        GTK_ADJUSTMENT (gtk_adjustment_new (0.0, -8.0, 8.0, 0.5, 1.0, 0.0));

      g_signal_connect (editor->zoom_adjustment, "value_changed",
                        G_CALLBACK (gimp_navigation_editor_zoom_adj_changed),
                        editor);

      hscale = gtk_hscale_new (GTK_ADJUSTMENT (editor->zoom_adjustment));
      gtk_range_set_update_policy (GTK_RANGE (hscale), GTK_UPDATE_DELAYED);
      gtk_scale_set_draw_value (GTK_SCALE (hscale), FALSE);
      gtk_scale_set_digits (GTK_SCALE (hscale), 2);
      gtk_box_pack_end (GTK_BOX (editor), hscale, FALSE, FALSE, 0);
      gtk_widget_show (hscale);

      /* the zoom label */

      editor->zoom_label = gtk_label_new ("100%");
      gtk_box_pack_end (GTK_BOX (editor), editor->zoom_label, FALSE, FALSE, 0);
      gtk_widget_show (editor->zoom_label);

      /* eek */
      {
        GtkRequisition requisition;

        gtk_widget_size_request (editor->zoom_label, &requisition);
        gtk_widget_set_size_request (editor->zoom_label,
                                     4 * requisition.width,
                                     requisition.height);
      }
    }

  if (shell)
    gimp_navigation_editor_set_shell (editor, shell);

  gimp_view_renderer_set_background (GIMP_VIEW (editor->view)->renderer,
                                     GIMP_STOCK_TEXTURE);

  return GTK_WIDGET (editor);
}

static gboolean
gimp_navigation_editor_button_release (GtkWidget        *widget,
                                       GdkEventButton   *bevent,
                                       GimpDisplayShell *shell)
{
  if (bevent->button == 1)
    {
      gtk_widget_hide (shell->nav_popup);
    }

  return FALSE;
}

static void
gimp_navigation_editor_marker_changed (GimpNavigationView   *view,
                                       gdouble               x,
                                       gdouble               y,
                                       GimpNavigationEditor *editor)
{
  if (editor->shell)
    {
      gdouble xratio;
      gdouble yratio;
      gint    xoffset;
      gint    yoffset;

      xratio = SCALEFACTOR_X (editor->shell);
      yratio = SCALEFACTOR_Y (editor->shell);

      xoffset = RINT (x * xratio - editor->shell->offset_x);
      yoffset = RINT (y * yratio - editor->shell->offset_y);

      gimp_display_shell_scroll (editor->shell, xoffset, yoffset);
    }
}

static void
gimp_navigation_editor_zoom (GimpNavigationView   *view,
                             GimpZoomType          direction,
                             GimpNavigationEditor *editor)
{
  g_return_if_fail (direction != GIMP_ZOOM_TO);

  if (editor->shell)
    {
      gimp_display_shell_scale (editor->shell, direction, 0.0);
    }
}

static void
gimp_navigation_editor_scroll (GimpNavigationView   *view,
                               GdkScrollDirection    direction,
                               GimpNavigationEditor *editor)
{
  if (editor->shell)
    {
      GtkAdjustment *adj = NULL;
      gdouble        value;

      switch (direction)
        {
        case GDK_SCROLL_LEFT:
        case GDK_SCROLL_RIGHT:
          adj = editor->shell->hsbdata;
          break;

        case GDK_SCROLL_UP:
        case GDK_SCROLL_DOWN:
          adj = editor->shell->vsbdata;
          break;
        }

      g_assert (adj != NULL);

      value = adj->value;

      switch (direction)
        {
        case GDK_SCROLL_LEFT:
        case GDK_SCROLL_UP:
          value -= adj->page_increment / 2;
          break;

        case GDK_SCROLL_RIGHT:
        case GDK_SCROLL_DOWN:
          value += adj->page_increment / 2;
          break;
        }

      value = CLAMP (value, adj->lower, adj->upper - adj->page_size);

      gtk_adjustment_set_value (adj, value);
    }
}

static void
gimp_navigation_editor_zoom_adj_changed (GtkAdjustment        *adj,
                                         GimpNavigationEditor *editor)
{
  gimp_display_shell_scale (editor->shell, GIMP_ZOOM_TO,
                            pow (2.0, adj->value));
}

static void
gimp_navigation_editor_zoom_out_clicked (GtkWidget            *widget,
                                         GimpNavigationEditor *editor)
{
  if (editor->shell)
    gimp_display_shell_scale (editor->shell, GIMP_ZOOM_OUT, 0.0);
}

static void
gimp_navigation_editor_zoom_in_clicked (GtkWidget            *widget,
                                        GimpNavigationEditor *editor)
{
  if (editor->shell)
    gimp_display_shell_scale (editor->shell, GIMP_ZOOM_IN, 0.0);
}

static void
gimp_navigation_editor_zoom_100_clicked (GtkWidget            *widget,
                                         GimpNavigationEditor *editor)
{
  if (editor->shell)
    gimp_display_shell_scale (editor->shell, GIMP_ZOOM_TO, 1.0);
}

static void
gimp_navigation_editor_zoom_fit_in_clicked (GtkWidget            *widget,
                                            GimpNavigationEditor *editor)
{
  if (editor->shell)
    gimp_display_shell_scale_fit_in (editor->shell);
}

static void
gimp_navigation_editor_zoom_fit_to_clicked (GtkWidget            *widget,
                                            GimpNavigationEditor *editor)
{
  if (editor->shell)
    gimp_display_shell_scale_fit_to (editor->shell);
}

static void
gimp_navigation_editor_shrink_clicked (GtkWidget            *widget,
                                       GimpNavigationEditor *editor)
{
  if (editor->shell)
    gimp_display_shell_scale_shrink_wrap (editor->shell);
}

static void
gimp_navigation_editor_shell_scaled (GimpDisplayShell     *shell,
                                     GimpNavigationEditor *editor)
{
  if (editor->zoom_label)
    {
      gchar scale_str[MAX_SCALE_BUF];

      /* Update the zoom scale string */
      g_snprintf (scale_str, sizeof (scale_str),
                  shell->scale >= 0.15 ? "%.0f%%" : "%.2f%%",
                  editor->shell->scale * 100);

      gtk_label_set_text (GTK_LABEL (editor->zoom_label), scale_str);
    }

  if (editor->zoom_adjustment)
    {
      gdouble val;
  
      val = log (CLAMP (editor->shell->scale, 1.0 / 256, 256.0) ) / G_LN2;

      g_signal_handlers_block_by_func (editor->zoom_adjustment,
                                       gimp_navigation_editor_zoom_adj_changed,
                                       editor);

      gtk_adjustment_set_value (editor->zoom_adjustment, val);

      g_signal_handlers_unblock_by_func (editor->zoom_adjustment,
                                         gimp_navigation_editor_zoom_adj_changed,
                                         editor);
    }

  gimp_navigation_editor_update_marker (editor);
}

static void
gimp_navigation_editor_shell_scrolled (GimpDisplayShell     *shell,
                                       GimpNavigationEditor *editor)
{
  gimp_navigation_editor_update_marker (editor);
}

static void
gimp_navigation_editor_shell_reconnect (GimpDisplayShell     *shell,
                                        GimpNavigationEditor *editor)
{
  gimp_view_set_viewable (GIMP_VIEW (editor->view),
                          GIMP_VIEWABLE (shell->gdisp->gimage));
}

static void
gimp_navigation_editor_update_marker (GimpNavigationEditor *editor)
{
  GimpViewRenderer *renderer;
  gdouble           xratio;
  gdouble           yratio;

  renderer = GIMP_VIEW (editor->view)->renderer;

  xratio = SCALEFACTOR_X (editor->shell);
  yratio = SCALEFACTOR_Y (editor->shell);

  if (renderer->dot_for_dot != editor->shell->dot_for_dot)
    gimp_view_renderer_set_dot_for_dot (renderer,
                                        editor->shell->dot_for_dot);

  gimp_navigation_view_set_marker (GIMP_NAVIGATION_VIEW (editor->view),
                                   editor->shell->offset_x    / xratio,
                                   editor->shell->offset_y    / yratio,
                                   editor->shell->disp_width  / xratio,
                                   editor->shell->disp_height / yratio);
}
