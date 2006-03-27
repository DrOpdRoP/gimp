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


#include <glib-object.h>

#include "pdb-types.h"
#include "procedural_db.h"

#include "core/gimp.h"
#include "core/gimpcontainer.h"
#include "core/gimpimage.h"

static ProcRecord display_new_proc;
static ProcRecord display_delete_proc;
static ProcRecord display_get_window_handle_proc;
static ProcRecord displays_flush_proc;
static ProcRecord displays_reconnect_proc;

void
register_display_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &display_new_proc);
  procedural_db_register (gimp, &display_delete_proc);
  procedural_db_register (gimp, &display_get_window_handle_proc);
  procedural_db_register (gimp, &displays_flush_proc);
  procedural_db_register (gimp, &displays_reconnect_proc);
}

static Argument *
display_new_invoker (ProcRecord   *proc_record,
                     Gimp         *gimp,
                     GimpContext  *context,
                     GimpProgress *progress,
                     Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *image;
  GimpObject *display = NULL;

  image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (image))
    success = FALSE;

  if (success)
    {
      display = gimp_create_display (gimp, image, GIMP_UNIT_PIXEL, 1.0);

      if (display)
        {
          /* the first display takes ownership of the image */
          if (image->disp_count == 1)
            g_object_unref (image);
        }
      else
        success = FALSE;
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    return_args[1].value.pdb_int = display ? gimp_get_display_ID (gimp, display) : -1;

  return return_args;
}

static ProcArg display_new_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "image",
    "The image"
  }
};

static ProcArg display_new_outargs[] =
{
  {
    GIMP_PDB_DISPLAY,
    "display",
    "The new display"
  }
};

static ProcRecord display_new_proc =
{
  "gimp-display-new",
  "gimp-display-new",
  "Create a new display for the specified image.",
  "Creates a new display for the specified image. If the image already has a display, another is added. Multiple displays are handled transparently by the GIMP. The newly created display is returned and can be subsequently destroyed with a call to 'gimp-display-delete'. This procedure only makes sense for use with the GIMP UI.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  1,
  display_new_inargs,
  1,
  display_new_outargs,
  { { display_new_invoker } }
};

static Argument *
display_delete_invoker (ProcRecord   *proc_record,
                        Gimp         *gimp,
                        GimpContext  *context,
                        GimpProgress *progress,
                        Argument     *args)
{
  gboolean success = TRUE;
  GimpObject *display;

  display = gimp_get_display_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_OBJECT (display))
    success = FALSE;

  if (success)
    {
      gimp_delete_display (gimp, display);
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg display_delete_inargs[] =
{
  {
    GIMP_PDB_DISPLAY,
    "display",
    "The display to delete"
  }
};

static ProcRecord display_delete_proc =
{
  "gimp-display-delete",
  "gimp-display-delete",
  "Delete the specified display.",
  "This procedure removes the specified display. If this is the last remaining display for the underlying image, then the image is deleted also.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  1,
  display_delete_inargs,
  0,
  NULL,
  { { display_delete_invoker } }
};

static Argument *
display_get_window_handle_invoker (ProcRecord   *proc_record,
                                   Gimp         *gimp,
                                   GimpContext  *context,
                                   GimpProgress *progress,
                                   Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpObject *display;
  gint32 window = 0;

  display = gimp_get_display_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_OBJECT (display))
    success = FALSE;

  if (success)
    {
      window = (gint32) gimp_get_display_window (gimp, display);
    }

  return_args = procedural_db_return_args (proc_record, success);

  if (success)
    return_args[1].value.pdb_int = window;

  return return_args;
}

static ProcArg display_get_window_handle_inargs[] =
{
  {
    GIMP_PDB_DISPLAY,
    "display",
    "The display to get the window handle from"
  }
};

static ProcArg display_get_window_handle_outargs[] =
{
  {
    GIMP_PDB_INT32,
    "window",
    "The native window handle or 0"
  }
};

static ProcRecord display_get_window_handle_proc =
{
  "gimp-display-get-window-handle",
  "gimp-display-get-window-handle",
  "Get a handle to the native window for an image display.",
  "This procedure returns a handle to the native window for a given image display. For example in the X backend of GDK, a native window handle is an Xlib XID. A value of 0 is returned for an invalid display or if this function is unimplemented for the windowing system that is being used.",
  "Sven Neumann <sven@gimp.org>",
  "Sven Neumann",
  "2005",
  NULL,
  GIMP_INTERNAL,
  1,
  display_get_window_handle_inargs,
  1,
  display_get_window_handle_outargs,
  { { display_get_window_handle_invoker } }
};

static Argument *
displays_flush_invoker (ProcRecord   *proc_record,
                        Gimp         *gimp,
                        GimpContext  *context,
                        GimpProgress *progress,
                        Argument     *args)
{
  gimp_container_foreach (gimp->images, (GFunc) gimp_image_flush, NULL);
  return procedural_db_return_args (proc_record, TRUE);
}

static ProcRecord displays_flush_proc =
{
  "gimp-displays-flush",
  "gimp-displays-flush",
  "Flush all internal changes to the user interface",
  "This procedure takes no arguments and returns nothing except a success status. Its purpose is to flush all pending updates of image manipulations to the user interface. It should be called whenever appropriate.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  0,
  NULL,
  0,
  NULL,
  { { displays_flush_invoker } }
};

static Argument *
displays_reconnect_invoker (ProcRecord   *proc_record,
                            Gimp         *gimp,
                            GimpContext  *context,
                            GimpProgress *progress,
                            Argument     *args)
{
  gboolean success = TRUE;
  GimpImage *old_image;
  GimpImage *new_image;

  old_image = gimp_image_get_by_ID (gimp, args[0].value.pdb_int);
  if (! GIMP_IS_IMAGE (old_image))
    success = FALSE;

  new_image = gimp_image_get_by_ID (gimp, args[1].value.pdb_int);
  if (! GIMP_IS_IMAGE (new_image))
    success = FALSE;

  if (success)
    {
      success = (old_image != new_image    &&
                 old_image->disp_count > 0 &&
                 new_image->disp_count == 0);

      if (success)
        {
          gimp_reconnect_displays (gimp, old_image, new_image);

          /* take ownership of the image */
          if (new_image->disp_count > 0)
            g_object_unref (new_image);
        }
    }

  return procedural_db_return_args (proc_record, success);
}

static ProcArg displays_reconnect_inargs[] =
{
  {
    GIMP_PDB_IMAGE,
    "old-image",
    "The old image (must have at least one display)"
  },
  {
    GIMP_PDB_IMAGE,
    "new-image",
    "The new image (must not have a display)"
  }
};

static ProcRecord displays_reconnect_proc =
{
  "gimp-displays-reconnect",
  "gimp-displays-reconnect",
  "Reconnect displays from one image to another image.",
  "This procedure connects all displays of the old_image to the new_image. If the old_image has no display or new_image already has a display the reconnect is not performed and the procedure returns without success. You should rarely need to use this function.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  NULL,
  GIMP_INTERNAL,
  2,
  displays_reconnect_inargs,
  0,
  NULL,
  { { displays_reconnect_invoker } }
};
