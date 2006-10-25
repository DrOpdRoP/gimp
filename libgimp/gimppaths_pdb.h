/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimppaths_pdb.h
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

#ifndef __GIMP_PATHS_PDB_H__
#define __GIMP_PATHS_PDB_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


#ifndef GIMP_DISABLE_DEPRECATED
gchar**  gimp_path_list              (gint32           image_ID,
                                      gint            *num_paths);
gchar*   gimp_path_get_current       (gint32           image_ID);
gboolean gimp_path_set_current       (gint32           image_ID,
                                      const gchar     *name);
gboolean gimp_path_delete            (gint32           image_ID,
                                      const gchar     *name);
gint     gimp_path_get_points        (gint32           image_ID,
                                      const gchar     *name,
                                      gint            *path_closed,
                                      gint            *num_path_point_details,
                                      gdouble        **points_pairs);
#endif /* GIMP_DISABLE_DEPRECATED */
gboolean gimp_path_set_points        (gint32           image_ID,
                                      const gchar     *name,
                                      gint             ptype,
                                      gint             num_path_points,
                                      const gdouble   *points_pairs);
#ifndef GIMP_DISABLE_DEPRECATED
gboolean gimp_path_stroke_current    (gint32           image_ID);
gint     gimp_path_get_point_at_dist (gint32           image_ID,
                                      gdouble          distance,
                                      gint            *y_point,
                                      gdouble         *slope);
gint     gimp_path_get_tattoo        (gint32           image_ID,
                                      const gchar     *name);
gboolean gimp_path_set_tattoo        (gint32           image_ID,
                                      const gchar     *name,
                                      gint             tattovalue);
gchar*   gimp_get_path_by_tattoo     (gint32           image_ID,
                                      gint             tattoo);
gboolean gimp_path_get_locked        (gint32           image_ID,
                                      const gchar     *name);
gboolean gimp_path_set_locked        (gint32           image_ID,
                                      const gchar     *name,
                                      gboolean         locked);
gboolean gimp_path_to_selection      (gint32           image_ID,
                                      const gchar     *name,
                                      GimpChannelOps   op,
                                      gboolean         antialias,
                                      gboolean         feather,
                                      gdouble          feather_radius_x,
                                      gdouble          feather_radius_y);
gboolean gimp_path_import            (gint32           image_ID,
                                      const gchar     *filename,
                                      gboolean         merge,
                                      gboolean         scale);
#endif /* GIMP_DISABLE_DEPRECATED */


G_END_DECLS

#endif /* __GIMP_PATHS_PDB_H__ */
