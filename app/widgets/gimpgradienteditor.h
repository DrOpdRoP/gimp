/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * Gradient editor module copyight (C) 1996-1997 Federico Mena Quintero
 * federico@nuclecu.unam.mx
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

#ifndef __GIMP_GRADIENT_EDITOR_H__
#define __GIMP_GRADIENT_EDITOR_H__


#include "gimpdataeditor.h"


#define GRAD_NUM_COLORS 10


typedef enum
{
  GRAD_DRAG_NONE = 0,
  GRAD_DRAG_LEFT,
  GRAD_DRAG_MIDDLE,
  GRAD_DRAG_ALL
} GradientEditorDragMode;


#define GIMP_TYPE_GRADIENT_EDITOR            (gimp_gradient_editor_get_type ())
#define GIMP_GRADIENT_EDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_GRADIENT_EDITOR, GimpGradientEditor))
#define GIMP_GRADIENT_EDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_GRADIENT_EDITOR, GimpGradientEditorClass))
#define GIMP_IS_GRADIENT_EDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_GRADIENT_EDITOR))
#define GIMP_IS_GRADIENT_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_GRADIENT_EDITOR))
#define GIMP_GRADIENT_EDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_GRADIENT_EDITOR, GimpGradientEditorClass))


typedef struct _GimpGradientEditorClass GimpGradientEditorClass;

struct _GimpGradientEditor
{
  GimpDataEditor  parent_instance;

  GtkWidget      *hint_label1;
  GtkWidget      *hint_label2;
  GtkWidget      *hint_label3;
  GtkWidget      *hint_label4;
  GtkWidget      *scrollbar;
  GtkWidget      *view;
  GtkWidget      *control;

  /*  Zoom and scrollbar  */
  guint           zoom_factor;
  GtkObject      *scroll_data;

  /*  Instant update  */
  gboolean        instant_update;

  /*  Color dialog  */
  GtkWidget      *color_dialog;

  /*  Gradient view  */
  gint            view_last_x;
  gboolean        view_button_down;

  /*  Gradient control  */
  GdkPixmap              *control_pixmap;
  GimpGradientSegment    *control_drag_segment; /* Segment which is being dragged */
  GimpGradientSegment    *control_sel_l;        /* Left segment of selection */
  GimpGradientSegment    *control_sel_r;        /* Right segment of selection */
  GradientEditorDragMode  control_drag_mode;    /* What is being dragged? */
  guint32                 control_click_time;   /* Time when mouse was pressed */
  gboolean                control_compress;     /* Compressing/expanding handles */
  gint                    control_last_x;       /* Last mouse position when dragging */
  gdouble                 control_last_gx;      /* Last position (wrt gradient) when dragging */
  gdouble                 control_orig_pos;     /* Original click position when dragging */

  /*  Split uniformly dialog  */
  gint          split_parts;

  /*  Replicate dialog  */
  gint          replicate_times;

  /*  Saved colors  */
  GimpRGB       saved_colors[GRAD_NUM_COLORS];

  /*  Color dialogs  */
  GimpGradientSegment *left_saved_segments;
  gboolean             left_saved_dirty;

  GimpGradientSegment *right_saved_segments;
  gboolean             right_saved_dirty;
};

struct _GimpGradientEditorClass
{
  GimpDataEditorClass  parent_class;
};


GType       gimp_gradient_editor_get_type (void) G_GNUC_CONST;

GtkWidget * gimp_gradient_editor_new      (Gimp               *gimp,
                                           GimpMenuFactory    *menu_factory);

void        gimp_gradient_editor_update   (GimpGradientEditor *editor);
void        gimp_gradient_editor_zoom     (GimpGradientEditor *editor,
                                           GimpZoomType        zoom_type);


#endif  /* __GIMP_GRADIENT_EDITOR_H__ */
