/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"

#include <string.h>

#include <glib-object.h>

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/gimp.h"
#include "core/gimpchannel-select.h"
#include "core/gimpimage.h"
#include "core/gimplist.h"
#include "core/gimpstrokedesc.h"
#include "gimp-intl.h"
#include "vectors/gimpanchor.h"
#include "vectors/gimpbezierstroke.h"
#include "vectors/gimpvectors-compat.h"
#include "vectors/gimpvectors-import.h"
#include "vectors/gimpvectors.h"

static ProcRecord path_list_proc;
static ProcRecord path_get_current_proc;
static ProcRecord path_set_current_proc;
static ProcRecord path_delete_proc;
static ProcRecord path_get_points_proc;
static ProcRecord path_set_points_proc;
static ProcRecord path_stroke_current_proc;
static ProcRecord path_get_point_at_dist_proc;
static ProcRecord path_get_tattoo_proc;
static ProcRecord path_set_tattoo_proc;
static ProcRecord get_path_by_tattoo_proc;
static ProcRecord path_get_locked_proc;
static ProcRecord path_set_locked_proc;
static ProcRecord path_to_selection_proc;
static ProcRecord path_import_proc;
static ProcRecord path_import_string_proc;

void
register_paths_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &path_list_proc);
  procedural_db_register (gimp, &path_get_current_proc);
  procedural_db_register (gimp, &path_set_current_proc);
  procedural_db_register (gimp, &path_delete_proc);
  procedural_db_register (gimp, &path_get_points_proc);
  procedural_db_register (gimp, &path_set_points_proc);
  procedural_db_register (gimp, &path_stroke_current_proc);
  procedural_db_register (gimp, &path_get_point_at_dist_proc);
  procedural_db_register (gimp, &path_get_tattoo_proc);
  procedural_db_register (gimp, &path_set_tattoo_proc);
  procedural_db_register (gimp, &get_path_by_tattoo_proc);
  procedural_db_register (gimp, &path_get_locked_proc);
  procedural_db_register (gimp, &path_set_locked_proc);
  procedural_db_register (gimp, &path_to_selection_proc);
  procedural_db_register (gimp, &path_import_proc);
  procedural_db_register (gimp, &path_import_string_proc);
}

static Argument *
path_list_invoker (ProcRecord   *proc_record,
                   Gimp         *gimp,
                   GimpContext  *context,
                   GimpProgress *progress,
                   Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gint32 num_paths = 0;
  gchar **path_list = NULL;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      path_list = gimp_container_get_name_array (image->vectors, &num_paths);
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    {
      return_args[1].value.pdb_int = num_paths;
      return_args[2].value.pdb_pointer = path_list;
    }

  return return_args;
}

static ProcArg path_list_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to list the paths from"
  }
};

static ProcArg path_list_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "num-paths",
    "The number of paths returned."
  },
  {
    GIMP_PDB_STRINGARRAY,
    "path-list",
    "List of the paths belonging to this image."
  }
};

static ProcRecord path_list_proc =
{
  "gimp-path-list",
  "gimp-path-list",
  "This procedure is deprecated! Use 'gimp-image-get-vectors' instead.",
  "This procedure is deprecated! Use 'gimp-image-get-vectors' instead.",
  "",
  "",
  "",
  "gimp-image-get-vectors",
  GIMP_INTERNAL,
  1,
  path_list_inargs,
  2,
  path_list_outargs,
  { { path_list_invoker } }
};

static Argument *
path_get_current_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gchar *name = NULL;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_active_vectors (image);

      if (vectors)
        name = g_strdup (gimp_object_get_name (GIMP_OBJECT (vectors)));
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    return_args[1].value.pdb_pointer = name;

  return return_args;
}

static ProcArg path_get_current_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to get the current path from"
  }
};

static ProcArg path_get_current_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the current path."
  }
};

