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

#include <stdlib.h>
#include <string.h>
#include "appenv.h"
#include "color_area.h"
#include "color_notebook.h"
#include "colormaps.h"
#include "gimpdnd.h"
#include "palette.h"

typedef enum
{
  FORE_AREA,
  BACK_AREA,
  SWAP_AREA,
  DEF_AREA,
  INVALID_AREA
} ColorAreaTarget;

/*  local function prototypes  */
static void color_area_drag_begin  (GtkWidget *, GdkDragContext *, gpointer);
static void color_area_drag_end    (GtkWidget *, GdkDragContext *, gpointer);
static void color_area_drop_handle (GtkWidget        *widget, 
				    GdkDragContext   *context,
				    gint              x,
				    gint              y,
				    GtkSelectionData *selection_data,
				    guint             info,
				    guint             time,
				    gpointer          data);
static void color_area_drag_handle (GtkWidget        *widget, 
				    GdkDragContext   *context,
				    GtkSelectionData *selection_data,
				    guint             info,
				    guint             time,
				    gpointer          data);

/*  Global variables  */
gint active_color = 0;

/*  Static variables  */
static GdkGC          *color_area_gc = NULL;
static GtkWidget      *color_area = NULL;
static GdkPixmap      *color_area_pixmap = NULL;
static GdkPixmap      *default_pixmap = NULL;
static GdkPixmap      *swap_pixmap = NULL;
static ColorNotebookP  color_notebook = NULL;
static gboolean        color_notebook_active = FALSE;
static gint            edit_color;
static guchar          revert_fg_r, revert_fg_g, revert_fg_b;
static guchar          revert_bg_r, revert_bg_g, revert_bg_b;

/*  dnd structures  */
static GtkTargetEntry color_area_target_table[] =
{
  GIMP_TARGET_COLOR
};
static guint n_color_area_targets = (sizeof (color_area_target_table) /
				     sizeof (color_area_target_table[0]));

/*  Local functions  */
static ColorAreaTarget
color_area_target (gint x,
		   gint y)
{
  gint rect_w, rect_h;
  gint width, height;

  gdk_window_get_size (color_area_pixmap, &width, &height);

  rect_w = width * 0.65;
  rect_h = height * 0.65;

  /*  foreground active  */
  if (x > 0 && x < rect_w &&
      y > 0 && y < rect_h)
    return FORE_AREA;
  else if (x > (width - rect_w) && x < width &&
	   y > (height - rect_h) && y < height)
    return BACK_AREA;
  else if (x > 0 && x < (width - rect_w) &&
	   y > rect_h && y < height)
    return DEF_AREA;
  else if (x > rect_w && x < width &&
	   y > 0 && y < (height - rect_h))
    return SWAP_AREA;
  else
    return -1;
}

void
color_area_draw_rect (GdkDrawable *drawable,
		      GdkGC       *gc,
		      gint         x,
		      gint         y,
		      gint         width,
		      gint         height,
		      guchar       r,
		      guchar       g,
		      guchar       b)
{
  static guchar *color_area_rgb_buf = NULL;
  static gint color_area_rgb_buf_size;
  static gint rowstride;
  gint xx, yy;
  guchar *bp;

  rowstride = 3 * ((width + 3) & -4);
  if (color_area_rgb_buf == NULL ||
      color_area_rgb_buf_size < height * rowstride)
    {
      if (color_area_rgb_buf)
	g_free (color_area_rgb_buf);
      color_area_rgb_buf = g_malloc (rowstride * height);
    }
  bp = color_area_rgb_buf;
  for (xx = 0; xx < width; xx++)
    {
      *bp++ = r;
      *bp++ = g;
      *bp++ = b;
    }
  bp = color_area_rgb_buf;
  for (yy = 1; yy < height; yy++)
    {
      bp += rowstride;
      memcpy (bp, color_area_rgb_buf, rowstride);
    }
  gdk_draw_rgb_image (drawable, gc, x, y, width, height,
		      GDK_RGB_DITHER_MAX,
		      color_area_rgb_buf,
		      rowstride);
}

