/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1999 Spencer Kimball and Peter Mattis
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

#include "procedural_db.h"

#include "config.h"

#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "fileops.h"
#include "fileopsP.h"
#include "gimprc.h"
#include "plug_in.h"

#include "libgimp/gimpintl.h"

static ProcRecord file_load_proc;
static ProcRecord file_save_proc;
static ProcRecord file_load_thumbnail_proc;
static ProcRecord file_save_thumbnail_proc;
static ProcRecord temp_name_proc;
static ProcRecord register_magic_load_handler_proc;
static ProcRecord register_load_handler_proc;
static ProcRecord register_save_handler_proc;

void
register_fileops_procs (void)
{
  procedural_db_register (&file_load_proc);
  procedural_db_register (&file_save_proc);
  procedural_db_register (&file_load_thumbnail_proc);
  procedural_db_register (&file_save_thumbnail_proc);
  procedural_db_register (&temp_name_proc);
  procedural_db_register (&register_magic_load_handler_proc);
  procedural_db_register (&register_load_handler_proc);
  procedural_db_register (&register_save_handler_proc);
}

static Argument *
file_load_invoker (Argument *args)
{
  PlugInProcDef *file_proc;
  ProcRecord *proc;

  file_proc = file_proc_find (load_procs, (gchar *) args[2].value.pdb_pointer);
  if (!file_proc)
    return procedural_db_return_args (&file_load_proc, FALSE);

  proc = &file_proc->db_info;

  return procedural_db_execute (proc->name, args);
}

static ProcArg file_load_inargs[] =
{
  {
    PDB_INT32,
    "run_mode",
    "The run mode: RUN_INTERACTIVE (0) or RUN_NONINTERACTIVE (1)"
  },
  {
    PDB_STRING,
    "filename",
    "The name of the file to load"
  },
  {
    PDB_STRING,
    "raw_filename",
    "The name entered"
  }
};

static ProcArg file_load_outargs[] =
{
  {
    PDB_IMAGE,
    "image",
    "The output image"
  }
};

static ProcRecord file_load_proc =
{
  "gimp_file_load",
  "Loads a file by extension.",
  "This procedure invokes the correct file load handler according to the file's extension and/or prefix. The name of the file to load is typically a full pathname, and the name entered is what the user actually typed before prepending a directory path. The reason for this is that if the user types http://www.xcf/~gimp/ he wants to fetch a URL, and the full pathname will not look like a URL.",
  "Josh MacDonald",
  "Josh MacDonald",
  "1997",
  PDB_INTERNAL,
  3,
  file_load_inargs,
  1,
  file_load_outargs,
  { { file_load_invoker } }
};

static Argument *
file_save_invoker (Argument *args)
{
  Argument *new_args;
  Argument *return_vals;
  PlugInProcDef *file_proc;
  ProcRecord *proc;
  gint i;

  file_proc = file_proc_find (save_procs, (gchar *) args[4].value.pdb_pointer);
  if (!file_proc) 
    return procedural_db_return_args (&file_save_proc, FALSE);

  proc = &file_proc->db_info;

  new_args = g_new (Argument, proc->num_args);
  memset (new_args, 0, sizeof (Argument) * proc->num_args);
  memcpy (new_args, args, sizeof (Argument) * 5);

  for (i=5; i<proc->num_args; i++)
  {
    new_args[i].arg_type = proc->args[i].arg_type;
    if (proc->args[i].arg_type == PDB_STRING)
      new_args[i].value.pdb_pointer = g_strdup("\0");
  }

  return_vals = procedural_db_execute (proc->name, new_args);
  g_free (new_args);

  return return_vals;
}

static ProcArg file_save_inargs[] =
{
  {
    PDB_INT32,
    "run_mode",
    "The run mode: RUN_INTERACTIVE (0) or RUN_NONINTERACTIVE (1)"
  },
  {
    PDB_IMAGE,
    "image",
    "Input image"
  },
  {
    PDB_DRAWABLE,
    "drawable",
    "Drawable to save"
  },
  {
    PDB_STRING,
    "filename",
    "The name of the file to save the image in"
  },
  {
    PDB_STRING,
    "raw_filename",
    "The name of the file to save the image in"
  }
};