static ProcRecord path_get_current_proc =
{
  "gimp-path-get-current",
  "gimp-path-get-current",
  "This procedure is deprecated! Use 'gimp-image-get-active-vectors' instead.",
  "This procedure is deprecated! Use 'gimp-image-get-active-vectors' instead.",
  "",
  "",
  "",
  "gimp-image-get-active-vectors",
  GIMP_INTERNAL,
  1,
  path_get_current_inargs,
  1,
  path_get_current_outargs,
  { { path_get_current_invoker } }
};

static Argument *
path_set_current_invoker (ProcRecord   *proc_record,
                          Gimp         *gimp,
                          GimpContext  *context,
                          GimpProgress *progress,
                          Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *name;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        gimp_image_set_active_vectors (image, vectors);
      else
        success = FALSE;
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_set_current_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image in which a path will become current"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path to make current."
  }
};

static ProcRecord path_set_current_proc =
{
  "gimp-path-set-current",
  "gimp-path-set-current",
  "This procedure is deprecated! Use 'gimp-image-set-active-vectors' instead.",
  "This procedure is deprecated! Use 'gimp-image-set-active-vectors' instead.",
  "",
  "",
  "",
  "gimp-image-set-active-vectors",
  GIMP_INTERNAL,
  2,
  path_set_current_inargs,
  0,
  NULL,
  { { path_set_current_invoker } }
};

static Argument *
path_delete_invoker (ProcRecord   *proc_record,
                     Gimp         *gimp,
                     GimpContext  *context,
                     GimpProgress *progress,
                     Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *name;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        gimp_image_remove_vectors (image, vectors);
      else
        success = FALSE;
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_delete_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to delete the path from"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path to delete."
  }
};

static ProcRecord path_delete_proc =
{
  "gimp-path-delete",
  "gimp-path-delete",
  "This procedure is deprecated! Use 'gimp-image-remove-vectors' instead.",
  "This procedure is deprecated! Use 'gimp-image-remove-vectors' instead.",
  "",
  "",
  "",
  "gimp-image-remove-vectors",
  GIMP_INTERNAL,
  2,
  path_delete_inargs,
  0,
  NULL,
  { { path_delete_invoker } }
};

static Argument *
path_get_points_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gchar *name;
  gint32 path_type = 0;
  gint32 path_closed = 0;
  gint32 num_path_point_details = 0;
  gdouble *points_pairs = NULL;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        {
          GimpVectorsCompatPoint *points;
          gint num_points;

          path_type = 1; /* BEZIER (1.2 compat) */

          points = gimp_vectors_compat_get_points (vectors, &num_points,
                                                   &path_closed);

          num_path_point_details = num_points * 3;

          if (points)
            {
              gdouble *curr_point;
              gint     i;

              points_pairs = g_new0 (gdouble, num_path_point_details);

              for (i = 0, curr_point = points_pairs;
                   i < num_points;
                   i++, curr_point += 3)
                {
                  curr_point[0] = points[i].x;
                  curr_point[1] = points[i].y;
                  curr_point[2] = points[i].type;
                }

              g_free (points);
            }
          else
            success = FALSE;
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    {
      return_args[1].value.pdb_int = path_type;
      return_args[2].value.pdb_int = path_closed;
      return_args[3].value.pdb_int = num_path_point_details;
      return_args[4].value.pdb_pointer = points_pairs;
    }

  return return_args;
}

static ProcArg path_get_points_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to list the paths from"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path whose points should be listed."
  }
};

static ProcArg path_get_points_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "path-type",
    "The type of the path. Currently only one type (1 = Bezier) is supported"
  },
  {
    GIMP_PDB_INT32,
    "path-closed",
    "Return if the path is closed. (0 = path open, 1 = path closed)"
  },
  {
    GIMP_PDB_INT32,
    "num-path-point-details",
    "The number of points returned. Each point is made up of (x, y, pnt_type) of floats."
  },
  {
    GIMP_PDB_FLOATARRAY,
    "points-pairs",
    "The points in the path represented as 3 floats. The first is the x pos, next is the y pos, last is the type of the pnt. The type field is dependant on the path type. For beziers (type 1 paths) the type can either be (1.0 = BEZIER_ANCHOR, 2.0 = BEZIER_CONTROL, 3.0 = BEZIER_MOVE). Note all points are returned in pixel resolution."
  }
};

