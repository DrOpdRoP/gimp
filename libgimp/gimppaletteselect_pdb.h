/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimppaletteselect_pdb.h
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

#ifndef __GIMP_PALETTE_SELECT_PDB_H__
#define __GIMP_PALETTE_SELECT_PDB_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


gboolean gimp_palettes_popup       (gchar *palette_callback,
				    gchar *popup_title,
				    gchar *initial_palette);
gboolean gimp_palettes_close_popup (gchar *palette_callback);
gboolean gimp_palettes_set_popup   (gchar *palette_callback,
				    gchar *palette_name);


G_END_DECLS

#endif /* __GIMP_PALETTE_SELECT_PDB_H__ */
