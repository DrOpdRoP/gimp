/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimppalette_pdb.c
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

#include "gimp.h"

/**
 * gimp_palette_get_foreground:
 * @foreground: The foreground color.
 *
 * Get the current GIMP foreground color.
 *
 * This procedure retrieves the current GIMP foreground color. The
 * foreground color is used in a variety of tools such as paint tools,
 * blending, and bucket fill.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_palette_get_foreground (GimpRGB *foreground)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_palette_get_foreground",
				    &nreturn_vals,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  if (success)
    *foreground = return_vals[1].data.d_color;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_palette_get_background:
 * @background: The background color.
 *
 * Get the current GIMP background color.
 *
 * This procedure retrieves the current GIMP background color. The
 * background color is used in a variety of tools such as blending,
 * erasing (with non-alpha images), and image filling.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_palette_get_background (GimpRGB *background)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_palette_get_background",
				    &nreturn_vals,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  if (success)
    *background = return_vals[1].data.d_color;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_palette_set_foreground:
 * @foreground: The foreground color.
 *
 * Set the current GIMP foreground color.
 *
 * This procedure sets the current GIMP foreground color. After this is
 * set, operations which use foreground such as paint tools, blending,
 * and bucket fill will use the new value.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_palette_set_foreground (GimpRGB *foreground)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_palette_set_foreground",
				    &nreturn_vals,
				    GIMP_PDB_COLOR, foreground,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_palette_set_background:
 * @background: The background color.
 *
 * Set the current GIMP background color.
 *
 * This procedure sets the current GIMP background color. After this is
 * set, operations which use background such as blending, filling
 * images, clearing, and erasing (in non-alpha images) will use the new
 * value.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_palette_set_background (GimpRGB *background)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_palette_set_background",
				    &nreturn_vals,
				    GIMP_PDB_COLOR, background,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_palette_set_default_colors:
 *
 * Set the current GIMP foreground and background colors to black and
 * white.
 *
 * This procedure sets the current GIMP foreground and background
 * colors to their initial default values, black and white.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_palette_set_default_colors (void)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_palette_set_default_colors",
				    &nreturn_vals,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_palette_swap_colors:
 *
 * Swap the current GIMP foreground and background colors.
 *
 * This procedure swaps the current GIMP foreground and background
 * colors, so that the new foreground color becomes the old background
 * color and vice versa.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_palette_swap_colors (void)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp_palette_swap_colors",
				    &nreturn_vals,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}
