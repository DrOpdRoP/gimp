/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimpcontroller.c
 * Copyright (C) 2004 Michael Natterer <mitch@gimp.org>
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

#include "config.h"

#include <gtk/gtk.h>

#include "libgimpcolor/gimpcolor.h"

#include "gimpwidgetstypes.h"

#include "gimpcontroller.h"
#include "gimpwidgetsmarshal.h"


enum
{
  PROP_0,
  PROP_NAME,
  PROP_ENABLED
};

enum
{
  EVENT,
  LAST_SIGNAL
};


static void   gimp_controller_class_init   (GimpControllerClass *klass);
static void   gimp_controller_set_property (GObject             *object,
                                            guint                property_id,
                                            const GValue        *value,
                                            GParamSpec          *pspec);
static void   gimp_controller_get_property (GObject             *object,
                                            guint                property_id,
                                            GValue              *value,
                                            GParamSpec          *pspec);


static GObjectClass *parent_class = NULL;

static guint  controller_signals[LAST_SIGNAL] = { 0 };


GType
gimp_controller_get_type (void)
{
  static GType controller_type = 0;

  if (! controller_type)
    {
      static const GTypeInfo controller_info =
      {
        sizeof (GimpControllerClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gimp_controller_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data     */
        sizeof (GimpController),
        0,              /* n_preallocs    */
        NULL            /* instance_init  */
      };

      controller_type = g_type_register_static (G_TYPE_OBJECT,
                                                "GimpController",
                                                &controller_info, 0);
    }

  return controller_type;
}

gboolean
gimp_controller_boolean_handled_accumulator (GSignalInvocationHint *ihint,
                                             GValue                *return_accu,
                                             const GValue          *handler_return,
                                             gpointer               dummy)
{
  gboolean continue_emission;
  gboolean signal_handled;

  signal_handled = g_value_get_boolean (handler_return);
  g_value_set_boolean (return_accu, signal_handled);
  continue_emission = ! signal_handled;

  return continue_emission;
}

static void
gimp_controller_class_init (GimpControllerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->set_property = gimp_controller_set_property;
  object_class->get_property = gimp_controller_get_property;

  g_object_class_install_property (object_class, PROP_NAME,
                                   g_param_spec_string ("name", NULL, NULL,
                                                        "Unnamed Controller",
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_CONSTRUCT));
  g_object_class_install_property (object_class, PROP_ENABLED,
                                   g_param_spec_boolean ("enabled", NULL, NULL,
                                                         TRUE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_CONSTRUCT));

  controller_signals[EVENT] =
    g_signal_new ("event",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GimpControllerClass, event),
                  gimp_controller_boolean_handled_accumulator, NULL,
                  _gimp_widgets_marshal_BOOLEAN__POINTER,
                  G_TYPE_BOOLEAN, 1,
                  G_TYPE_POINTER);

  klass->name           = "Unnamed";
  klass->help_id        = NULL;

  klass->get_n_events   = NULL;
  klass->get_event_name = NULL;
  klass->event          = NULL;
}

static void
gimp_controller_set_property (GObject      *object,
                              guint         property_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GimpController *controller = GIMP_CONTROLLER (object);

  switch (property_id)
    {
    case PROP_NAME:
      if (controller->name)
        g_free (controller->name);
      controller->name = g_value_dup_string (value);
      break;
    case PROP_ENABLED:
      controller->enabled = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_controller_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GimpController *controller = GIMP_CONTROLLER (object);

  switch (property_id)
    {
    case PROP_NAME:
      g_value_set_string (value, controller->name);
      break;
    case PROP_ENABLED:
      g_value_set_boolean (value, controller->enabled);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

GimpController *
gimp_controller_new (GType controller_type)
{
  GimpController *controller;

  g_return_val_if_fail (g_type_is_a (controller_type, GIMP_TYPE_CONTROLLER),
                        NULL);

  controller = g_object_new (controller_type, NULL);

  return controller;
}

gint
gimp_controller_get_n_events (GimpController *controller)
{
  g_return_val_if_fail (GIMP_IS_CONTROLLER (controller), 0);

  if (GIMP_CONTROLLER_GET_CLASS (controller)->get_n_events)
    return GIMP_CONTROLLER_GET_CLASS (controller)->get_n_events (controller);

  return 0;
}

const gchar *
gimp_controller_get_event_name (GimpController *controller,
                                gint            event_id)
{
  g_return_val_if_fail (GIMP_IS_CONTROLLER (controller), NULL);

  if (GIMP_CONTROLLER_GET_CLASS (controller)->get_event_name)
    return GIMP_CONTROLLER_GET_CLASS (controller)->get_event_name (controller,
                                                                   event_id);

  return NULL;
}

const gchar *
gimp_controller_get_event_blurb (GimpController *controller,
                                 gint            event_id)
{
  g_return_val_if_fail (GIMP_IS_CONTROLLER (controller), NULL);

  if (GIMP_CONTROLLER_GET_CLASS (controller)->get_event_blurb)
    return GIMP_CONTROLLER_GET_CLASS (controller)->get_event_blurb (controller,
                                                                    event_id);

  return NULL;
}

void
gimp_controller_set_enabled (GimpController *controller,
                             gboolean        enabled)
{
  g_return_if_fail (GIMP_IS_CONTROLLER (controller));

  if (enabled != controller->enabled)
    {
      g_object_set (controller,
                    "enabled", enabled,
                    NULL);
    }
}

gboolean
gimp_controller_get_enabled (GimpController *controller)
{
  g_return_val_if_fail (GIMP_IS_CONTROLLER (controller), FALSE);

  return controller->enabled;
}

gboolean
gimp_controller_event (GimpController            *controller,
                       const GimpControllerEvent *event)
{
  gboolean retval = FALSE;

  g_return_val_if_fail (GIMP_IS_CONTROLLER (controller), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (controller->enabled)
    g_signal_emit (controller, controller_signals[EVENT], 0,
                   event, &retval);

  return retval;
}
