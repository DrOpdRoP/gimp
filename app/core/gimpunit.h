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

#ifndef __APP_GIMP_UNIT_H__
#define __APP_GIMP_UNIT_H__


void       gimp_units_init                         (Gimp     *gimp);
void       gimp_units_exit                         (Gimp     *gimp);

void       gimp_unitrc_load                        (Gimp     *gimp);
void       gimp_unitrc_save                        (Gimp     *gimp);

gint       _gimp_unit_get_number_of_units          (Gimp     *gimp);
gint       _gimp_unit_get_number_of_built_in_units (Gimp     *gimp);

GimpUnit   _gimp_unit_new                          (Gimp     *gimp,
						    gchar    *identifier,
						    gdouble   factor,
						    gint      digits,
						    gchar    *symbol,
						    gchar    *abbreviation,
						    gchar    *singular,
						    gchar    *plural);

gboolean   _gimp_unit_get_deletion_flag            (Gimp     *gimp,
						    GimpUnit  unit);
void       _gimp_unit_set_deletion_flag            (Gimp     *gimp,
						    GimpUnit  unit,
						    gboolean  deletion_flag);

gdouble    _gimp_unit_get_factor                   (Gimp     *gimp,
						    GimpUnit  unit);

gint       _gimp_unit_get_digits                   (Gimp     *gimp,
						    GimpUnit  unit);

gchar    * _gimp_unit_get_identifier               (Gimp     *gimp,
						    GimpUnit  unit);

gchar    * _gimp_unit_get_symbol                   (Gimp     *gimp,
						    GimpUnit  unit);
gchar    * _gimp_unit_get_abbreviation             (Gimp     *gimp,
						    GimpUnit  unit);
gchar    * _gimp_unit_get_singular                 (Gimp     *gimp,
						    GimpUnit  unit);
gchar    * _gimp_unit_get_plural                   (Gimp     *gimp,
						    GimpUnit  unit);


#endif  /*  __APP_GIMP_UNIT_H__  */
