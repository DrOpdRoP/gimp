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

#ifndef __GIMP_FREE_SELECT_TOOL_H__
#define __GIMP_FREE_SELECT_TOOL_H__


#include "gimpselectiontool.h"


#define GIMP_TYPE_FREE_SELECT_TOOL            (gimp_free_select_tool_get_type ())
#define GIMP_FREE_SELECT_TOOL(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_FREE_SELECT_TOOL, GimpFreeSelectTool))
#define GIMP_IS_FREE_SELECT_TOOL(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_FREE_SELECT_TOOL))
#define GIMP_FREE_SELECT_TOOL_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_FREE_SELECT_TOOL, GimpFreeSelectToolClass))
#define GIMP_IS_FREE_SELECT_TOOL_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_FREE_SELECT_TOOL))


typedef struct _GimpFreeSelectTool      GimpFreeSelectTool;
typedef struct _GimpFreeSelectToolClass GimpFreeSelectToolClass;

struct _GimpFreeSelectTool
{
  GimpSelectionTool  parent_instance;

  GdkPoint          *points;
  gint               num_points;
  gint               max_segs;
};

struct _GimpFreeSelectToolClass
{
  GimpSelectionToolClass parent_class;
};


void       gimp_free_select_tool_register (void);

GtkType    gimp_free_select_tool_get_type (void);


void       free_select                    (GimpImage        *gimage,
                                           gint              num_pts,
                                           ScanConvertPoint *pts,
                                           SelectOps         op,
                                           gboolean          antialias,
                                           gboolean          feather,
                                           gdouble           feather_radius);


#endif  /*  __GIMP_FREE_SELECT_TOOL_H__  */
