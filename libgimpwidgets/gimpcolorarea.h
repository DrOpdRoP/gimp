/* LIBGIMP - The GIMP Library 
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball                
 *
 * gimpcolorarea.h
 * Copyright (C) 2001 Sven Neumann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* This provides a color preview area. The preview
 * can handle transparency by showing the checkerboard and
 * handles drag'n'drop.
 */

#ifndef __GIMP_COLOR_AREA_H__
#define __GIMP_COLOR_AREA_H__


#include <gtk/gtkpreview.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GIMP_TYPE_COLOR_AREA            (gimp_color_area_get_type ())
#define GIMP_COLOR_AREA(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_COLOR_AREA, GimpColorArea))
#define GIMP_COLOR_AREA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_COLOR_AREA, GimpColorAreaClass))
#define GIMP_IS_COLOR_AREA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_COLOR_AREA))
#define GIMP_IS_COLOR_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_COLOR_AREA))
#define GIMP_COLOR_AREA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_COLOR_AREA, GimpColorAreaClass))


typedef struct _GimpColorAreaClass  GimpColorAreaClass;

struct _GimpColorArea
{
  GtkPreview         parent_instance;

  /*< private >*/
  GimpColorAreaType  type;
  GimpRGB            color;
  guint              idle_id;
};

struct _GimpColorAreaClass
{
  GtkPreviewClass  parent_class;

  void (* color_changed) (GimpColorArea *gca);
};


GType       gimp_color_area_get_type   (void);

GtkWidget * gimp_color_area_new        (const GimpRGB     *color,
					GimpColorAreaType  type,
					GdkModifierType    drag_mask);

void        gimp_color_area_set_color  (GimpColorArea     *gca,
					const GimpRGB     *color);
void        gimp_color_area_get_color  (GimpColorArea     *gca,
					GimpRGB           *color);
gboolean    gimp_color_area_has_alpha  (GimpColorArea     *gca);
void        gimp_color_area_set_type   (GimpColorArea     *gca,
					GimpColorAreaType  type);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GIMP_COLOR_AREA_H__ */