static ProcRecord path_get_points_proc =
{
  "gimp-path-get-points",
  "gimp-path-get-points",
  "List the points associated with the named path.",
  "List the points associated with the named path.",
  "Andy Thomas",
  "Andy Thomas",
  "1999",
  NULL,
  GIMP_INTERNAL,
  2,
  path_get_points_inargs,
  4,
  path_get_points_outargs,
  { { path_get_points_invoker } }
};

static Argument *
path_set_points_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *name;
  gint32 ptype;
  gint32 num_path_points;
  gdouble *points_pairs;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  ptype = args[2].value.pdb_int;

  num_path_points = args[3].value.pdb_int;
  if (num_path_points <= 0)
    success = FALSE;

  points_pairs = (gdouble *) args[4].value.pdb_pointer;

  if (success)
    {
      gboolean closed = FALSE;

      if ((num_path_points / 3) % 3 == 0)
        closed = TRUE;
      else if ((num_path_points / 3) % 3 != 2)
        success = FALSE;

      if (success)
        {
          GimpVectors            *vectors;
          gdouble                *curr_point_pair;
          GimpVectorsCompatPoint *points;
          gint                    n_points;
          gint                    i;

          n_points = num_path_points / 3;

          points = g_new0 (GimpVectorsCompatPoint, n_points);

          for (i = 0, curr_point_pair = points_pairs;
               i < n_points;
               i++, curr_point_pair += 3)
            {
              points[i].x    = curr_point_pair[0];
              points[i].y    = curr_point_pair[1];
              points[i].type = curr_point_pair[2];
            }

          vectors = gimp_vectors_compat_new (image, name, points, n_points,
                                             closed);

          g_free (points);

          if (vectors)
            gimp_image_add_vectors (image, vectors, 0);
          else
            success = FALSE;
        }
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_set_points_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image to set the paths in"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path to create. If it exists then a unique name will be created - query the list of paths if you want to make sure that the name of the path you create is unique. This will be set as the current path."
  },
  {
    GIMP_PDB_INT32,
    "ptype",
    "The type of the path. Currently only one type (1 = Bezier) is supported."
  },
  {
    GIMP_PDB_INT32,
    "num-path-points",
    "The number of elements in the array, i.e. the number of points in the path * 3. Each point is made up of (x, y, type) of floats. Currently only the creation of bezier curves is allowed. The type parameter must be set to (1) to indicate a BEZIER type curve. Note that for BEZIER curves, points must be given in the following order: ACCACCAC... If the path is not closed the last control point is missed off. Points consist of three control points (control/anchor/control) so for a curve that is not closed there must be at least two points passed (2 x,y pairs). If (num_path_points/3) % 3 = 0 then the path is assumed to be closed and the points are ACCACCACCACC."
  },
  {
    GIMP_PDB_FLOATARRAY,
    "points-pairs",
    "The points in the path represented as 3 floats. The first is the x pos, next is the y pos, last is the type of the pnt. The type field is dependant on the path type. For beziers (type 1 paths) the type can either be (1.0 = BEZIER_ANCHOR, 2.0 = BEZIER_CONTROL, 3.0= BEZIER_MOVE). Note all points are returned in pixel resolution."
  }
};

static ProcRecord path_set_points_proc =
{
  "gimp-path-set-points",
  "gimp-path-set-points",
  "Set the points associated with the named path.",
  "Set the points associated with the named path.",
  "Andy Thomas",
  "Andy Thomas",
  "1999",
  NULL,
  GIMP_INTERNAL,
  5,
  path_set_points_inargs,
  0,
  NULL,
  { { path_set_points_invoker } }
};

