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

#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include "core/core-types.h"

#include "core/gimp.h"
#include "core/gimpimage.h"

#include "xcf.h"
#include "xcf-private.h"
#include "xcf-load.h"
#include "xcf-read.h"
#include "xcf-save.h"

#include "plug_in.h"

#include "libgimp/gimpintl.h"


typedef GimpImage * GimpXcfLoaderFunc (Gimp    *gimp,
				       XcfInfo *info);


static Argument * xcf_load_invoker (Gimp     *gimp,
				    Argument *args);
static Argument * xcf_save_invoker (Gimp     *gimp,
				    Argument *args);


static ProcArg xcf_load_args[] =
{
  { GIMP_PDB_INT32,
    "dummy_param",
    "dummy parameter" },
  { GIMP_PDB_STRING,
    "filename",
    "The name of the file to load" },
  { GIMP_PDB_STRING,
    "raw_filename",
    "The name of the file to load" },
};

static ProcArg xcf_load_return_vals[] =
{
  { GIMP_PDB_IMAGE,
    "image",
    "Output image" },
};

static PlugInProcDef xcf_plug_in_load_proc =
{
  "gimp_xcf_load",
  "<Load>/XCF (GIMP)",
  NULL,
  "xcf",
  "",
  "0,string,gimp\\040xcf\\040",
  NULL, /* ignored for load */
  0,    /* ignored for load */
  {
    "gimp_xcf_load",
    "loads file saved in the .xcf file format",
    "The xcf file format has been designed specifically for loading and saving tiled and layered images in the GIMP. This procedure will load the specified file.",
    "Spencer Kimball & Peter Mattis",
    "Spencer Kimball & Peter Mattis",
    "1995-1996",
    GIMP_INTERNAL,
    3,
    xcf_load_args,
    1,
    xcf_load_return_vals,
    { { xcf_load_invoker } },
  },
  NULL, /* fill me in at runtime */
  NULL /* fill me in at runtime */
};

static ProcArg xcf_save_args[] =
{
  { GIMP_PDB_INT32,
    "dummy_param",
    "dummy parameter" },
  { GIMP_PDB_IMAGE,
    "image",
    "Input image" },
  { GIMP_PDB_DRAWABLE,
    "drawable",
    "Active drawable of input image" },
  { GIMP_PDB_STRING,
    "filename",
    "The name of the file to save the image in" },
  { GIMP_PDB_STRING,
    "raw_filename",
    "The name of the file to load" },
};

static PlugInProcDef xcf_plug_in_save_proc =
{
  "gimp_xcf_save",
  "<Save>/XCF (GIMP)",
  NULL,
  "xcf",
  "",
  NULL,
  "RGB*, GRAY*, INDEXED*",
  0, /* fill me in at runtime */
  {
    "gimp_xcf_save",
    "saves file in the .xcf file format",
    "The xcf file format has been designed specifically for loading and saving tiled and layered images in the GIMP. This procedure will save the specified image in the xcf file format.",
    "Spencer Kimball & Peter Mattis",
    "Spencer Kimball & Peter Mattis",
    "1995-1996",
    GIMP_INTERNAL,
    5,
    xcf_save_args,
    0,
    NULL,
    { { xcf_save_invoker } },
  },
  NULL, /* fill me in at runtime */
  NULL /* fill me in at runtime */
};


static GimpXcfLoaderFunc *xcf_loaders[] =
{
  xcf_load_image,	/* version 0 */
  xcf_load_image	/* version 1 */
};
static gint n_xcf_loaders = sizeof (xcf_loaders) / sizeof (xcf_loaders[0]);


