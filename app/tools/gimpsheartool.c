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

#include <stdlib.h>

#include <gtk/gtk.h>

#include "libgimpmath/gimpmath.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"
#include "gui/gui-types.h"

#include "core/gimpimage.h"
#include "core/gimpdrawable-transform.h"
#include "core/gimpdrawable-transform-utils.h"
#include "core/gimptoolinfo.h"

#include "display/gimpdisplay.h"

#include "gui/info-dialog.h"

#include "gimpsheartool.h"
#include "tool_manager.h"
#include "transform_options.h"

#include "libgimp/gimpintl.h"


/*  index into trans_info array  */
#define HORZ_OR_VERT 0
#define XSHEAR       1
#define YSHEAR       2

/*  the minimum movement before direction of shear can be determined (pixels) */
#define MIN_MOVE     5


/*  forward function declarations  */

static void          gimp_shear_tool_class_init (GimpShearToolClass *klass);
static void          gimp_shear_tool_init       (GimpShearTool      *shear_tool);

static TileManager * gimp_shear_tool_transform (GimpTransformTool *tr_tool,
						GimpDisplay       *gdisp,
						TransformState     state);

static void          shear_tool_recalc         (GimpTransformTool *tr_tool,
						GimpDisplay       *gdisp);
static void          shear_tool_motion         (GimpTransformTool *tr_tool,
						GimpDisplay       *gdisp);
static void          shear_info_update         (GimpTransformTool *tr_tool);

static void          shear_x_mag_changed       (GtkWidget         *widget,
						gpointer           data);
static void          shear_y_mag_changed       (GtkWidget         *widget,
						gpointer           data);


/*  variables local to this file  */
static gdouble  xshear_val;
static gdouble  yshear_val;

static GimpTransformToolClass *parent_class = NULL;

static TransformOptions *shear_options = NULL;


/* Public functions */

void 
gimp_shear_tool_register (Gimp *gimp)
{
  tool_manager_register_tool (gimp,
			      GIMP_TYPE_SHEAR_TOOL,
                              FALSE,
			      "gimp:shear_tool",
			      _("Shear Tool"),
			      _("Shear the layer or selection"),
			      N_("/Tools/Transform Tools/Shear"), "<shift>F",
			      NULL, "tools/shear.html",
			      GIMP_STOCK_TOOL_SHEAR);
}

GType
gimp_shear_tool_get_type (void)
{
  static GType tool_type = 0;

  if (! tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpShearToolClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_shear_tool_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpShearTool),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_shear_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_TRANSFORM_TOOL,
					  "GimpShearTool", 
                                          &tool_info, 0);
    }

  return tool_type;
}

static void
gimp_shear_tool_class_init (GimpShearToolClass *klass)
{
  GimpTransformToolClass *trans_class;

  trans_class = GIMP_TRANSFORM_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  trans_class->transform = gimp_shear_tool_transform;
}

static void
gimp_shear_tool_init (GimpShearTool *shear_tool)
{
  GimpTool          *tool;
  GimpTransformTool *transform_tool;

  tool           = GIMP_TOOL (shear_tool);
  transform_tool = GIMP_TRANSFORM_TOOL (shear_tool);

  if (! shear_options)
    {
      shear_options = transform_options_new (GIMP_TYPE_SHEAR_TOOL,
                                             transform_options_reset);

      tool_manager_register_tool_options (GIMP_TYPE_SHEAR_TOOL,
                                          (GimpToolOptions *) shear_options);
    }

  tool->tool_cursor = GIMP_SHEAR_TOOL_CURSOR;
}

static TileManager *
gimp_shear_tool_transform (GimpTransformTool *transform_tool,
			   GimpDisplay       *gdisp,
			   TransformState     state)
{
  switch (state)
    {
    case TRANSFORM_INIT:
      if (! transform_info)
	{
	  transform_info = info_dialog_new (_("Shear Information"),
					    gimp_standard_help_func,
					    "tools/transform_shear.html");

	  info_dialog_add_spinbutton (transform_info,
				      _("Shear Magnitude X:"),
				      &xshear_val,
				      -65536, 65536, 1, 15, 1, 1, 0,
				      G_CALLBACK (shear_x_mag_changed),
				      transform_tool);

	  info_dialog_add_spinbutton (transform_info,
				      _("Y:"),
				      &yshear_val,
				      -65536, 65536, 1, 15, 1, 1, 0,
				      G_CALLBACK (shear_y_mag_changed),
				      transform_tool);
	}

      gtk_widget_set_sensitive (GTK_WIDGET (transform_info->shell), TRUE);

      transform_tool->trans_info[HORZ_OR_VERT] = ORIENTATION_UNKNOWN;
      transform_tool->trans_info[XSHEAR]       = 0.0;
      transform_tool->trans_info[YSHEAR]       = 0.0;
      break;

    case TRANSFORM_MOTION:
      shear_tool_motion (transform_tool, gdisp);
      shear_tool_recalc (transform_tool, gdisp);
      break;

    case TRANSFORM_RECALC:
      shear_tool_recalc (transform_tool, gdisp);
      break;

    case TRANSFORM_FINISH:
      return gimp_transform_tool_transform_tiles (transform_tool,
                                                  _("Shearing..."));
      break;
    }

  return NULL;
}