static Argument *
path_stroke_current_invoker (ProcRecord   *proc_record,
                             Gimp         *gimp,
                             GimpContext  *context,
                             GimpProgress *progress,
                             Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      GimpVectors  *vectors  = gimp_image_get_active_vectors (image);
      GimpDrawable *drawable = gimp_image_active_drawable (image);

      if (vectors && drawable)
        {
          GimpStrokeDesc *desc = gimp_stroke_desc_new (gimp, context);

          g_object_set (desc, "method", GIMP_STROKE_METHOD_PAINT_CORE, NULL);

          success = gimp_item_stroke (GIMP_ITEM (vectors),
                                      drawable, context, desc, TRUE);

          g_object_unref (desc);
        }
      else
        success = FALSE;
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_stroke_current_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image which contains the path to stroke"
  }
};

static ProcRecord path_stroke_current_proc =
{
  "gimp-path-stroke-current",
  "gimp-path-stroke-current",
  "Stroke the current path in the passed image.",
  "Stroke the current path in the passed image.",
  "Andy Thomas",
  "Andy Thomas",
  "1999",
  NULL,
  GIMP_INTERNAL,
  1,
  path_stroke_current_inargs,
  0,
  NULL,
  { { path_stroke_current_invoker } }
};

static Argument *
path_get_point_at_dist_invoker (ProcRecord   *proc_record,
                                Gimp         *gimp,
                                GimpContext  *context,
                                GimpProgress *progress,
                                Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gdouble distance;
  gint32 x_point = 0;
  gint32 y_point = 0;
  gdouble slope = 0.0;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  distance = args[1].value.pdb_float;

  if (success)
    {
      GimpVectors *vectors;
      GimpStroke  *stroke;
      gdouble      distance_along;
      gdouble      stroke_length;
      gdouble      stroke_distance;
      GimpCoords   position;

      vectors = gimp_image_get_active_vectors (image);

      if (vectors)
        {
          distance_along = 0.0;
          stroke = gimp_vectors_stroke_get_next (vectors, NULL);

          while (stroke != NULL )
            {
              stroke_length = gimp_stroke_get_length (stroke, 0.5);

              if (distance_along + stroke_length < distance)
                {
                  distance_along += stroke_length;
                }
              else
                {
                  stroke_distance = distance - distance_along;
                  stroke_distance = stroke_distance < 0 ? 0: stroke_distance;

                  if (!gimp_stroke_get_point_at_dist (stroke, stroke_distance, 0.5,
                                                      &position, &slope))
                    {
                      success = FALSE;
                      break;
                    }
                  else
                    {
                      success = TRUE;
                      x_point = ROUND (position.x);
                      y_point = ROUND (position.y);
                      break;
                    }
                }

              stroke = gimp_vectors_stroke_get_next (vectors, stroke);
            }
        }
      else
        {
          success = FALSE;
        }
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    {
      return_args[1].value.pdb_int = x_point;
      return_args[2].value.pdb_int = y_point;
      return_args[3].value.pdb_float = slope;
    }

  return return_args;
}

static ProcArg path_get_point_at_dist_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image the paths belongs to"
  },
  {
    GIMP_PDB_FLOAT,
    "distance",
    "The distance along the path."
  }
};

static ProcArg path_get_point_at_dist_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "x-point",
    "The x position of the point."
  },
  {
    GIMP_PDB_INT32,
    "y-point",
    "The y position of the point."
  },
  {
    GIMP_PDB_FLOAT,
    "slope",
    "The slope (dy / dx) at the specified point."
  }
};

