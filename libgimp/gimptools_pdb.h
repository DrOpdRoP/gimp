/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimptools_pdb.h
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

/* NOTE: This file is autogenerated by pdbgen.pl */

#ifndef __GIMP_TOOLS_PDB_H__
#define __GIMP_TOOLS_PDB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* For information look into the C source or the html documentation */


gboolean gimp_airbrush           (gint32                    drawable_ID,
				  gdouble                   pressure,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_airbrush_default   (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_blend              (gint32                    drawable_ID,
				  GimpBlendMode             blend_mode,
				  GimpLayerModeEffects      paint_mode,
				  GimpGradientType          gradient_type,
				  gdouble                   opacity,
				  gdouble                   offset,
				  GimpRepeatMode            repeat,
				  gboolean                  supersample,
				  gint                      max_depth,
				  gdouble                   threshold,
				  gdouble                   x1,
				  gdouble                   y1,
				  gdouble                   x2,
				  gdouble                   y2);
gboolean gimp_bucket_fill        (gint32                    drawable_ID,
				  GimpBucketFillMode        fill_mode,
				  GimpLayerModeEffects      paint_mode,
				  gdouble                   opacity,
				  gdouble                   threshold,
				  gboolean                  sample_merged,
				  gdouble                   x,
				  gdouble                   y);
gboolean gimp_clone              (gint32                    drawable_ID,
				  gint32                    src_drawable_ID,
				  GimpCloneType             clone_type,
				  gdouble                   src_x,
				  gdouble                   src_y,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_clone_default      (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_color_picker       (gint32                    image_ID,
				  gint32                    drawable_ID,
				  gdouble                   x,
				  gdouble                   y,
				  gboolean                  sample_merged,
				  gboolean                  sample_average,
				  gdouble                   average_radius,
				  gboolean                  save_color,
				  GimpRGB                  *color);
gboolean gimp_convolve           (gint32                    drawable_ID,
				  gdouble                   pressure,
				  GimpConvolveType          convolve_type,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_convolve_default   (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_dodgeburn          (gint32                    drawable_ID,
				  gdouble                   exposure,
				  GimpDodgeBurnType         dodgeburn_type,
				  GimpTransferMode          dodgeburn_mode,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_dodgeburn_default  (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_eraser             (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes,
				  GimpBrushApplicationMode  hardness,
				  GimpPaintApplicationMode  method);
gboolean gimp_eraser_default     (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_paintbrush         (gint32                    drawable_ID,
				  gdouble                   fade_out,
				  gint                      num_strokes,
				  gdouble                  *strokes,
				  GimpPaintApplicationMode  method,
				  gdouble                   gradient_length);
gboolean gimp_paintbrush_default (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_pencil             (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_smudge             (gint32                    drawable_ID,
				  gdouble                   pressure,
				  gint                      num_strokes,
				  gdouble                  *strokes);
gboolean gimp_smudge_default     (gint32                    drawable_ID,
				  gint                      num_strokes,
				  gdouble                  *strokes);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GIMP_TOOLS_PDB_H__ */