static ProcRecord file_save_proc =
{
  "gimp_file_save",
  "Saves a file by extension.",
  "This procedure invokes the correct file save handler according to the file's extension and/or prefix. The name of the file to save is typically a full pathname, and the name entered is what the user actually typed before prepending a directory path. The reason for this is that if the user types http://www.xcf/~gimp/ she wants to fetch a URL, and the full pathname will not look like a URL.",
  "Josh MacDonald",
  "Josh MacDonald",
  "1997",
  PDB_INTERNAL,
  5,
  file_save_inargs,
  0,
  NULL,
  { { file_save_invoker } }
};

static Argument *
file_load_thumbnail_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *filename;
  gint32 width = 0;
  gint32 height = 0;
  guint8 *thumb_data = NULL;
  gchar *pname;
  gchar *fname;
  gchar *tname;
  guchar *raw_thumb;
  gchar *imginfo = NULL;
  gint i;

  filename = (gchar *) args[0].value.pdb_pointer;
  if (filename == NULL)
    success = FALSE;

  if (success)
    {
      pname = g_dirname (filename);
      fname = g_basename (filename);
      tname = g_strconcat (pname, G_DIR_SEPARATOR_S, ".xvpics", G_DIR_SEPARATOR_S,
			   fname,
			   NULL);
      g_free (pname);
      raw_thumb = readXVThumb (tname, &width, &height, &imginfo);
      g_free (tname);
    
      if (raw_thumb)
	{
	  thumb_data = g_malloc (3 * width * height);
	      
	  for (i=0; i<width*height; i++)
	    {
	      thumb_data[i*3  ] = ((raw_thumb[i]>>5)*255)/7;
	      thumb_data[i*3+1] = (((raw_thumb[i]>>2)&7)*255)/7;
	      thumb_data[i*3+2] = (((raw_thumb[i])&3)*255)/3;
	    }
	  g_free (raw_thumb);
	  success = TRUE;
	}
      else
	success = FALSE;
    }

  return_args = procedural_db_return_args (&file_load_thumbnail_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = width;
      return_args[2].value.pdb_int = height;
      return_args[3].value.pdb_pointer = thumb_data;
    }

  return return_args;
}

static ProcArg file_load_thumbnail_inargs[] =
{
  {
    PDB_STRING,
    "filename",
    "The name of the file that owns the thumbnail to load"
  }
};

static ProcArg file_load_thumbnail_outargs[] =
{
  {
    PDB_INT32,
    "width",
    "The width of the thumbnail"
  },
  {
    PDB_INT32,
    "height",
    "The height of the thumbnail"
  },
  {
    PDB_INT8ARRAY,
    "thumb_data",
    "The thumbnail data"
  }
};

static ProcRecord file_load_thumbnail_proc =
{
  "gimp_file_load_thumbnail",
  "Loads the thumbnail for a file.",
  "This procedure tries to load a thumbnail that belongs to the file with the given filename. This name is a full pathname. The returned data is an array of colordepth 3 (RGB), regardless of the image type. Width and height of the thumbnail are also returned. Don't use this function if you need a thumbnail of an already opened image, use gimp_image_thumbnail instead.",
  "Adam D. Moss, Sven Neumann",
  "Adam D. Moss, Sven Neumann",
  "1999",
  PDB_INTERNAL,
  1,
  file_load_thumbnail_inargs,
  3,
  file_load_thumbnail_outargs,
  { { file_load_thumbnail_invoker } }
};

static Argument *
file_save_thumbnail_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpImage *gimage;
  gchar *filename;
  TempBuf *thumb;

  gimage = pdb_id_to_image (args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  filename = (gchar *) args[1].value.pdb_pointer;
  if (filename == NULL)
    success = FALSE;

  if (success)
    {
      thumb = make_thumb_tempbuf (gimage);
      if (file_save_thumbnail (gimage, filename, thumb))
	success = TRUE;
    }

  return procedural_db_return_args (&file_save_thumbnail_proc, success);
}

