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

#ifndef __TOOLS_TYPES_H__
#define __TOOLS_TYPES_H__


#include "core/core-types.h"

#include "widgets/widgets-types.h"


/*  tools  */

typedef struct _GimpTool            GimpTool;
typedef struct _GimpPaintTool       GimpPaintTool;
typedef struct _GimpDrawTool        GimpDrawTool;
typedef struct _GimpPathTool        GimpPathTool;
typedef struct _GimpTransformTool   GimpTransformTool;

typedef struct _GimpBezierSelectPoint  GimpBezierSelectPoint;
typedef struct _GimpBezierSelectTool   GimpBezierSelectTool;


/*  stuff  */

typedef struct _SelectionOptions    SelectionOptions;


/*  functions  */

typedef void   (* ToolOptionsResetFunc) (GimpToolOptions *tool_options);


/*  enums  */

/*  The possible states for tools  */
typedef enum /*< skip >*/
{
  INACTIVE,
  ACTIVE,
  PAUSED
} ToolState;

/*  Tool control actions  */
typedef enum /*< skip >*/
{
  PAUSE,
  RESUME,
  HALT,
  DESTROY,
  RECREATE
} ToolAction;

/* possible transform functions */
typedef enum /*< skip >*/
{
  TRANSFORM_CREATING,
  TRANSFORM_HANDLE_1,
  TRANSFORM_HANDLE_2,
  TRANSFORM_HANDLE_3,
  TRANSFORM_HANDLE_4,
  TRANSFORM_HANDLE_CENTER
} TransformAction;

/* the different states that the transformation function can be called with */
typedef enum /*< skip >*/
{
  TRANSFORM_INIT,
  TRANSFORM_MOTION,
  TRANSFORM_RECALC,
  TRANSFORM_FINISH
} TransformState;


#endif /* __TOOLS_TYPES_H__ */