static void
color_area_draw (void)
{
  GdkColor *win_bg;
  GdkColor fg, bg, bd;
  gint rect_w, rect_h;
  gint width, height;
  gint def_width, def_height;
  gint swap_width, swap_height;
  guchar r, g, b;

  /* Check we haven't gotten initial expose yet,
   * no point in drawing anything */
  if (!color_area_pixmap || !color_area_gc)
    return;

  gdk_window_get_size (color_area_pixmap, &width, &height);

  win_bg = &(color_area->style->bg[GTK_STATE_NORMAL]);
  fg.pixel = foreground_pixel;
  bg.pixel = background_pixel;
  bd.pixel = g_black_pixel;

  rect_w = width * 0.65;
  rect_h = height * 0.65;

  gdk_gc_set_foreground (color_area_gc, win_bg);
  gdk_draw_rectangle (color_area_pixmap, color_area_gc, 1,
		      0, 0, width, height);

#ifdef OLD_COLOR_AREA
  gdk_gc_set_foreground (color_area_gc, &bg);
  gdk_draw_rectangle (color_area_pixmap, color_area_gc, 1,
		      (width - rect_w), (height - rect_h), rect_w, rect_h);
#else
  palette_get_background (&r, &g, &b);
  color_area_draw_rect (color_area_pixmap, color_area_gc,
			(width - rect_w), (height - rect_h), rect_w, rect_h,
			r, g, b);
#endif

  if (active_color == FOREGROUND)
    gtk_draw_shadow (color_area->style, color_area_pixmap, GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		     (width - rect_w), (height - rect_h), rect_w, rect_h);
  else
    gtk_draw_shadow (color_area->style, color_area_pixmap, GTK_STATE_NORMAL, GTK_SHADOW_IN,
		     (width - rect_w), (height - rect_h), rect_w, rect_h);

#ifdef OLD_COLOR_AREA
  gdk_gc_set_foreground (color_area_gc, &fg);
  gdk_draw_rectangle (color_area_pixmap, color_area_gc, 1,
		      0, 0, rect_w, rect_h);
#else
  palette_get_foreground (&r, &g, &b);
  color_area_draw_rect (color_area_pixmap, color_area_gc,
			0, 0, rect_w, rect_h,
			r, g, b);
#endif

  if (active_color == FOREGROUND)
    gtk_draw_shadow (color_area->style, color_area_pixmap, GTK_STATE_NORMAL, GTK_SHADOW_IN,
		     0, 0, rect_w, rect_h);
  else
    gtk_draw_shadow (color_area->style, color_area_pixmap, GTK_STATE_NORMAL, GTK_SHADOW_OUT,
		     0, 0, rect_w, rect_h);


  gdk_window_get_size (default_pixmap, &def_width, &def_height);
  gdk_draw_pixmap (color_area_pixmap, color_area_gc, default_pixmap,
		   0, 0, 0, height - def_height, def_width, def_height);

  gdk_window_get_size (swap_pixmap, &swap_width, &swap_height);
  gdk_draw_pixmap (color_area_pixmap, color_area_gc, swap_pixmap,
		   0, 0, width - swap_width, 0, swap_width, swap_height);

  gdk_draw_pixmap (color_area->window, color_area_gc, color_area_pixmap,
		   0, 0, 0, 0, width, height);
}

static void
color_area_select_callback (gint                r,
			    gint                g,
			    gint                b,
			    ColorNotebookState  state,
			    void               *client_data)
{
  if (color_notebook)
    {
      switch (state)
	{
	case COLOR_NOTEBOOK_OK:
	  color_notebook_hide (color_notebook);
	  color_notebook_active = FALSE;
	  /* Fallthrough */
	case COLOR_NOTEBOOK_UPDATE:
	  if (edit_color == FOREGROUND)
	    palette_set_foreground (r, g, b);
	  else
	    palette_set_background (r, g, b);
	  break;
	case COLOR_NOTEBOOK_CANCEL:
	  color_notebook_hide (color_notebook);
	  color_notebook_active = FALSE;
	  palette_set_foreground (revert_fg_r, revert_fg_g, revert_fg_b);
	  palette_set_background (revert_bg_r, revert_bg_g, revert_bg_b);
	}
    }
}