static ProcRecord path_get_point_at_dist_proc =
{
  "gimp-path-get-point-at-dist",
  "gimp-path-get-point-at-dist",
  "This procedure is deprecated! Use 'gimp-vectors-stroke-get-point-at-dist' instead.",
  "This will return the x,y position of a point at a given distance along the bezier curve. The distance will be obtained by first digitizing the curve internally and then walking along the curve. For a closed curve the start of the path is the first point on the path that was created. This might not be obvious. Note the current path is used.",
  "Andy Thomas",
  "Andy Thomas",
  "1999",
  "gimp-vectors-stroke-get-point-at-dist",
  GIMP_INTERNAL,
  2,
  path_get_point_at_dist_inargs,
  3,
  path_get_point_at_dist_outargs,
  { { path_get_point_at_dist_invoker } }
};

static Argument *
path_get_tattoo_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gchar *name;
  gint32 tattoo = 0;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        tattoo = gimp_item_get_tattoo (GIMP_ITEM (vectors));
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    return_args[1].value.pdb_int = tattoo;

  return return_args;
}

static ProcArg path_get_tattoo_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path whose tattoo should be obtained."
  }
};

static ProcArg path_get_tattoo_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "tattoo",
    "The tattoo associated with the named path."
  }
};

static ProcRecord path_get_tattoo_proc =
{
  "gimp-path-get-tattoo",
  "gimp-path-get-tattoo",
  "This procedure is deprecated! Use 'gimp-vectors-get-tattoo' instead.",
  "This procedure is deprecated! Use 'gimp-vectors-get-tattoo' instead.",
  "",
  "",
  "",
  "gimp-vectors-get-tattoo",
  GIMP_INTERNAL,
  2,
  path_get_tattoo_inargs,
  1,
  path_get_tattoo_outargs,
  { { path_get_tattoo_invoker } }
};

static Argument *
path_set_tattoo_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *name;
  gint32 tattovalue;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  tattovalue = args[2].value.pdb_int;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        gimp_item_set_tattoo (GIMP_ITEM (vectors), tattovalue);
      else
        success = FALSE;
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_set_tattoo_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "the name of the path whose tattoo should be set"
  },
  {
    GIMP_PDB_INT32,
    "tattovalue",
    "The tattoo associated with the name path. Only values returned from 'path_get_tattoo' should be used here"
  }
};

static ProcRecord path_set_tattoo_proc =
{
  "gimp-path-set-tattoo",
  "gimp-path-set-tattoo",
  "This procedure is deprecated! Use 'gimp-vectors-set-tattoo' instead.",
  "This procedure is deprecated! Use 'gimp-vectors-set-tattoo' instead.",
  "",
  "",
  "",
  "gimp-vectors-set-tattoo",
  GIMP_INTERNAL,
  3,
  path_set_tattoo_inargs,
  0,
  NULL,
  { { path_set_tattoo_invoker } }
};

static Argument *
get_path_by_tattoo_invoker (ProcRecord   *proc_record,
                            Gimp         *gimp,
                            GimpContext  *context,
                            GimpProgress *progress,
                            Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gint32 tattoo;
  gchar *name = NULL;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  tattoo = args[1].value.pdb_int;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_tattoo (image, tattoo);

      if (vectors)
        name = g_strdup (gimp_object_get_name (GIMP_OBJECT (vectors)));
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    return_args[1].value.pdb_pointer = name;

  return return_args;
}

static ProcArg get_path_by_tattoo_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_INT32,
    "tattoo",
    "The tattoo of the required path."
  }
};

static ProcArg get_path_by_tattoo_outargs[] =
{
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path with the specified tattoo."
  }
};

static ProcRecord get_path_by_tattoo_proc =
{
  "gimp-get-path-by-tattoo",
  "gimp-get-path-by-tattoo",
  "This procedure is deprecated! Use 'gimp-image-get-vectors-by-tattoo' instead.",
  "This procedure is deprecated! Use 'gimp-image-get-vectors-by-tattoo' instead.",
  "",
  "",
  "",
  "gimp-image-get-vectors-by-tattoo",
  GIMP_INTERNAL,
  2,
  get_path_by_tattoo_inargs,
  1,
  get_path_by_tattoo_outargs,
  { { get_path_by_tattoo_invoker } }
};

