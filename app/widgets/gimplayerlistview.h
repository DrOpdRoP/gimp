/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimplayerlistview.h
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

#ifndef __GIMP_LAYER_LIST_VIEW_H__
#define __GIMP_LAYER_LIST_VIEW_H__


#include "gimpdrawablelistview.h"


#define GIMP_TYPE_LAYER_LIST_VIEW            (gimp_layer_list_view_get_type ())
#define GIMP_LAYER_LIST_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_LAYER_LIST_VIEW, GimpLayerListView))
#define GIMP_LAYER_LIST_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_LAYER_LIST_VIEW, GimpLayerListViewClass))
#define GIMP_IS_LAYER_LIST_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_LAYER_LIST_VIEW))
#define GIMP_IS_LAYER_LIST_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_LAYER_LIST_VIEW))
#define GIMP_LAYER_LIST_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_LAYER_LIST_VIEW, GimpLayerListViewClass))


typedef struct _GimpLayerListViewClass  GimpLayerListViewClass;

struct _GimpLayerListView
{
  GimpDrawableListView  parent_instance;

  GtkWidget            *options_box;

  GtkWidget            *paint_mode_menu;
  GtkWidget            *preserve_trans_toggle;
  GtkAdjustment        *opacity_adjustment;

  GtkWidget            *anchor_button;

  /*< private >*/
  GQuark                mode_changed_handler_id;
  GQuark                opacity_changed_handler_id;
  GQuark                preserve_trans_changed_handler_id;
};

struct _GimpLayerListViewClass
{
  GimpDrawableListViewClass  parent_class;
};


GType   gimp_layer_list_view_get_type (void);


#endif  /*  __GIMP_LAYER_LIST_VIEW_H__  */
