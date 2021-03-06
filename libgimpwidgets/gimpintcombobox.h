/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimpintcombobox.h
 * Copyright (C) 2004  Sven Neumann <sven@gimp.org>
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

#ifndef __GIMP_INT_COMBO_BOX_H__
#define __GIMP_INT_COMBO_BOX_H__

#include <gtk/gtkcombobox.h>

G_BEGIN_DECLS


#define GIMP_TYPE_INT_COMBO_BOX            (gimp_int_combo_box_get_type ())
#define GIMP_INT_COMBO_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_INT_COMBO_BOX, GimpIntComboBox))
#define GIMP_INT_COMBO_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_INT_COMBO_BOX, GimpIntComboBoxClass))
#define GIMP_IS_INT_COMBO_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_INT_COMBO_BOX))
#define GIMP_IS_INT_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_INT_COMBO_BOX))
#define GIMP_INT_COMBO_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_INT_COMBO_BOX, GimpIntComboBoxClass))


typedef struct _GimpIntComboBoxClass  GimpIntComboBoxClass;

struct _GimpIntComboBox
{
  GtkComboBox       parent_instance;

  /*< private >*/
  gpointer          priv;

  /* Padding for future expansion (should have gone to the class) */
  void (* _gimp_reserved2) (void);
  void (* _gimp_reserved3) (void);
  void (* _gimp_reserved4) (void);
};

struct _GimpIntComboBoxClass
{
  GtkComboBoxClass  parent_class;
};


typedef  gboolean (* GimpIntSensitivityFunc) (gint      value,
                                              gpointer  data);



GType       gimp_int_combo_box_get_type        (void) G_GNUC_CONST;

GtkWidget * gimp_int_combo_box_new             (const gchar     *first_label,
                                                gint             first_value,
                                                ...) G_GNUC_NULL_TERMINATED;
GtkWidget * gimp_int_combo_box_new_valist      (const gchar     *first_label,
                                                gint             first_value,
                                                va_list          values);

GtkWidget * gimp_int_combo_box_new_array       (gint             n_values,
                                                const gchar     *labels[]);

void        gimp_int_combo_box_prepend         (GimpIntComboBox *combo_box,
                                                ...);
void        gimp_int_combo_box_append          (GimpIntComboBox *combo_box,
                                                ...);

gboolean    gimp_int_combo_box_set_active      (GimpIntComboBox *combo_box,
                                                gint             value);
gboolean    gimp_int_combo_box_get_active      (GimpIntComboBox *combo_box,
                                                gint            *value);

gulong      gimp_int_combo_box_connect         (GimpIntComboBox *combo_box,
                                                gint             value,
                                                GCallback        callback,
                                                gpointer         data);

void        gimp_int_combo_box_set_sensitivity (GimpIntComboBox        *combo_box,
                                                GimpIntSensitivityFunc  func,
                                                gpointer                data,
                                                GDestroyNotify          destroy);


G_END_DECLS

#endif  /* __GIMP_INT_COMBO_BOX_H__ */
