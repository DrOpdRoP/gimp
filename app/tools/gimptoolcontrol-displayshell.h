/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2002 Spencer Kimball, Peter Mattis and others
 *
 * gimptoolcontrol-displayshell.c: toolcontrol functions exclusively for
 * the DisplayShell
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

#ifndef __GIMP_TOOL_CONTROL_DISPLAYSHELL_H__
#define __GIMP_TOOL_CONTROL_DISPLAYSHELL_H__

gboolean gimp_tool_control_wants_perfect_mouse        (GimpToolControl   *control);
gboolean gimp_tool_control_handles_empty_image        (GimpToolControl   *control);
gboolean gimp_tool_control_auto_snap_to               (GimpToolControl   *control);
gboolean gimp_tool_control_preserve                   (GimpToolControl   *control);
gboolean gimp_tool_control_scroll_lock                (GimpToolControl   *control);
GimpToolCursorType gimp_tool_control_get_tool_cursor  (GimpToolControl   *control);
  

#endif /* __GIMP_TOOL_CONTROL_DISPLAYSHELL_H__ */

