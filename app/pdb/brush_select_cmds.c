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

#include <string.h>

#include <gtk/gtk.h>

#include "libgimpbase/gimpbasetypes.h"

#include "pdb-types.h"
#include "widgets/widgets-types.h"
#include "procedural_db.h"

#include "base/base-types.h"
#include "core/gimp.h"
#include "core/gimpbrush.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimplist.h"
#include "gui/brush-select.h"
#include "widgets/gimpbrushfactoryview.h"

static ProcRecord brushes_popup_proc;
static ProcRecord brushes_close_popup_proc;
static ProcRecord brushes_set_popup_proc;

void
register_brush_select_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &brushes_popup_proc);
  procedural_db_register (gimp, &brushes_close_popup_proc);
  procedural_db_register (gimp, &brushes_set_popup_proc);
}

static Argument *
brushes_popup_invoker (Gimp     *gimp,
                       Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  gchar *title;
  gchar *brush;
  gdouble opacity;
  gint32 spacing;
  gint32 paint_mode;
  ProcRecord *prec;
  BrushSelect *newdialog;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  title = (gchar *) args[1].value.pdb_pointer;
  if (title == NULL)
    success = FALSE;

  brush = (gchar *) args[2].value.pdb_pointer;

  opacity = args[3].value.pdb_float;
  if (opacity < 0.0 || opacity > 100.0)
    success = FALSE;

  spacing = args[4].value.pdb_int;
  if (spacing < 0 || spacing > 1000)
    success = FALSE;

  paint_mode = args[5].value.pdb_int;
  if (paint_mode < NORMAL_MODE || paint_mode > HARDLIGHT_MODE)
    success = FALSE;

  if (success)
    {
      if ((prec = procedural_db_lookup (gimp, name)))
	{
	  if (brush && strlen (brush))
	    newdialog = brush_select_new (gimp, title, brush,
					  opacity / 100.0,
					  spacing,
					  paint_mode, name);
	  else
	    newdialog = brush_select_new (gimp, title, NULL, 0.0, 0, 0, name);
	}
      else
	{
	  success = FALSE;
	}
    }

  return procedural_db_return_args (&brushes_popup_proc, success);
}

static ProcArg brushes_popup_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "brush_callback",
    "The callback PDB proc to call when brush selection is made"
  },
  {
    GIMP_PDB_STRING,
    "popup_title",
    "Title to give the brush popup window"
  },
  {
    GIMP_PDB_STRING,
    "initial_brush",
    "The name of the brush to set as the first selected"
  },
  {
    GIMP_PDB_FLOAT,
    "opacity",
    "The initial opacity of the brush"
  },
  {
    GIMP_PDB_INT32,
    "spacing",
    "The initial spacing of the brush (if < 0 then use brush default spacing)"
  },
  {
    GIMP_PDB_INT32,
    "paint_mode",
    "The initial paint mode: { NORMAL_MODE (0), DISSOLVE_MODE (1), BEHIND_MODE (2), MULTIPLY_MODE (3), SCREEN_MODE (4), OVERLAY_MODE (5), DIFFERENCE_MODE (6), ADDITION_MODE (7), SUBTRACT_MODE (8), DARKEN_ONLY_MODE (9), LIGHTEN_ONLY_MODE (10), HUE_MODE (11), SATURATION_MODE (12), COLOR_MODE (13), VALUE_MODE (14), DIVIDE_MODE (15), DODGE_MODE (16), BURN_MODE (17), HARDLIGHT_MODE (18) }"
  }
};

static ProcRecord brushes_popup_proc =
{
  "gimp_brushes_popup",
  "Invokes the Gimp brush selection.",
  "This procedure popups the brush selection dialog.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  GIMP_INTERNAL,
  6,
  brushes_popup_inargs,
  0,
  NULL,
  { { brushes_popup_invoker } }
};

static Argument *
brushes_close_popup_invoker (Gimp     *gimp,
                             Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  ProcRecord *prec;
  BrushSelect *bsp;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  if (success)
    {
      if ((prec = procedural_db_lookup (gimp, name)) &&
	  (bsp = brush_select_get_by_callback (name)))
	{
	  brush_select_free (bsp);
	}
      else
	{
	  success = FALSE;
	}
    }

  return procedural_db_return_args (&brushes_close_popup_proc, success);
}

