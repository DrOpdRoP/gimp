/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * brush_generated module Copyright 1998 Jay Cox <jaycox@earthlink.net>
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

#ifndef __GIMP_BRUSH_GENERATED_H__
#define __GIMP_BRUSH_GENERATED_H__


#include "gimpbrush.h"


#define GIMP_BRUSH_GENERATED_FILE_EXTENSION ".vbr"


#define GIMP_TYPE_BRUSH_GENERATED            (gimp_brush_generated_get_type ())
#define GIMP_BRUSH_GENERATED(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_BRUSH_GENERATED, GimpBrushGenerated))
#define GIMP_BRUSH_GENERATED_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_BRUSH_GENERATED, GimpBrushGeneratedClass))
#define GIMP_IS_BRUSH_GENERATED(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_BRUSH_GENERATED))
#define GIMP_IS_BRUSH_GENERATED_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_BRUSH_GENERATED))


typedef struct _GimpBrushGeneratedClass GimpBrushGeneratedClass;

struct _GimpBrushGenerated
{
  GimpBrush  parent_instance;

  gfloat     radius;
  gfloat     hardness;     /* 0.0 - 1.0  */
  gfloat     angle;        /* in degrees */
  gfloat     aspect_ratio; /* y/x        */

  /* private */
  gint      freeze;
};

struct _GimpBrushGeneratedClass
{
  GimpBrushClass  parent_class;
};


GType       gimp_brush_generated_get_type     (void);

GimpData  * gimp_brush_generated_new          (gfloat       radius,
					       gfloat       hardness,
					       gfloat       angle,
					       gfloat       aspect_ratio);
GimpData  * gimp_brush_generated_load         (const gchar *file_name);

void    gimp_brush_generated_freeze           (GimpBrushGenerated *brush);
void    gimp_brush_generated_thaw             (GimpBrushGenerated *brush);

gfloat  gimp_brush_generated_set_radius       (GimpBrushGenerated *brush,
					       gfloat              radius);
gfloat  gimp_brush_generated_set_hardness     (GimpBrushGenerated *brush,
					       gfloat              hardness);
gfloat  gimp_brush_generated_set_angle        (GimpBrushGenerated *brush,
					       gfloat              angle);
gfloat  gimp_brush_generated_set_aspect_ratio (GimpBrushGenerated *brush,
					       gfloat              ratio);

gfloat  gimp_brush_generated_get_radius       (const GimpBrushGenerated *brush);
gfloat  gimp_brush_generated_get_hardness     (const GimpBrushGenerated *brush);
gfloat  gimp_brush_generated_get_angle        (const GimpBrushGenerated *brush);
gfloat  gimp_brush_generated_get_aspect_ratio (const GimpBrushGenerated *brush);


#endif  /*  __GIMP_BRUSH_GENERATED_H__  */