static void
color_area_edit (void)
{
  guchar r, g, b;

  if (!color_notebook_active)
    {
      palette_get_foreground (&revert_fg_r, &revert_fg_g, &revert_fg_b);
      palette_get_background (&revert_bg_r, &revert_bg_g, &revert_bg_b);
    }
  if (active_color == FOREGROUND)
    {
      palette_get_foreground (&r, &g, &b);
      edit_color = FOREGROUND;
    }
  else
    {
      palette_get_background (&r, &g, &b);
      edit_color = BACKGROUND;
    }

  if (! color_notebook)
    {
      color_notebook = color_notebook_new (r, g, b, color_area_select_callback,
					   NULL, TRUE);
      color_notebook_active = TRUE;
    }
  else
    {
      if (! color_notebook_active)
	{
	  color_notebook_show (color_notebook);
	  color_notebook_active = TRUE;
	}
      else 
	gdk_window_raise (color_notebook->shell->window);

      color_notebook_set_color (color_notebook, r, g, b, TRUE);
    }
}

static gint
color_area_events (GtkWidget *widget,
		   GdkEvent  *event)
{
  GdkEventButton  *bevent;
  ColorAreaTarget  target;

  static ColorAreaTarget press_target = INVALID_AREA;

  switch (event->type)
    {
    case GDK_CONFIGURE:
      if (color_area_pixmap)
	gdk_pixmap_unref (color_area_pixmap);

      color_area_pixmap = gdk_pixmap_new (widget->window,
					  widget->allocation.width,
					  widget->allocation.height, -1);
      break;

    case GDK_EXPOSE:
      if (GTK_WIDGET_DRAWABLE (widget))
	{
	  if (!color_area_gc)
	    color_area_gc = gdk_gc_new (widget->window);

	  color_area_draw ();
	}
      break;

    case GDK_BUTTON_PRESS:
      bevent = (GdkEventButton *) event;

      if (bevent->button == 1)
	press_target = color_area_target (bevent->x, bevent->y);
      break;

    case GDK_BUTTON_RELEASE:
      bevent = (GdkEventButton *) event;

      if (bevent->button == 1)
	{
	  target = color_area_target (bevent->x, bevent->y);

	  if (target == press_target)
	    {
	      switch (target)
		{
		case FORE_AREA:
		case BACK_AREA:
		  if (target == active_color)
		    {
		      color_area_edit ();
		    }
		  else
		    {
		      active_color = target;
		      color_area_draw ();
		    }
		  break;
		case SWAP_AREA:
		  palette_swap_colors();
		  color_area_draw ();
		  break;
		case DEF_AREA:
		  palette_set_default_colors();
		  color_area_draw ();
		  break;
		default:
		  break;
		}
	    }
	  press_target = INVALID_AREA;
	}
      break;

    case GDK_LEAVE_NOTIFY:
      press_target = INVALID_AREA;
      break;

    default:
      break;
    }

  return FALSE;
}