static ProcArg file_save_thumbnail_inargs[] =
{
  {
    PDB_IMAGE,
    "image",
    "The image"
  },
  {
    PDB_STRING,
    "filename",
    "The name of the file the thumbnail belongs to"
  }
};

static ProcRecord file_save_thumbnail_proc =
{
  "gimp_file_save_thumbnail",
  "Saves a thumbnail for the given image",
  "This procedure saves a thumbnail in the .xvpics format for the given image. The thumbnail is saved so that it belongs to the file with the given filename. This means you have to save the image under this name first, otherwise this procedure will fail. This procedure may become useful if you want to explicitely save a thumbnail with a file.",
  "Josh MacDonald",
  "Josh MacDonald",
  "1997",
  PDB_INTERNAL,
  2,
  file_save_thumbnail_inargs,
  0,
  NULL,
  { { file_save_thumbnail_invoker } }
};

static Argument *
temp_name_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *extension;
  gchar *name = NULL;
  static gint id = 0;
  static gint pid;

  extension = (gchar *) args[0].value.pdb_pointer;
  if (extension == NULL)
    success = FALSE;

  if (success)
    {
      if (id == 0)
	pid = getpid();
    
      name = g_strdup_printf ("%s" G_DIR_SEPARATOR_S "gimp_temp.%d%d.%s",
			      temp_path, pid, id++, extension);
    }

  return_args = procedural_db_return_args (&temp_name_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = name;

  return return_args;
}

static ProcArg temp_name_inargs[] =
{
  {
    PDB_STRING,
    "extension",
    "The extension the file will have"
  }
};

static ProcArg temp_name_outargs[] =
{
  {
    PDB_STRING,
    "name",
    "The new temp filename"
  }
};

static ProcRecord temp_name_proc =
{
  "gimp_temp_name",
  "Generates a unique filename.",
  "Generates a unique filename using the temp path supplied in the user's gimprc.",
  "Josh MacDonald",
  "Josh MacDonald",
  "1997",
  PDB_INTERNAL,
  1,
  temp_name_inargs,
  1,
  temp_name_outargs,
  { { temp_name_invoker } }
};

static Argument *
register_magic_load_handler_invoker (Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  gchar *extensions;
  gchar *prefixes;
  gchar *magics;
  ProcRecord *proc;
  PlugInProcDef *file_proc;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  extensions = (gchar *) args[1].value.pdb_pointer;

  prefixes = (gchar *) args[2].value.pdb_pointer;

  magics = (gchar *) args[3].value.pdb_pointer;

  if (success)
    {
      success = FALSE;
    
      proc = procedural_db_lookup (name);
    
      if (proc && ((proc->num_args < 3) ||
		   (proc->num_values < 1) ||
		   (proc->args[0].arg_type != PDB_INT32) ||
		   (proc->args[1].arg_type != PDB_STRING) ||
		   (proc->args[2].arg_type != PDB_STRING) ||
		   (proc->values[0].arg_type != PDB_IMAGE)))
	{
	  g_message (_("load handler \"%s\" does not take the standard load handler args"),
		     name);
	  goto done;
	}
    
      file_proc = plug_in_file_handler (name, extensions, prefixes, magics);
    
      if (!file_proc)
	{
	  g_message (_("attempt to register non-existant load handler \"%s\""),
		     name);
	  goto done;
	}
    
      load_procs = g_slist_prepend (load_procs, file_proc);
    
      success = TRUE;
    
    done: ;
    }

  return procedural_db_return_args (&register_magic_load_handler_proc, success);
}

