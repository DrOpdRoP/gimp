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

#include <gtk/gtk.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"

#include "config/gimpconfig-params.h"
#include "config/gimpdisplayconfig.h"

#include "core/gimp.h"
#include "core/gimptoolinfo.h"

#include "widgets/gimppropwidgets.h"
#include "widgets/gimpwidgets-utils.h"

#include "gimpmagnifyoptions.h"
#include "gimptooloptions-gui.h"

#include "gimp-intl.h"


enum
{
  PROP_0,
  PROP_AUTO_RESIZE,
  PROP_ZOOM_TYPE,
  PROP_THRESHOLD
};


static void   gimp_magnify_options_init       (GimpMagnifyOptions      *options);
static void   gimp_magnify_options_class_init (GimpMagnifyOptionsClass *options_class);

static void   gimp_magnify_options_set_property (GObject         *object,
                                                 guint            property_id,
                                                 const GValue    *value,
                                                 GParamSpec      *pspec);
static void   gimp_magnify_options_get_property (GObject         *object,
                                                 guint            property_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);

static void   gimp_magnify_options_reset        (GimpToolOptions *tool_options);
static void   gimp_magnify_options_set_defaults (GimpToolOptions *tool_options);


static GimpToolOptionsClass *parent_class = NULL;


GType
gimp_magnify_options_get_type (void)
{
  static GType type = 0;

  if (! type)
    {
      static const GTypeInfo info =
      {
        sizeof (GimpMagnifyOptionsClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_magnify_options_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpMagnifyOptions),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_magnify_options_init,
      };

      type = g_type_register_static (GIMP_TYPE_TOOL_OPTIONS,
                                     "GimpMagnifyOptions",
                                     &info, 0);
    }

  return type;
}

static void
gimp_magnify_options_class_init (GimpMagnifyOptionsClass *klass)
{
  GObjectClass         *object_class;
  GimpToolOptionsClass *options_class;

  object_class  = G_OBJECT_CLASS (klass);
  options_class = GIMP_TOOL_OPTIONS_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->set_property = gimp_magnify_options_set_property;
  object_class->get_property = gimp_magnify_options_get_property;

  options_class->reset       = gimp_magnify_options_reset;

  GIMP_CONFIG_INSTALL_PROP_BOOLEAN (object_class, PROP_AUTO_RESIZE,
                                    "auto-resize", NULL,
                                    FALSE,
                                    0);
  GIMP_CONFIG_INSTALL_PROP_ENUM (object_class, PROP_ZOOM_TYPE,
                                 "zoom-type", NULL,
                                 GIMP_TYPE_ZOOM_TYPE,
                                 GIMP_ZOOM_IN,
                                 0);
  GIMP_CONFIG_INSTALL_PROP_DOUBLE (object_class, PROP_THRESHOLD,
                                   "threshold", NULL,
                                   1.0, 15.0, 5.0,
                                   0);
}

static void
gimp_magnify_options_init (GimpMagnifyOptions *options)
{
}

static void
gimp_magnify_options_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GimpMagnifyOptions *options;

  options = GIMP_MAGNIFY_OPTIONS (object);

  switch (property_id)
    {
    case PROP_AUTO_RESIZE:
      options->auto_resize = g_value_get_boolean (value);
      break;
    case PROP_ZOOM_TYPE:
      options->zoom_type = g_value_get_enum (value);
      break;
    case PROP_THRESHOLD:
      options->threshold = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_magnify_options_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  GimpMagnifyOptions *options;

  options = GIMP_MAGNIFY_OPTIONS (object);

  switch (property_id)
    {
    case PROP_AUTO_RESIZE:
      g_value_set_boolean (value, options->auto_resize);
      break;
    case PROP_ZOOM_TYPE:
      g_value_set_enum (value, options->zoom_type);
      break;
    case PROP_THRESHOLD:
      g_value_set_double (value, options->threshold);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_magnify_options_reset (GimpToolOptions *tool_options)
{
  gimp_magnify_options_set_defaults (tool_options);

  GIMP_TOOL_OPTIONS_CLASS (parent_class)->reset (tool_options);
}

static void
gimp_magnify_options_set_defaults (GimpToolOptions *tool_options)
{
  GParamSpec *pspec;

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (tool_options),
                                        "auto-resize");

  if (pspec)
    G_PARAM_SPEC_BOOLEAN (pspec)->default_value =
      GIMP_DISPLAY_CONFIG (tool_options->tool_info->gimp->config)->resize_windows_on_zoom;
}

GtkWidget *
gimp_magnify_options_gui (GimpToolOptions *tool_options)
{
  GObject   *config;
  GtkWidget *vbox;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *button;
  gchar     *str;

  config = G_OBJECT (tool_options);

  vbox = gimp_tool_options_gui (tool_options);

  /*  the auto_resize toggle button  */
  button = gimp_prop_check_button_new (config, "auto-resize",
                                       _("Auto-Resize Window"));
  gtk_box_pack_start (GTK_BOX (vbox),  button, FALSE, FALSE, 0);
  gtk_widget_show (button);

  /*  tool toggle  */
  str = g_strdup_printf (_("Tool Toggle  %s"), gimp_get_mod_name_control ());

  frame = gimp_prop_enum_radio_frame_new (config, "zoom-type",
                                          str, 0, 0);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  g_free (str);

  /*  window threshold */
  table = gtk_table_new (1, 3, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 2);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);
  gtk_widget_show (table);

  gimp_prop_scale_entry_new (config, "threshold",
                             GTK_TABLE (table), 0, 0,
                             _("Threshold:"),
                             1.0, 3.0, 1,
                             FALSE, 0.0, 0.0);

  gimp_magnify_options_set_defaults (tool_options);

  return vbox;
}
