/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimppatterns_pdb.c
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

#include "config.h"

#include <string.h>

#include "gimp.h"

/**
 * gimp_patterns_refresh:
 *
 * Refresh current patterns.
 *
 * This procedure retrieves all patterns currently in the user's
 * pattern path and updates the pattern dialogs accordingly.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_patterns_refresh (void)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_patterns_refresh",
				    &nreturn_vals,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_patterns_get_list:
 * @num_patterns: The number of patterns in the pattern list.
 *
 * Retrieve a complete listing of the available patterns.
 *
 * This procedure returns a complete listing of available GIMP
 * patterns. Each name returned can be used as input to the
 * 'gimp_patterns_set_pattern'.
 *
 * Returns: The list of pattern names.
 */
gchar **
gimp_patterns_get_list (gint *num_patterns)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gchar **pattern_list = NULL;
  gint i;

  return_vals = gimp_run_procedure ("gimp_patterns_get_list",
				    &nreturn_vals,
				    GIMP_PDB_END);

  *num_patterns = 0;

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      *num_patterns = return_vals[1].data.d_int32;
      pattern_list = g_new (gchar *, *num_patterns);
      for (i = 0; i < *num_patterns; i++)
	pattern_list[i] = g_strdup (return_vals[2].data.d_stringarray[i]);
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return pattern_list;
}

/**
 * gimp_patterns_get_pattern:
 * @width: The pattern width.
 * @height: The pattern height.
 *
 * Retrieve information about the currently active pattern.
 *
 * This procedure retrieves information about the currently active
 * pattern. This includes the pattern name, and the pattern extents
 * (width and height). All clone and bucket-fill operations with
 * patterns will use this pattern to control the application of paint
 * to the image.
 *
 * Returns: The pattern name.
 */
gchar *
gimp_patterns_get_pattern (gint *width,
			   gint *height)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gchar *name = NULL;

  return_vals = gimp_run_procedure ("gimp_patterns_get_pattern",
				    &nreturn_vals,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      name = g_strdup (return_vals[1].data.d_string);
      *width = return_vals[2].data.d_int32;
      *height = return_vals[3].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return name;
}

/**
 * gimp_patterns_set_pattern:
 * @name: The pattern name.
 *
 * Set the specified pattern as the active pattern.
 *
 * This procedure allows the active pattern mask to be set by
 * specifying its name. The name is simply a string which corresponds
 * to one of the names of the installed patterns. If there is no
 * matching pattern found, this procedure will return an error.
 * Otherwise, the specified pattern becomes active and will be used in
 * all subsequent paint operations.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_patterns_set_pattern (const gchar *name)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_patterns_set_pattern",
				    &nreturn_vals,
				    GIMP_PDB_STRING, name,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_patterns_get_pattern_data:
 * @name: the pattern name (\"\" means currently active pattern).
 * @width: The pattern width.
 * @height: The pattern height.
 * @mask_bpp: Pattern bytes per pixel.
 * @length: Length of pattern mask data.
 * @mask_data: The pattern mask data.
 *
 * Retrieve information about the currently active pattern (including
 * data).
 *
 * This procedure retrieves information about the currently active
 * pattern. This includes the pattern name, and the pattern extents
 * (width and height). It also returns the pattern data.
 *
 * Returns: The pattern name.
 */
gchar *
gimp_patterns_get_pattern_data (const gchar  *name,
				gint         *width,
				gint         *height,
				gint         *mask_bpp,
				gint         *length,
				guint8      **mask_data)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gchar *ret_name = NULL;

  return_vals = gimp_run_procedure ("gimp_patterns_get_pattern_data",
				    &nreturn_vals,
				    GIMP_PDB_STRING, name,
				    GIMP_PDB_END);

  *length = 0;

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      ret_name = g_strdup (return_vals[1].data.d_string);
      *width = return_vals[2].data.d_int32;
      *height = return_vals[3].data.d_int32;
      *mask_bpp = return_vals[4].data.d_int32;
      *length = return_vals[5].data.d_int32;
      *mask_data = g_new (guint8, *length);
      memcpy (*mask_data, return_vals[6].data.d_int8array,
	      *length * sizeof (guint8));
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return ret_name;
}