GtkWidget *
color_area_create (gint       width,
		   gint       height,
		   GdkPixmap *default_pmap,
		   GdkPixmap *swap_pmap)
{
  color_area = gtk_drawing_area_new ();
  gtk_drawing_area_size (GTK_DRAWING_AREA (color_area), width, height);
  gtk_widget_set_events (color_area,
			 GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK |
			 GDK_BUTTON_RELEASE_MASK |
			 GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
  gtk_signal_connect (GTK_OBJECT (color_area), "event",
		      (GtkSignalFunc) color_area_events,
		      NULL);
  default_pixmap = default_pmap;
  swap_pixmap    = swap_pmap;

  /*  dnd stuff  */
  gtk_drag_dest_set (color_area,
		     GTK_DEST_DEFAULT_HIGHLIGHT |
		     GTK_DEST_DEFAULT_MOTION |
		     GTK_DEST_DEFAULT_DROP,
		     color_area_target_table, n_color_area_targets,
		     GDK_ACTION_COPY);

  gtk_drag_source_set (color_area,
                       GDK_BUTTON1_MASK | GDK_BUTTON3_MASK,
                       color_area_target_table, n_color_area_targets,
                       GDK_ACTION_COPY | GDK_ACTION_MOVE);

  gtk_signal_connect (GTK_OBJECT (color_area),
                      "drag_begin",
                      GTK_SIGNAL_FUNC (color_area_drag_begin),
                      color_area);
  gtk_signal_connect (GTK_OBJECT (color_area),
                      "drag_end",
                      GTK_SIGNAL_FUNC (color_area_drag_end),
                      color_area);
  gtk_signal_connect (GTK_OBJECT (color_area),
                      "drag_data_get",
                      GTK_SIGNAL_FUNC (color_area_drag_handle),
                      color_area);
  gtk_signal_connect (GTK_OBJECT (color_area),
                      "drag_data_received",
                      GTK_SIGNAL_FUNC (color_area_drop_handle),
                      color_area);

  return color_area;
}

void
color_area_update ()
{
  color_area_draw ();
}

static void
color_area_drag_begin (GtkWidget      *widget,
		       GdkDragContext *context,
		       gpointer        data)
{
  GtkWidget *window;
  GdkColor bg;
  guchar r, g, b;

  window = gtk_window_new (GTK_WINDOW_POPUP);
  gtk_widget_set_app_paintable (GTK_WIDGET (window), TRUE);
  gtk_widget_set_usize (window, 48, 32);
  gtk_widget_realize (window);
  gtk_object_set_data_full (GTK_OBJECT (widget),
                            "gimp-color-area-drag-window",
                            window,
                            (GtkDestroyNotify) gtk_widget_destroy);

  if (active_color == FOREGROUND)
    palette_get_foreground (&r, &g, &b);
  else
    palette_get_background (&r, &g, &b);

  bg.red   = 0xff * r;
  bg.green = 0xff * g;
  bg.blue  = 0xff * b;

  gdk_color_alloc (gtk_widget_get_colormap (window), &bg);
  gdk_window_set_background (window->window, &bg);

  gtk_drag_set_icon_widget (context, window, -2, -2);
}

static void
color_area_drag_end (GtkWidget      *widget,
		      GdkDragContext *context,
		      gpointer        data)
{
  gtk_object_set_data (GTK_OBJECT (widget),
		       "gimp-color-area-drag-window", NULL);
}

static void
color_area_drop_handle (GtkWidget        *widget, 
			 GdkDragContext   *context,
			 gint              x,
			 gint              y,
			 GtkSelectionData *selection_data,
			 guint             info,
			 guint             time,
			 gpointer          data)
{
  guint16 *vals;
  guchar r, g, b;

  if (selection_data->length < 0)
    return;

  if ((selection_data->format != 16) || 
      (selection_data->length != 8))
    {
      g_warning ("Received invalid color data\n");
      return;
    }
  
  vals = (guint16 *) selection_data->data;

  r = vals[0] / 0xff;
  g = vals[1] / 0xff;
  b = vals[2] / 0xff;

  if (color_notebook_active &&
      active_color == edit_color)
    {
      color_notebook_set_color (color_notebook, r, g, b, TRUE);
    }
  else
    {
      if (active_color == FOREGROUND)
	palette_set_foreground (r, g, b);
      else
	palette_set_background (r, g, b);
    }
}

static void
color_area_drag_handle (GtkWidget        *widget, 
			 GdkDragContext   *context,
			 GtkSelectionData *selection_data,
			 guint             info,
			 guint             time,
			 gpointer          data)
{
  guint16 vals[4];
  guchar r, g, b;

  if (active_color == FOREGROUND)
    palette_get_foreground (&r, &g, &b);
  else
    palette_get_background (&r, &g, &b);

  vals[0] = r * 0xff;
  vals[1] = g * 0xff;
  vals[2] = b * 0xff;
  vals[3] = 0xffff;

  gtk_selection_data_set (selection_data,
                          gdk_atom_intern ("application/x-color", FALSE),
                          16, (guchar *) vals, 8);
}
