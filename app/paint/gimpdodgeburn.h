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

#ifndef __GIMP_DODGE_BURN_TOOL_H__
#define __GIMP_DODGE_BURN_TOOL_H__

#include "gimppainttool.h"

#define GIMP_TYPE_DODGEBURN_TOOL            (gimp_dodgeburn_tool_get_type ())
#define GIMP_DODGEBURN_TOOL(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_DODGEBURN_TOOL, GimpDodgeBurnTool))
#define GIMP_IS_DODGEBURN_TOOL(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_DODGEBURN_TOOL))
#define GIMP_DODGEBURN_TOOL_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_DODGEBURN_TOOL, GimpDodgeBurnToolClass))
#define GIMP_IS_DODGEBURN_TOOL_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_DODGEBURN_TOOL))


typedef enum
{
      DODGE,
        BURN
} DodgeBurnType;

typedef enum
{
      DODGEBURN_HIGHLIGHTS,
        DODGEBURN_MIDTONES,
	  DODGEBURN_SHADOWS
} DodgeBurnMode;


typedef struct _GimpDodgeBurnTool      GimpDodgeBurnTool;
typedef struct _GimpDodgeBurnToolClass GimpDodgeBurnToolClass;

struct _GimpDodgeBurnTool
{
  GimpPaintTool parent_instance;
};

struct _GimpDodgeBurnToolClass
{
  GimpPaintToolClass parent_class;
};


void       gimp_dodgeburn_tool_register (void);

GtkType    gimp_dodgeburn_tool_get_type (void);

/* FIXME: Get rid of this non_gui stuff someday.  Preferably make
 *        everything use it interally for ease of macro recording.
 */



gboolean dodgeburn_non_gui (GimpDrawable  *drawable,
                            gdouble        exposure,
		            DodgeBurnType  type, 
		            DodgeBurnMode  mode, 
		            gint           num_strokes, 
		            gdouble       *stroke_array);

gboolean dodgeburn_non_gui_default (GimpDrawable  *drawable,
                                    gint           num_strokes,
				    gdouble       *stroke_array);


#endif  /*  __GIMP_DODGEBURN_TOOL_H__  */