static ProcArg brushes_close_popup_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "brush_callback",
    "The name of the callback registered for this popup"
  }
};

static ProcRecord brushes_close_popup_proc =
{
  "gimp_brushes_close_popup",
  "Popdown the Gimp brush selection.",
  "This procedure closes an opened brush selection dialog.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  GIMP_INTERNAL,
  1,
  brushes_close_popup_inargs,
  0,
  NULL,
  { { brushes_close_popup_invoker } }
};

static Argument *
brushes_set_popup_invoker (Gimp     *gimp,
                           Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  gchar *brush_name;
  gdouble opacity;
  gint32 spacing;
  gint32 paint_mode;
  ProcRecord *prec;
  BrushSelect *bsp;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  brush_name = (gchar *) args[1].value.pdb_pointer;
  if (brush_name == NULL)
    success = FALSE;

  opacity = args[2].value.pdb_float;
  if (opacity < 0.0 || opacity > 100.0)
    success = FALSE;

  spacing = args[3].value.pdb_int;
  if (spacing < 0 || spacing > 1000)
    success = FALSE;

  paint_mode = args[4].value.pdb_int;
  if (paint_mode < NORMAL_MODE || paint_mode > HARDLIGHT_MODE)
    success = FALSE;

  if (success)
    {
      if ((prec = procedural_db_lookup (gimp, name)) &&
	  (bsp = brush_select_get_by_callback (name)))
	{
	  GimpObject *object =
	    gimp_container_get_child_by_name (gimp->brush_factory->container,
					      brush_name);
    
	  if (object)
	    {
	      GimpBrush     *active = GIMP_BRUSH (object);
	      GtkAdjustment *spacing_adj;
    
	      spacing_adj = GIMP_BRUSH_FACTORY_VIEW (bsp->view)->spacing_adjustment;
    
	      /* Updating the context updates the widgets as well */
    
	      gimp_context_set_brush (bsp->context, active);
	      gimp_context_set_opacity (bsp->context, opacity / 100.0);
	      gimp_context_set_paint_mode (bsp->context, paint_mode);
    
	      gtk_adjustment_set_value (spacing_adj, spacing);
	    }
	  else
	    success = FALSE;
	}
      else
	success = FALSE;
    }

  return procedural_db_return_args (&brushes_set_popup_proc, success);
}

static ProcArg brushes_set_popup_inargs[] =
{
  {
    GIMP_PDB_STRING,
    "brush_callback",
    "The name of the callback registered for this popup"
  },
  {
    GIMP_PDB_STRING,
    "brush_name",
    "The name of the brush to set as selected"
  },
  {
    GIMP_PDB_FLOAT,
    "opacity",
    "The initial opacity of the brush"
  },
  {
    GIMP_PDB_INT32,
    "spacing",
    "The initial spacing of the brush (if < 0 then use brush default spacing)"
  },
  {
    GIMP_PDB_INT32,
    "paint_mode",
    "The initial paint mode: { NORMAL_MODE (0), DISSOLVE_MODE (1), BEHIND_MODE (2), MULTIPLY_MODE (3), SCREEN_MODE (4), OVERLAY_MODE (5), DIFFERENCE_MODE (6), ADDITION_MODE (7), SUBTRACT_MODE (8), DARKEN_ONLY_MODE (9), LIGHTEN_ONLY_MODE (10), HUE_MODE (11), SATURATION_MODE (12), COLOR_MODE (13), VALUE_MODE (14), DIVIDE_MODE (15), DODGE_MODE (16), BURN_MODE (17), HARDLIGHT_MODE (18) }"
  }
};

static ProcRecord brushes_set_popup_proc =
{
  "gimp_brushes_set_popup",
  "Sets the current brush selection in a popup.",
  "Sets the current brush selection in a popup.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  GIMP_INTERNAL,
  5,
  brushes_set_popup_inargs,
  0,
  NULL,
  { { brushes_set_popup_invoker } }
};