static ProcArg register_magic_load_handler_inargs[] =
{
  {
    PDB_STRING,
    "procedure_name",
    "The name of the procedure to be used for loading"
  },
  {
    PDB_STRING,
    "extensions",
    "comma separated list of extensions this handler can load (i.e. \"jpg,jpeg\")"
  },
  {
    PDB_STRING,
    "prefixes",
    "comma separated list of prefixes this handler can load (i.e. \"http:,ftp:\")"
  },
  {
    PDB_STRING,
    "magics",
    "comma separated list of magic file information this handler can load (i.e. \"0,string,GIF\")"
  }
};

static ProcRecord register_magic_load_handler_proc =
{
  "gimp_register_magic_load_handler",
  "Registers a file load handler procedure.",
  "Registers a procedural database procedure to be called to load files of a particular file format using magic file information.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  4,
  register_magic_load_handler_inargs,
  0,
  NULL,
  { { register_magic_load_handler_invoker } }
};

static Argument *
register_load_handler_invoker (Argument *args)
{
  int i;
  Argument argv[4];

  for (i = 0; i < 3; i++)
    argv[i] = args[i];

  argv[3].arg_type = PDB_STRING;
  argv[3].value.pdb_pointer = NULL;

  return register_magic_load_handler_invoker (argv);
}

static ProcArg register_load_handler_inargs[] =
{
  {
    PDB_STRING,
    "procedure_name",
    "The name of the procedure to be used for loading"
  },
  {
    PDB_STRING,
    "extensions",
    "comma separated list of extensions this handler can load (i.e. \"jpg,jpeg\")"
  },
  {
    PDB_STRING,
    "prefixes",
    "comma separated list of prefixes this handler can load (i.e. \"http:,ftp:\")"
  }
};

static ProcRecord register_load_handler_proc =
{
  "gimp_register_load_handler",
  "Registers a file load handler procedure.",
  "Registers a procedural database procedure to be called to load files of a particular file format.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  3,
  register_load_handler_inargs,
  0,
  NULL,
  { { register_load_handler_invoker } }
};

static Argument *
register_save_handler_invoker (Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  gchar *extensions;
  gchar *prefixes;
  ProcRecord *proc;
  PlugInProcDef *file_proc;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  extensions = (gchar *) args[1].value.pdb_pointer;

  prefixes = (gchar *) args[2].value.pdb_pointer;

  if (success)
    {
      success = FALSE;
    
      proc = procedural_db_lookup (name);
    
      if (proc && ((proc->num_args < 5) ||
		   (proc->args[0].arg_type != PDB_INT32) ||
		   (proc->args[1].arg_type != PDB_IMAGE) ||
		   (proc->args[2].arg_type != PDB_DRAWABLE) ||
		   (proc->args[3].arg_type != PDB_STRING) ||
		   (proc->args[4].arg_type != PDB_STRING)))
	{
	  g_message (_("save handler \"%s\" does not take the standard save handler args"),
		     name);
	  goto done;
	}
    
      file_proc = plug_in_file_handler (name, extensions, prefixes, NULL);
    
      if (!file_proc)
	{
	  g_message (_("attempt to register non-existant save handler \"%s\""),
		     name);
	  goto done;
	}
    
      save_procs = g_slist_prepend (save_procs, file_proc);
    
      success = TRUE;
    
    done: ;
    }

  return procedural_db_return_args (&register_save_handler_proc, success);
}

static ProcArg register_save_handler_inargs[] =
{
  {
    PDB_STRING,
    "procedure_name",
    "The name of the procedure to be used for saving"
  },
  {
    PDB_STRING,
    "extensions",
    "comma separated list of extensions this handler can save (i.e. \"jpg,jpeg\")"
  },
  {
    PDB_STRING,
    "prefixes",
    "comma separated list of prefixes this handler can save (i.e. \"http:,ftp:\")"
  }
};

static ProcRecord register_save_handler_proc =
{
  "gimp_register_save_handler",
  "Registers a file save handler procedure.",
  "Registers a procedural database procedure to be called to save files in a particular file format.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  3,
  register_save_handler_inargs,
  0,
  NULL,
  { { register_save_handler_invoker } }
};
