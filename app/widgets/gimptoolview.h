/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimptoolview.h
 * Copyright (C) 2001 Michael Natterer <mitch@gimp.org>
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

#ifndef __GIMP_TOOL_VIEW_H__
#define __GIMP_TOOL_VIEW_H__


#include "gimpcontainereditor.h"


#define GIMP_TYPE_TOOL_VIEW            (gimp_tool_view_get_type ())
#define GIMP_TOOL_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_TOOL_VIEW, GimpToolView))
#define GIMP_TOOL_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_TOOL_VIEW, GimpToolViewClass))
#define GIMP_IS_TOOL_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_TOOL_VIEW))
#define GIMP_IS_TOOL_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_TOOL_VIEW))
#define GIMP_TOOL_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_TOOL_VIEW, GimpToolViewClass))


typedef struct _GimpToolViewClass  GimpToolViewClass;

struct _GimpToolView
{
  GimpContainerEditor  parent_instance;

  GQuark               visible_handler_id;

  GtkWidget           *raise_button;
  GtkWidget           *lower_button;
  GtkWidget           *reset_button;
};

struct _GimpToolViewClass
{
  GimpContainerEditorClass  parent_class;
};


GType       gimp_tool_view_get_type (void) G_GNUC_CONST;

GtkWidget * gimp_tool_view_new      (GimpViewType     view_type,
                                     GimpContainer   *container,
                                     GimpContext     *context,
                                     gint             view_size,
                                     gint             view_border_width,
                                     GimpMenuFactory *menu_factory);


#endif  /*  __GIMP_TOOL_VIEW_H__  */
