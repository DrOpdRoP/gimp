/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2000 Spencer Kimball and Peter Mattis
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


#include <gtk/gtk.h>

#include "libgimpbase/gimpbasetypes.h"

#include "core/core-types.h"
#include "procedural_db.h"

#include "core/gimp.h"
#include "core/gimpcontainer.h"
#include "core/gimpdatafactory.h"
#include "core/gimpimage-convert.h"
#include "core/gimpimage.h"
#include "core/gimppalette.h"

static ProcRecord convert_rgb_proc;
static ProcRecord convert_grayscale_proc;
static ProcRecord convert_indexed_proc;

void
register_convert_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &convert_rgb_proc);
  procedural_db_register (gimp, &convert_grayscale_proc);
  procedural_db_register (gimp, &convert_indexed_proc);
}

static Argument *
convert_rgb_invoker (Gimp     *gimp,
                     Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;

  gimage = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  if (success)
    if ((success = (gimp_image_base_type (gimage) != RGB)))
      gimp_image_convert ((void *) gimage, RGB, 0, 0, 0, 1, 0, NULL);

  return procedural_db_return_args (&convert_rgb_proc, success);
}

static ProcArg convert_rgb_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  }
};

static ProcRecord convert_rgb_proc =
{
  "gimp_convert_rgb",
  "Convert specified image to RGB color",
  "This procedure converts the specified image to RGB color. This process requires an image of type GRAY or INDEXED. No image content is lost in this process aside from the colormap for an indexed image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  1,
  convert_rgb_inargs,
  0,
  NULL,
  { { convert_rgb_invoker } }
};

static Argument *
convert_grayscale_invoker (Gimp     *gimp,
                           Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;

  gimage = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  if (success)
    if ((success = (gimp_image_base_type (gimage) != GRAY)))
      gimp_image_convert ((void *) gimage, GRAY, 0, 0, 0, 1, 0, NULL);

  return procedural_db_return_args (&convert_grayscale_proc, success);
}

static ProcArg convert_grayscale_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  }
};

static ProcRecord convert_grayscale_proc =
{
  "gimp_convert_grayscale",
  "Convert specified image to grayscale (256 intensity levels)",
  "This procedure converts the specified image to grayscale with 8 bits per pixel (256 intensity levels). This process requires an image of type RGB or INDEXED.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  1,
  convert_grayscale_inargs,
  0,
  NULL,
  { { convert_grayscale_invoker } }
};

static Argument *
convert_indexed_invoker (Gimp     *gimp,
                         Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;
  gint32 dither_type;
  gint32 palette_type;
  gint32 num_cols;
  gboolean alpha_dither;
  gboolean remove_unused;
  gchar *palette_name;

  gimage = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  dither_type = args[1].value.pdb_int;
  if (dither_type < NO_DITHER || dither_type > NODESTRUCT_DITHER)
    success = FALSE;

  palette_type = args[2].value.pdb_int;

  num_cols = args[3].value.pdb_int;

  alpha_dither = args[4].value.pdb_int ? TRUE : FALSE;

  remove_unused = args[5].value.pdb_int ? TRUE : FALSE;

  palette_name = (gchar *) args[6].value.pdb_pointer;
  if (palette_name == NULL)
    success = FALSE;

  if (success)
    {
      GimpPalette *palette = NULL;
    
      if ((success = (gimp_image_base_type (gimage) != INDEXED)))
	{
	  switch (dither_type)
	    {
	    case NO_DITHER:
	    case FS_DITHER:
	    case FSLOWBLEED_DITHER:
	    case FIXED_DITHER:
	      break;
	    default:
	      success = FALSE;
	      break;
	    }
	  
	  switch (palette_type)
	    {
	    case MAKE_PALETTE:
	      if (num_cols < 1 || num_cols > MAXNUMCOLORS)
		success = FALSE;
	      break;
    
	    case REUSE_PALETTE:
	    case WEB_PALETTE:
	    case MONO_PALETTE:
	      break;
    
	    case CUSTOM_PALETTE:
	      if (! gimp->palette_factory->container->num_children)
		gimp_data_factory_data_init (gimp->palette_factory, FALSE);
    
	      palette = (GimpPalette *)
		gimp_container_get_child_by_name (gimp->palette_factory->container,
						  palette_name);
    
	      if (palette == NULL)
		success = FALSE;
    
	      break;
    
	    default:
	      success = FALSE;
	    }
	}
    
      if (success)
	gimp_image_convert ((void *) gimage, INDEXED, num_cols, dither_type,
			    alpha_dither, remove_unused, palette_type, palette);
    }

  return procedural_db_return_args (&convert_indexed_proc, success);
}

static ProcArg convert_indexed_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  },
  {
    GIMP_PDB_INT32,
    "dither_type",
    "dither type (0=none, 1=fs, 2=fs/low-bleed 3=fixed)"
  },
  {
    GIMP_PDB_INT32,
    "palette_type",
    "The type of palette to use: { MAKE_PALETTE (0), REUSE_PALETTE (1), WEB_PALETTE (2), MONO_PALETTE (3), CUSTOM_PALETTE (4) }"
  },
  {
    GIMP_PDB_INT32,
    "num_cols",
    "the number of colors to quantize to, ignored unless (palette_type == MAKE_PALETTE)"
  },
  {
    GIMP_PDB_INT32,
    "alpha_dither",
    "dither transparency to fake partial opacity"
  },
  {
    GIMP_PDB_INT32,
    "remove_unused",
    "remove unused or duplicate colour entries from final palette, ignored if (palette_type == MAKE_PALETTE)"
  },
  {
    GIMP_PDB_STRING,
    "palette",
    "The name of the custom palette to use, ignored unless (palette_type == CUSTOM_PALETTE)"
  }
};

static ProcRecord convert_indexed_proc =
{
  "gimp_convert_indexed",
  "Convert specified image to and Indexed image",
  "This procedure converts the specified image to 'indexed' color. This process requires an image of type GRAY or RGB. The 'palette_type' specifies what kind of palette to use, A type of '0' means to use an optimal palette of 'num_cols' generated from the colors in the image. A type of '1' means to re-use the previous palette (not currently implemented). A type of '2' means to use the so-called WWW-optimized palette. Type '3' means to use only black and white colors. A type of '4' means to use a palette from the gimp palettes directories. The 'dither type' specifies what kind of dithering to use. '0' means no dithering, '1' means standard Floyd-Steinberg error diffusion, '2' means Floyd-Steinberg error diffusion with reduced bleeding, '3' means dithering based on pixel location ('Fixed' dithering).",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  GIMP_INTERNAL,
  7,
  convert_indexed_inargs,
  0,
  NULL,
  { { convert_indexed_invoker } }
};