void
xcf_init (Gimp *gimp)
{
  /* So this is sort of a hack, but its better than it was before.  To do this
   * right there would be a file load-save handler type and the whole interface
   * would change but there isn't, and currently the plug-in structure contains
   * all the load-save info, so it makes sense to use that for the XCF load/save
   * handlers, even though they are internal.  The only thing it requires is
   * using a PlugInProcDef struct.  -josh
   */
  procedural_db_register (gimp, &xcf_plug_in_save_proc.db_info);
  procedural_db_register (gimp, &xcf_plug_in_load_proc.db_info);

  xcf_plug_in_save_proc.image_types_val =
    plug_in_image_types_parse (xcf_plug_in_save_proc.image_types);
  xcf_plug_in_load_proc.image_types_val =
    plug_in_image_types_parse (xcf_plug_in_load_proc.image_types);

  plug_in_add_internal (&xcf_plug_in_save_proc);
  plug_in_add_internal (&xcf_plug_in_load_proc);
}

void
xcf_exit (void)
{
}

static Argument*
xcf_load_invoker (Gimp     *gimp,
		  Argument *args)
{
  XcfInfo    info;
  Argument  *return_args;
  GimpImage *gimage;
  gchar     *filename;
  gboolean   success;
  gchar      id[14];

  gimp_set_busy (gimp);

  gimage = NULL;

  success = FALSE;

  filename = args[1].value.pdb_pointer;

  info.fp = fopen (filename, "rb");

  if (info.fp)
    {
      info.cp                    = 0;
      info.filename              = filename;
      info.active_layer          = NULL;
      info.active_channel        = NULL;
      info.floating_sel_drawable = NULL;
      info.floating_sel          = NULL;
      info.floating_sel_offset   = 0;
      info.swap_num              = 0;
      info.ref_count             = NULL;
      info.compression           = COMPRESS_NONE;

      success = TRUE;

      info.cp += xcf_read_int8 (info.fp, (guint8*) id, 14);

      if (strncmp (id, "gimp xcf ", 9) != 0)
	{
	  success = FALSE;
	}
      else if (strcmp (id+9, "file") == 0) 
	{
	  info.file_version = 0;
	} 
      else if (id[9] == 'v') 
	{
	  info.file_version = atoi(id + 10);
	}
      else
	{
	  success = FALSE;
	}
      
      if (success)
	{
	  if (info.file_version < n_xcf_loaders)
	    {
	      gimage = (*(xcf_loaders[info.file_version])) (gimp, &info);

	      if (! gimage)
		success = FALSE;
	    }
	  else 
	    {
	      g_message (_("XCF error: unsupported XCF file version %d "
			   "encountered"), info.file_version);
	      success = FALSE;
	    }
	}

      fclose (info.fp);
    }
  
  return_args = procedural_db_return_args (&xcf_plug_in_load_proc.db_info,
					   success);

  if (success)
    return_args[1].value.pdb_int = gimp_image_get_ID (gimage);

  gimp_unset_busy (gimp);

  return return_args;
}

static Argument *
xcf_save_invoker (Gimp     *gimp,
		  Argument *args)
{
  XcfInfo    info;
  Argument  *return_args;
  GimpImage *gimage;
  gchar     *filename;
  gboolean   success;

  gimp_set_busy (gimp);

  success = FALSE;

  gimage   = gimp_image_get_by_ID (gimp, args[1].value.pdb_int);
  filename = args[3].value.pdb_pointer;

  info.fp = fopen (filename, "wb");

  if (info.fp)
    {
      info.cp                    = 0;
      info.filename              = filename;
      info.active_layer          = NULL;
      info.active_channel        = NULL;
      info.floating_sel_drawable = NULL;
      info.floating_sel          = NULL;
      info.floating_sel_offset   = 0;
      info.swap_num              = 0;
      info.ref_count             = NULL;
      info.compression           = COMPRESS_RLE;

      xcf_save_choose_format (&info, gimage);

      success = xcf_save_image (&info, gimage);
      fclose (info.fp);
    }
  else
    {
      g_message (_("open failed on %s: %s\n"), filename, g_strerror (errno));
    }

  return_args = procedural_db_return_args (&xcf_plug_in_save_proc.db_info,
					   success);

  gimp_unset_busy (gimp);

  return return_args;
}
