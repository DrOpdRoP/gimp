/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GIMP_MEMSIZE_H__
#define __GIMP_MEMSIZE_H__


#define GIMP_TYPE_MEMSIZE               (gimp_memsize_get_type ())
#define GIMP_VALUE_HOLDS_MEMSIZE(value) (G_TYPE_CHECK_VALUE_TYPE ((value), GIMP_TYPE_MEMSIZE))

GType      gimp_memsize_get_type         (void) G_GNUC_CONST;

gchar    * gimp_memsize_serialize        (guint64      memsize);
gboolean   gimp_memsize_deserialize      (const gchar *string,
                                          guint64     *memsize);

gchar    * gimp_memsize_to_string        (guint64      memsize);


#endif  /* __GIMP_MEMSIZE_H__ */