static Argument *
path_get_locked_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  gchar *name;
  gboolean locked = FALSE;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        locked = gimp_item_get_linked (GIMP_ITEM (vectors));
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    return_args[1].value.pdb_int = locked;

  return return_args;
}

static ProcArg path_get_locked_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path whose locked status should be obtained."
  }
};

static ProcArg path_get_locked_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "locked",
    "TRUE if the path is locked, FALSE otherwise"
  }
};

static ProcRecord path_get_locked_proc =
{
  "gimp-path-get-locked",
  "gimp-path-get-locked",
  "This procedure is deprecated! Use 'gimp-vectors-get-linked' instead.",
  "This procedure is deprecated! Use 'gimp-vectors-get-linked' instead.",
  "",
  "",
  "",
  "gimp-vectors-get-linked",
  GIMP_INTERNAL,
  2,
  path_get_locked_inargs,
  1,
  path_get_locked_outargs,
  { { path_get_locked_invoker } }
};

static Argument *
path_set_locked_invoker (ProcRecord   *proc_record,
                         Gimp         *gimp,
                         GimpContext  *context,
                         GimpProgress *progress,
                         Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *name;
  gboolean locked;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  locked = args[2].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        gimp_item_set_linked (GIMP_ITEM (vectors), locked, TRUE);
      else
        success = FALSE;
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_set_locked_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "the name of the path whose locked status should be set"
  },
  {
    GIMP_PDB_INT32,
    "locked",
    "Whether the path is locked"
  }
};

static ProcRecord path_set_locked_proc =
{
  "gimp-path-set-locked",
  "gimp-path-set-locked",
  "This procedure is deprecated! Use 'gimp-vectors-set-linked' instead.",
  "This procedure is deprecated! Use 'gimp-vectors-set-linked' instead.",
  "",
  "",
  "",
  "gimp-vectors-set-linked",
  GIMP_INTERNAL,
  3,
  path_set_locked_inargs,
  0,
  NULL,
  { { path_set_locked_invoker } }
};

static Argument *
path_to_selection_invoker (ProcRecord   *proc_record,
                           Gimp         *gimp,
                           GimpContext  *context,
                           GimpProgress *progress,
                           Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *name;
  gint32 op;
  gboolean antialias;
  gboolean feather;
  gdouble feather_radius_x;
  gdouble feather_radius_y;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL || !g_utf8_validate (name, -1, NULL))
    success = FALSE;

  op = args[2].value.pdb_int;
  if (op < GIMP_CHANNEL_OP_ADD || op > GIMP_CHANNEL_OP_INTERSECT)
    success = FALSE;

  antialias = args[3].value.pdb_int ? TRUE : FALSE;

  feather = args[4].value.pdb_int ? TRUE : FALSE;

  feather_radius_x = args[5].value.pdb_float;

  feather_radius_y = args[6].value.pdb_float;

  if (success)
    {
      GimpVectors *vectors = gimp_image_get_vectors_by_name (image, name);

      if (vectors)
        gimp_channel_select_vectors (gimp_image_get_mask (image),
                                     _("Path to Selection"),
                                     vectors,
                                     op,
                                     antialias,
                                     feather,
                                     feather_radius_x,
                                     feather_radius_y);
      else
        success = FALSE;
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_to_selection_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "name",
    "The name of the path which should be made into selection."
  },
  {
    GIMP_PDB_INT32,
    "op",
    "The desired operation with current selection: { GIMP_CHANNEL_OP_ADD (0), GIMP_CHANNEL_OP_SUBTRACT (1), GIMP_CHANNEL_OP_REPLACE (2), GIMP_CHANNEL_OP_INTERSECT (3) }"
  },
  {
    GIMP_PDB_INT32,
    "antialias",
    "Antialias selection."
  },
  {
    GIMP_PDB_INT32,
    "feather",
    "Feather selection."
  },
  {
    GIMP_PDB_FLOAT,
    "feather-radius-x",
    "Feather radius x."
  },
  {
    GIMP_PDB_FLOAT,
    "feather-radius-y",
    "Feather radius y."
  }
};

