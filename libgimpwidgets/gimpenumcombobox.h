/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpenumcombobox.h
 * Copyright (C) 2004  Sven Neumann <sven@gimp.org>
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

#ifndef __GIMP_ENUM_COMBO_BOX_H__
#define __GIMP_ENUM_COMBO_BOX_H__

#include <gtk/gtkcombobox.h>


#define GIMP_TYPE_ENUM_COMBO_BOX            (gimp_enum_combo_box_get_type ())
#define GIMP_ENUM_COMBO_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_ENUM_COMBO_BOX, GimpEnumComboBox))
#define GIMP_ENUM_COMBO_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_ENUM_COMBO_BOX, GimpEnumComboBoxClass))
#define GIMP_IS_ENUM_COMBO_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_ENUM_COMBO_BOX))
#define GIMP_IS_ENUM_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_ENUM_COMBO_BOX))
#define GIMP_ENUM_COMBO_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_ENUM_COMBO_BOX, GimpEnumComboBoxClass))


typedef struct _GimpEnumComboBoxClass  GimpEnumComboBoxClass;

struct _GimpEnumComboBoxClass
{
  GtkComboBoxClass  parent_instance;
};

struct _GimpEnumComboBox
{
  GtkComboBox       parent_instance;
};


GType       gimp_enum_combo_box_get_type         (void) G_GNUC_CONST;

GtkWidget * gimp_enum_combo_box_new              (GType             enum_type);
GtkWidget * gimp_enum_combo_box_new_with_model   (GimpEnumStore    *enum_store);

gboolean    gimp_enum_combo_box_set_active       (GimpEnumComboBox *combo_box,
                                                  gint              value);
gboolean    gimp_enum_combo_box_get_active       (GimpEnumComboBox *combo_box,
                                                  gint             *value);
void        gimp_enum_combo_box_set_stock_prefix (GimpEnumComboBox *combo_box,
                                                  const gchar      *stock_prefix);


#endif  /* __GIMP_ENUM_COMBO_BOX_H__ */