static void
shear_info_update (GimpTransformTool *transform_tool)
{
  xshear_val = transform_tool->trans_info[XSHEAR];
  yshear_val = transform_tool->trans_info[YSHEAR];

  info_dialog_update (transform_info);
  info_dialog_popup (transform_info);
}

static void
shear_x_mag_changed (GtkWidget *widget,
		     gpointer   data)
{
  GimpTool          *tool;
  GimpTransformTool *transform_tool;
  gint               value;

  tool           = GIMP_TOOL (data);
  transform_tool = GIMP_TRANSFORM_TOOL (data);

  value = GTK_ADJUSTMENT (widget)->value;

  if (value != transform_tool->trans_info[XSHEAR])
    {
      gimp_draw_tool_pause (GIMP_DRAW_TOOL (tool));

      transform_tool->trans_info[XSHEAR] = value;

      shear_tool_recalc (transform_tool, tool->gdisp);

      gimp_draw_tool_resume (GIMP_DRAW_TOOL (tool));
    }
}

static void
shear_y_mag_changed (GtkWidget *widget,
		     gpointer   data)
{
  GimpTool          *tool;
  GimpTransformTool *transform_tool;
  gint               value;

  tool           = GIMP_TOOL (data);
  transform_tool = GIMP_TRANSFORM_TOOL (data);

  value = GTK_ADJUSTMENT (widget)->value;

  if (value != transform_tool->trans_info[YSHEAR])
    {
      gimp_draw_tool_pause (GIMP_DRAW_TOOL (tool));

      transform_tool->trans_info[YSHEAR] = value;

      shear_tool_recalc (transform_tool, tool->gdisp);

      gimp_draw_tool_resume (GIMP_DRAW_TOOL (tool));
    }
}

static void
shear_tool_motion (GimpTransformTool *transform_tool,
		   GimpDisplay       *gdisp)
{
  gint diffx, diffy;
  gint dir;

  diffx = transform_tool->curx - transform_tool->lastx;
  diffy = transform_tool->cury - transform_tool->lasty;

  /*  If we haven't yet decided on which way to control shearing
   *  decide using the maximum differential
   */

  if (transform_tool->trans_info[HORZ_OR_VERT] == ORIENTATION_UNKNOWN)
    {
      if (abs (diffx) > MIN_MOVE || abs (diffy) > MIN_MOVE)
	{
	  if (abs (diffx) > abs (diffy))
	    {
	      transform_tool->trans_info[HORZ_OR_VERT] = ORIENTATION_HORIZONTAL;
	      transform_tool->trans_info[ORIENTATION_VERTICAL] = 0.0;
	    }
	  else
	    {
	      transform_tool->trans_info[HORZ_OR_VERT] = ORIENTATION_VERTICAL;
	      transform_tool->trans_info[ORIENTATION_HORIZONTAL] = 0.0;
	    }
	}
      /*  set the current coords to the last ones  */
      else
	{
	  transform_tool->curx = transform_tool->lastx;
	  transform_tool->cury = transform_tool->lasty;
	}
    }

  /*  if the direction is known, keep track of the magnitude  */
  if (transform_tool->trans_info[HORZ_OR_VERT] != ORIENTATION_UNKNOWN)
    {
      dir = transform_tool->trans_info[HORZ_OR_VERT];
      switch (transform_tool->function)
	{
	case TRANSFORM_HANDLE_1:
	  if (dir == ORIENTATION_HORIZONTAL)
	    transform_tool->trans_info[XSHEAR] -= diffx;
	  else
	    transform_tool->trans_info[YSHEAR] -= diffy;
	  break;
	case TRANSFORM_HANDLE_2:
	  if (dir == ORIENTATION_HORIZONTAL)
	    transform_tool->trans_info[XSHEAR] -= diffx;
	  else
	    transform_tool->trans_info[YSHEAR] += diffy;
	  break;
	case TRANSFORM_HANDLE_3:
	  if (dir == ORIENTATION_HORIZONTAL)
	    transform_tool->trans_info[XSHEAR] += diffx;
	  else
	    transform_tool->trans_info[YSHEAR] -= diffy;
	  break;
	case TRANSFORM_HANDLE_4:
	  if (dir == ORIENTATION_HORIZONTAL)
	    transform_tool->trans_info[XSHEAR] += diffx;
	  else
	    transform_tool->trans_info[YSHEAR] += diffy;
	  break;
	default:
	  break;
	}
    }
}

static void
shear_tool_recalc (GimpTransformTool *transform_tool,
		   GimpDisplay       *gdisp)
{
  gdouble amount;

  if (transform_tool->trans_info[HORZ_OR_VERT] == ORIENTATION_HORIZONTAL)
    amount = transform_tool->trans_info[XSHEAR];
  else
    amount = transform_tool->trans_info[YSHEAR];

  gimp_drawable_transform_matrix_shear (transform_tool->x1,
                                        transform_tool->y1,
                                        transform_tool->x2,
                                        transform_tool->y2,
                                        transform_tool->trans_info[HORZ_OR_VERT],
                                        amount,
                                        transform_tool->transform);

  /*  transform the bounding box  */
  gimp_transform_tool_transform_bounding_box (transform_tool);

  /*  update the information dialog  */
  shear_info_update (transform_tool);
}