static ProcRecord path_to_selection_proc =
{
  "gimp-path-to-selection",
  "gimp-path-to-selection",
  "Transforms the active path into a selection",
  "This procedure renders the desired path into the current selection.",
  "Jo\xc3\xa3o S. O. Bueno Calligaris",
  "Jo\xc3\xa3o S. O. Bueno Calligaris",
  "2003",
  NULL,
  GIMP_INTERNAL,
  7,
  path_to_selection_inargs,
  0,
  NULL,
  { { path_to_selection_invoker } }
};

static Argument *
path_import_invoker (ProcRecord   *proc_record,
                     Gimp         *gimp,
                     GimpContext  *context,
                     GimpProgress *progress,
                     Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *filename;
  gboolean merge;
  gboolean scale;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  filename = (gchar *) args[1].value.pdb_pointer;
  if (filename == NULL)
    success = FALSE;

  merge = args[2].value.pdb_int ? TRUE : FALSE;

  scale = args[3].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_vectors_import_file (image, filename, merge, scale, -1, NULL);
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_import_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "filename",
    "The name of the SVG file to import."
  },
  {
    GIMP_PDB_INT32,
    "merge",
    "Merge paths into a single vectors object."
  },
  {
    GIMP_PDB_INT32,
    "scale",
    "Scale the SVG to image dimensions."
  }
};

static ProcRecord path_import_proc =
{
  "gimp-path-import",
  "gimp-path-import",
  "Import paths from an SVG file.",
  "This procedure imports paths from an SVG file. SVG elements other than paths and basic shapes are ignored.",
  "Sven Neumann <sven@gimp.org>",
  "Sven Neumann",
  "2003",
  NULL,
  GIMP_INTERNAL,
  4,
  path_import_inargs,
  0,
  NULL,
  { { path_import_invoker } }
};

static Argument *
path_import_string_invoker (ProcRecord   *proc_record,
                            Gimp         *gimp,
                            GimpContext  *context,
                            GimpProgress *progress,
                            Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *image;
  gchar *string;
  gint32 length;
  gboolean merge;
  gboolean scale;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  string = (gchar *) args[1].value.pdb_pointer;
  if (string == NULL)
    success = FALSE;

  length = args[2].value.pdb_int;

  merge = args[3].value.pdb_int ? TRUE : FALSE;

  scale = args[4].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_vectors_import_buffer (image, string, length,
                                            merge, scale, -1, NULL);
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg path_import_string_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_STRING,
    "string",
    "A string that must be a complete and valid SVG document."
  },
  {
    GIMP_PDB_INT32,
    "length",
    "Number of bytes in string or -1 if the string is NULL terminated."
  },
  {
    GIMP_PDB_INT32,
    "merge",
    "Merge paths into a single vectors object."
  },
  {
    GIMP_PDB_INT32,
    "scale",
    "Scale the SVG to image dimensions."
  }
};

static ProcRecord path_import_string_proc =
{
  "gimp-path-import-string",
  "gimp-path-import-string",
  "Import paths from an SVG string.",
  "This procedure works like gimp_path_import() but takes a string rather than reading the SVG from a file. This allows you to write scripts that generate SVG and feed it to GIMP.",
  "Sven Neumann <sven@gimp.org>",
  "Sven Neumann",
  "2005",
  NULL,
  GIMP_INTERNAL,
  5,
  path_import_string_inargs,
  0,
  NULL,
  { { path_import_string_invoker } }
};
