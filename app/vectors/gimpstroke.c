/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpstroke.c
 * Copyright (C) 2002 Simon Budig  <simon@gimp.org>
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

#include <glib-object.h>

#include "vectors-types.h"

#include "core/gimp-utils.h"
#include "core/gimpcoords.h"

#include "gimpanchor.h"
#include "gimpstroke.h"

enum
{
  PROP_0,
  PROP_CONTROL_POINTS,
  PROP_CLOSED
};

/* Prototypes */

static void    gimp_stroke_class_init                (GimpStrokeClass  *klass);
static void    gimp_stroke_init                      (GimpStroke       *stroke);
static void    gimp_stroke_set_property              (GObject      *object,
                                                      guint         property_id,
                                                      const GValue *value,
                                                      GParamSpec   *pspec);
static void    gimp_stroke_get_property              (GObject      *object,
                                                      guint         property_id,
                                                      GValue       *value,
                                                      GParamSpec   *pspec);
static void    gimp_stroke_finalize                  (GObject      *object);

static gint64  gimp_stroke_get_memsize               (GimpObject   *object,
                                                      gint64       *gui_size);

static gdouble gimp_stroke_real_nearest_point_get    (const GimpStroke *stroke,
                                                      const GimpCoords *coord,
                                                      const gdouble     precision,
                                                      GimpCoords       *ret_point,
                                                      GimpAnchor      **ret_segment_start,
                                                      GimpAnchor      **ret_segment_end,
                                                      gdouble          *ret_pos);
static GimpAnchor * gimp_stroke_real_anchor_get      (const GimpStroke *stroke,
                                                      const GimpCoords *coord);
static GimpAnchor * gimp_stroke_real_anchor_get_next (const GimpStroke *stroke,
                                                      const GimpAnchor *prev);
static void         gimp_stroke_real_anchor_select   (GimpStroke       *stroke,
                                                      GimpAnchor       *anchor,
                                                      gboolean          selected,
                                                      gboolean          exclusive);
static void    gimp_stroke_real_anchor_move_relative (GimpStroke       *stroke,
                                                      GimpAnchor       *anchor,
                                                      const GimpCoords *delta,
                                                      GimpAnchorFeatureType feature);
static void    gimp_stroke_real_anchor_move_absolute (GimpStroke       *stroke,
                                                      GimpAnchor       *anchor,
                                                      const GimpCoords *delta,
                                                      GimpAnchorFeatureType feature);
static void         gimp_stroke_real_anchor_convert  (GimpStroke       *stroke,
                                                      GimpAnchor       *anchor,
                                                      GimpAnchorFeatureType  feature);
static void         gimp_stroke_real_anchor_delete   (GimpStroke       *stroke,
                                                      GimpAnchor       *anchor);
static gboolean     gimp_stroke_real_point_is_movable
                                              (GimpStroke            *stroke,
                                               GimpAnchor            *predec,
                                               gdouble                position);
static void         gimp_stroke_real_point_move_relative
                                              (GimpStroke            *stroke,
                                               GimpAnchor            *predec,
                                               gdouble                position,
                                               const GimpCoords      *deltacoord,
                                               GimpAnchorFeatureType  feature);
static void         gimp_stroke_real_point_move_absolute
                                              (GimpStroke            *stroke,
                                               GimpAnchor            *predec,
                                               gdouble                position,
                                               const GimpCoords      *coord,
                                               GimpAnchorFeatureType  feature);

static void         gimp_stroke_real_close           (GimpStroke       *stroke);
static GimpStroke * gimp_stroke_real_open            (GimpStroke       *stroke,
                                                      GimpAnchor       *end_anchor);
static gboolean     gimp_stroke_real_anchor_is_insertable
                                                     (GimpStroke       *stroke,
                                                      GimpAnchor       *predec,
                                                      gdouble           position);
static GimpAnchor * gimp_stroke_real_anchor_insert   (GimpStroke       *stroke,
                                                      GimpAnchor       *predec,
                                                      gdouble           position);

static gboolean     gimp_stroke_real_is_extendable   (GimpStroke       *stroke,
                                                      GimpAnchor       *neighbor);

static GimpAnchor * gimp_stroke_real_extend (GimpStroke           *stroke,
                                             const GimpCoords     *coords,
                                             GimpAnchor           *neighbor,
                                             GimpVectorExtendMode  extend_mode);

gboolean     gimp_stroke_real_connect_stroke (GimpStroke          *stroke,
                                              GimpAnchor          *anchor,
                                              GimpStroke          *extension,
                                              GimpAnchor          *neighbor);


static gboolean     gimp_stroke_real_is_empty        (const GimpStroke *stroke);

static gdouble      gimp_stroke_real_get_length      (const GimpStroke *stroke,
                                                      const gdouble     precision);
static gdouble      gimp_stroke_real_get_distance    (const GimpStroke *stroke,
                                                      const GimpCoords *coord);
static GArray *     gimp_stroke_real_interpolate     (const GimpStroke *stroke,
                                                      gdouble           precision,
                                                      gboolean         *closed);
static GimpStroke * gimp_stroke_real_duplicate       (const GimpStroke *stroke);
static GimpStroke * gimp_stroke_real_make_bezier     (const GimpStroke *stroke);

static void         gimp_stroke_real_translate       (GimpStroke       *stroke,
                                                      gdouble           offset_x,
                                                      gdouble           offset_y);
static void         gimp_stroke_real_scale           (GimpStroke       *stroke,
                                                      gdouble           scale_x,
                                                      gdouble           scale_y);
static void         gimp_stroke_real_transform       (GimpStroke        *stroke,
                                                      const GimpMatrix3 *matrix);

static GList    * gimp_stroke_real_get_draw_anchors  (const GimpStroke *stroke);
static GList    * gimp_stroke_real_get_draw_controls (const GimpStroke *stroke);
static GArray   * gimp_stroke_real_get_draw_lines    (const GimpStroke *stroke);
static GArray *  gimp_stroke_real_control_points_get (const GimpStroke *stroke,
                                                      gboolean         *ret_closed);
static gboolean   gimp_stroke_real_get_point_at_dist (const GimpStroke *stroke,
                                                      const gdouble     dist,
                                                      const gdouble     precision,
                                                      GimpCoords       *position,
                                                      gdouble          *gradient);

/*  private variables  */

static GimpObjectClass *parent_class = NULL;


GType
gimp_stroke_get_type (void)
{
  static GType stroke_type = 0;

  if (! stroke_type)
    {
      static const GTypeInfo stroke_info =
      {
        sizeof (GimpStrokeClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gimp_stroke_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data     */
        sizeof (GimpStroke),
        0,              /* n_preallocs    */
        (GInstanceInitFunc) gimp_stroke_init,
      };

      stroke_type = g_type_register_static (GIMP_TYPE_OBJECT,
                                            "GimpStroke",
                                            &stroke_info, 0);
    }

  return stroke_type;
}

static void
gimp_stroke_class_init (GimpStrokeClass *klass)
{
  GObjectClass    *object_class;
  GimpObjectClass *gimp_object_class;
  GParamSpec      *anchor_param_spec;
  GParamSpec      *control_points_param_spec;
  GParamSpec      *close_param_spec;

  object_class      = G_OBJECT_CLASS (klass);
  gimp_object_class = GIMP_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize         = gimp_stroke_finalize;
  object_class->get_property     = gimp_stroke_get_property;
  object_class->set_property     = gimp_stroke_set_property;

  gimp_object_class->get_memsize = gimp_stroke_get_memsize;

  klass->changed                 = NULL;
  klass->removed                 = NULL;

  klass->anchor_get              = gimp_stroke_real_anchor_get;
  klass->anchor_get_next         = gimp_stroke_real_anchor_get_next;
  klass->anchor_select           = gimp_stroke_real_anchor_select;
  klass->anchor_move_relative    = gimp_stroke_real_anchor_move_relative;
  klass->anchor_move_absolute    = gimp_stroke_real_anchor_move_absolute;
  klass->anchor_convert          = gimp_stroke_real_anchor_convert;
  klass->anchor_delete           = gimp_stroke_real_anchor_delete;

  klass->point_is_movable        = gimp_stroke_real_point_is_movable;
  klass->point_move_relative     = gimp_stroke_real_point_move_relative;
  klass->point_move_absolute     = gimp_stroke_real_point_move_absolute;

  klass->nearest_point_get       = gimp_stroke_real_nearest_point_get;
  klass->close                   = gimp_stroke_real_close;
  klass->open                    = gimp_stroke_real_open;
  klass->anchor_is_insertable    = gimp_stroke_real_anchor_is_insertable;
  klass->anchor_insert           = gimp_stroke_real_anchor_insert;
  klass->is_extendable           = gimp_stroke_real_is_extendable;
  klass->extend                  = gimp_stroke_real_extend;
  klass->connect_stroke          = gimp_stroke_real_connect_stroke;

  klass->is_empty                = gimp_stroke_real_is_empty;
  klass->get_length              = gimp_stroke_real_get_length;
  klass->get_distance            = gimp_stroke_real_get_distance;
  klass->get_point_at_dist       = gimp_stroke_real_get_point_at_dist;
  klass->interpolate             = gimp_stroke_real_interpolate;

  klass->duplicate               = gimp_stroke_real_duplicate;
  klass->make_bezier             = gimp_stroke_real_make_bezier;

  klass->translate               = gimp_stroke_real_translate;
  klass->scale                   = gimp_stroke_real_scale;
  klass->transform               = gimp_stroke_real_transform;

  klass->get_draw_anchors        = gimp_stroke_real_get_draw_anchors;
  klass->get_draw_controls       = gimp_stroke_real_get_draw_controls;
  klass->get_draw_lines          = gimp_stroke_real_get_draw_lines;
  klass->control_points_get      = gimp_stroke_real_control_points_get;

  anchor_param_spec = g_param_spec_boxed ("gimp-anchor",
                                          "Gimp Anchor",
                                          "The control points of a Stroke",
                                          GIMP_TYPE_ANCHOR,
                                          G_PARAM_WRITABLE |
                                          G_PARAM_CONSTRUCT_ONLY);

  control_points_param_spec = g_param_spec_value_array ("control-points",
                                                        "Control Points",
                                                        "This is an ValueArray "
                                                        "with the initial "
                                                        "control points of "
                                                        "the new Stroke",
                                                        anchor_param_spec,
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT_ONLY);

  close_param_spec = g_param_spec_boolean ("closed",
                                           "Close Flag",
                                           "this flag indicates, if the "
                                           "stroke is closed or not",
                                           FALSE,
                                           G_PARAM_READWRITE |
                                           G_PARAM_CONSTRUCT_ONLY);

  g_object_class_install_property (object_class,
                                   PROP_CONTROL_POINTS,
                                   control_points_param_spec);

  g_object_class_install_property (object_class,
                                   PROP_CLOSED,
                                   close_param_spec);
}

static void
gimp_stroke_init (GimpStroke *stroke)
{
  stroke->anchors = NULL;
  stroke->closed  = FALSE;
}

static void
gimp_stroke_set_property (GObject      *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GimpStroke  *stroke = GIMP_STROKE (object);
  GValueArray *val_array;
  GValue      *item;
  gint         i;

  switch (property_id)
    {
    case PROP_CLOSED:
      stroke->closed = g_value_get_boolean (value);
      break;
    case PROP_CONTROL_POINTS:
      g_return_if_fail (stroke->anchors == NULL);
      g_return_if_fail (value != NULL);

      val_array = g_value_get_boxed (value);

      if (val_array == NULL)
        return;

      for (i = 0; i < val_array->n_values; i++)
        {
          item = g_value_array_get_nth (val_array, i);

          g_return_if_fail (G_VALUE_HOLDS (item, GIMP_TYPE_ANCHOR));
          stroke->anchors = g_list_append (stroke->anchors,
                                           g_value_dup_boxed (item));
        }

      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_stroke_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  GimpStroke *stroke = GIMP_STROKE (object);

  switch (property_id)
    {
    case PROP_CLOSED:
      g_value_set_boolean (value, stroke->closed);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_stroke_finalize (GObject *object)
{
  GimpStroke *stroke = GIMP_STROKE (object);

  if (stroke->anchors)
    {
      g_list_foreach (stroke->anchors, (GFunc) gimp_anchor_free, NULL);
      g_list_free (stroke->anchors);
      stroke->anchors = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gint64
gimp_stroke_get_memsize (GimpObject *object,
                         gint64     *gui_size)
{
  GimpStroke *stroke  = GIMP_STROKE (object);
  gint64      memsize = 0;

  memsize += gimp_g_list_get_memsize (stroke->anchors, sizeof (GimpAnchor));

  return memsize + GIMP_OBJECT_CLASS (parent_class)->get_memsize (object,
                                                                  gui_size);
}

GimpAnchor *
gimp_stroke_anchor_get (const GimpStroke *stroke,
                        const GimpCoords *coord)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->anchor_get (stroke, coord);
}


gdouble
gimp_stroke_nearest_point_get (const GimpStroke *stroke,
                               const GimpCoords *coord,
                               const gdouble     precision,
                               GimpCoords       *ret_point,
                               GimpAnchor      **ret_segment_start,
                               GimpAnchor      **ret_segment_end,
                               gdouble          *ret_pos)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);
  g_return_val_if_fail (coord != NULL, FALSE);

  return GIMP_STROKE_GET_CLASS (stroke)->nearest_point_get (stroke,
                                                            coord,
                                                            precision,
                                                            ret_point,
                                                            ret_segment_start,
                                                            ret_segment_end,
                                                            ret_pos);
}

static gdouble
gimp_stroke_real_nearest_point_get (const GimpStroke *stroke,
                                    const GimpCoords *coord,
                                    const gdouble     precision,
                                    GimpCoords       *ret_point,
                                    GimpAnchor      **ret_segment_start,
                                    GimpAnchor      **ret_segment_end,
                                    gdouble          *ret_pos)
{
  g_printerr ("gimp_stroke_nearest_point_get: default implementation\n");
  return -1;
}

static GimpAnchor *
gimp_stroke_real_anchor_get (const GimpStroke *stroke,
                             const GimpCoords *coord)
{
  gdouble     dx, dy;
  gdouble     mindist = -1;
  GList      *anchors;
  GList      *list;
  GimpAnchor *anchor = NULL;

  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  anchors = gimp_stroke_get_draw_controls (stroke);

  for (list = anchors; list; list = g_list_next (list))
    {
      dx = coord->x - GIMP_ANCHOR (list->data)->position.x;
      dy = coord->y - GIMP_ANCHOR (list->data)->position.y;

      if (mindist < 0 || mindist > dx * dx + dy * dy)
        {
          mindist = dx * dx + dy * dy;
          anchor = GIMP_ANCHOR (list->data);
        }
    }

  g_list_free (anchors);

  anchors = gimp_stroke_get_draw_anchors (stroke);

  for (list = anchors; list; list = g_list_next (list))
    {
      dx = coord->x - GIMP_ANCHOR (list->data)->position.x;
      dy = coord->y - GIMP_ANCHOR (list->data)->position.y;

      if (mindist < 0 || mindist > dx * dx + dy * dy)
        {
          mindist = dx * dx + dy * dy;
          anchor = GIMP_ANCHOR (list->data);
        }
    }

  g_list_free (anchors);

  return anchor;
}


GimpAnchor *
gimp_stroke_anchor_get_next (const GimpStroke *stroke,
                             const GimpAnchor *prev)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->anchor_get_next (stroke, prev);
}

static GimpAnchor *
gimp_stroke_real_anchor_get_next (const GimpStroke *stroke,
                                  const GimpAnchor *prev)
{
  GList *list;

  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  if (prev)
    {
      list = g_list_find (stroke->anchors, prev);
      if (list)
        list = g_list_next (list);
    }
  else
    {
      list = stroke->anchors;
    }

  if (list)
    return GIMP_ANCHOR (list->data);

  return NULL;
}


void
gimp_stroke_anchor_select (GimpStroke *stroke,
                           GimpAnchor *anchor,
                           gboolean    selected,
                           gboolean    exclusive)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->anchor_select (stroke, anchor,
                                                 selected, exclusive);
}

static void
gimp_stroke_real_anchor_select (GimpStroke *stroke,
                                GimpAnchor *anchor,
                                gboolean    selected,
                                gboolean    exclusive)
{
  GList *list;

  list = stroke->anchors;

  if (exclusive)
    {
      while (list)
        {
          GIMP_ANCHOR (list->data)->selected = FALSE;
          list = g_list_next (list);
        }
    }

  list = g_list_find (stroke->anchors, anchor);

  if (list)
    GIMP_ANCHOR (list->data)->selected = selected;
}


void
gimp_stroke_anchor_move_relative (GimpStroke            *stroke,
                                  GimpAnchor            *anchor,
                                  const GimpCoords      *delta,
                                  GimpAnchorFeatureType  feature)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));
  g_return_if_fail (anchor != NULL);
  g_return_if_fail (g_list_find (stroke->anchors, anchor));

  GIMP_STROKE_GET_CLASS (stroke)->anchor_move_relative (stroke, anchor,
                                                        delta, feature);
}

static void
gimp_stroke_real_anchor_move_relative (GimpStroke            *stroke,
                                       GimpAnchor            *anchor,
                                       const GimpCoords      *delta,
                                       GimpAnchorFeatureType  feature)
{
  anchor->position.x += delta->x;
  anchor->position.y += delta->y;
}


void
gimp_stroke_anchor_move_absolute (GimpStroke            *stroke,
                                  GimpAnchor            *anchor,
                                  const GimpCoords      *coord,
                                  GimpAnchorFeatureType  feature)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));
  g_return_if_fail (anchor != NULL);
  g_return_if_fail (g_list_find (stroke->anchors, anchor));

  GIMP_STROKE_GET_CLASS (stroke)->anchor_move_absolute (stroke, anchor,
                                                        coord, feature);
}

static void
gimp_stroke_real_anchor_move_absolute (GimpStroke            *stroke,
                                       GimpAnchor            *anchor,
                                       const GimpCoords      *coord,
                                       GimpAnchorFeatureType  feature)
{
  anchor->position.x = coord->x;
  anchor->position.y = coord->y;
}

gboolean
gimp_stroke_point_is_movable (GimpStroke *stroke,
                              GimpAnchor *predec,
                              gdouble     position)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

  return GIMP_STROKE_GET_CLASS (stroke)->point_is_movable (stroke, predec,
                                                           position);
}


gboolean
gimp_stroke_real_point_is_movable (GimpStroke *stroke,
                                   GimpAnchor *predec,
                                   gdouble     position)
{
  return FALSE;
}


void
gimp_stroke_point_move_relative (GimpStroke            *stroke,
                                 GimpAnchor            *predec,
                                 gdouble                position,
                                 const GimpCoords      *deltacoord,
                                 GimpAnchorFeatureType  feature)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->point_move_relative (stroke, predec,
                                                       position, deltacoord,
                                                       feature);
}


void
gimp_stroke_real_point_move_relative (GimpStroke           *stroke,
                                      GimpAnchor           *predec,
                                      gdouble               position,
                                      const GimpCoords     *deltacoord,
                                      GimpAnchorFeatureType feature)
{
  g_printerr ("gimp_stroke_point_move_relative: default implementation\n");
}


void
gimp_stroke_point_move_absolute (GimpStroke            *stroke,
                                 GimpAnchor            *predec,
                                 gdouble                position,
                                 const GimpCoords      *coord,
                                 GimpAnchorFeatureType  feature)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->point_move_absolute (stroke, predec,
                                                       position, coord,
                                                       feature);
}

void
gimp_stroke_real_point_move_absolute (GimpStroke           *stroke,
                                      GimpAnchor           *predec,
                                      gdouble               position,
                                      const GimpCoords     *coord,
                                      GimpAnchorFeatureType feature)
{
  g_printerr ("gimp_stroke_point_move_absolute: default implementation\n");
}


void
gimp_stroke_close (GimpStroke *stroke)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->close (stroke);
}

static void
gimp_stroke_real_close (GimpStroke *stroke)
{
  stroke->closed = TRUE;
  g_object_notify (G_OBJECT (stroke), "closed");
}


void
gimp_stroke_anchor_convert (GimpStroke            *stroke,
                            GimpAnchor            *anchor,
                            GimpAnchorFeatureType  feature)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->anchor_convert (stroke, anchor, feature);
}

static void
gimp_stroke_real_anchor_convert (GimpStroke            *stroke,
                                 GimpAnchor            *anchor,
                                 GimpAnchorFeatureType  feature)
{
  g_printerr ("gimp_stroke_anchor_convert: default implementation\n");
}


void
gimp_stroke_anchor_delete (GimpStroke *stroke,
                           GimpAnchor *anchor)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->anchor_delete (stroke, anchor);
}

static void
gimp_stroke_real_anchor_delete (GimpStroke *stroke,
                                GimpAnchor *anchor)
{
  g_printerr ("gimp_stroke_anchor_delete: default implementation\n");
}

GimpStroke *
gimp_stroke_open (GimpStroke *stroke,
                  GimpAnchor *end_anchor)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->open (stroke, end_anchor);
}

static GimpStroke *
gimp_stroke_real_open (GimpStroke *stroke,
                       GimpAnchor *end_anchor)
{
  g_printerr ("gimp_stroke_open: default implementation\n");
  return NULL;
}

gboolean
gimp_stroke_anchor_is_insertable (GimpStroke *stroke,
                                  GimpAnchor *predec,
                                  gdouble     position)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

  return GIMP_STROKE_GET_CLASS (stroke)->anchor_is_insertable (stroke,
                                                               predec,
                                                               position);
}

gboolean
gimp_stroke_real_anchor_is_insertable (GimpStroke *stroke,
                                       GimpAnchor *predec,
                                       gdouble     position)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

  return FALSE;
}

GimpAnchor *
gimp_stroke_anchor_insert (GimpStroke *stroke,
                           GimpAnchor *predec,
                           gdouble     position)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->anchor_insert (stroke,
                                                        predec, position);
}

GimpAnchor *
gimp_stroke_real_anchor_insert (GimpStroke *stroke,
                                GimpAnchor *predec,
                                gdouble     position)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return NULL;
}


gboolean
gimp_stroke_is_extendable (GimpStroke *stroke,
                           GimpAnchor *neighbor)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

  return GIMP_STROKE_GET_CLASS (stroke)->is_extendable (stroke, neighbor);
}

static gboolean
gimp_stroke_real_is_extendable (GimpStroke *stroke,
                                GimpAnchor *neighbor)
{
  return FALSE;
}


GimpAnchor *
gimp_stroke_extend (GimpStroke           *stroke,
                    const GimpCoords     *coords,
                    GimpAnchor           *neighbor,
                    GimpVectorExtendMode  extend_mode)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->extend (stroke, coords,
                                                 neighbor, extend_mode);
}

static GimpAnchor *
gimp_stroke_real_extend (GimpStroke           *stroke,
                         const GimpCoords     *coords,
                         GimpAnchor           *neighbor,
                         GimpVectorExtendMode  extend_mode)
{
  g_printerr ("gimp_stroke_extend: default implementation\n");
  return NULL;
}

gboolean
gimp_stroke_connect_stroke (GimpStroke *stroke,
                            GimpAnchor *anchor,
                            GimpStroke *extension,
                            GimpAnchor *neighbor)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);
  g_return_val_if_fail (GIMP_IS_STROKE (extension), FALSE);

  return GIMP_STROKE_GET_CLASS (stroke)->connect_stroke (stroke, anchor,
                                                         extension, neighbor);
}

gboolean
gimp_stroke_real_connect_stroke (GimpStroke *stroke,
                                 GimpAnchor *anchor,
                                 GimpStroke *extension,
                                 GimpAnchor *neighbor)
{
  g_printerr ("gimp_stroke_connect_stroke: default implementation\n");
  return FALSE;
}

gboolean
gimp_stroke_is_empty (const GimpStroke *stroke)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

  return GIMP_STROKE_GET_CLASS (stroke)->is_empty (stroke);
}

static gboolean
gimp_stroke_real_is_empty (const GimpStroke *stroke)
{
  return stroke->anchors == NULL;
}


gdouble
gimp_stroke_get_length (const GimpStroke *stroke,
                        const gdouble     precision)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), 0.0);

  return GIMP_STROKE_GET_CLASS (stroke)->get_length (stroke, precision);
}

static gdouble
gimp_stroke_real_get_length (const GimpStroke *stroke,
                             const gdouble     precision)
{
  GArray     *points;
  gint        i;
  gdouble     length;
  GimpCoords  difference; 

  g_return_val_if_fail (GIMP_IS_STROKE (stroke), -1);

  if (!stroke->anchors)
    return -1;
    
  points = gimp_stroke_interpolate (stroke, precision, NULL);
  if (points == NULL)
    return -1;

  length = 0;

  for (i = 0; i < points->len - 1; i++ )
    {
       gimp_coords_difference (&(g_array_index (points, GimpCoords, i)), 
                               &(g_array_index (points, GimpCoords, i+1)),
                               &difference);
       length += gimp_coords_length (&difference);
    }

  g_array_free(points, TRUE);

  return length;
}


gdouble
gimp_stroke_get_distance (const GimpStroke *stroke,
                          const GimpCoords  *coord)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), 0.0);

  return GIMP_STROKE_GET_CLASS (stroke)->get_distance (stroke, coord);
}

static gdouble
gimp_stroke_real_get_distance (const GimpStroke *stroke,
                               const GimpCoords  *coord)
{
  g_printerr ("gimp_stroke_get_distance: default implementation\n");

  return 0.0;
}


GArray *
gimp_stroke_interpolate (const GimpStroke *stroke,
                         gdouble           precision,
                         gboolean         *ret_closed)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->interpolate (stroke, precision,
                                                      ret_closed);
}

static GArray *
gimp_stroke_real_interpolate (const GimpStroke  *stroke,
                              gdouble            precision,
                              gboolean          *ret_closed)
{
  g_printerr ("gimp_stroke_interpolate: default implementation\n");

  return NULL;
}

GimpStroke *
gimp_stroke_duplicate (const GimpStroke *stroke)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->duplicate (stroke);
}

GimpStroke *
gimp_stroke_real_duplicate (const GimpStroke *stroke)
{
  GimpStroke *new_stroke;
  GList      *list;

  new_stroke = g_object_new (G_TYPE_FROM_INSTANCE (stroke),
                             "name", GIMP_OBJECT (stroke)->name,
                             NULL);

  new_stroke->anchors = g_list_copy (stroke->anchors);

  for (list = new_stroke->anchors; list; list = g_list_next (list))
    {
      list->data = gimp_anchor_duplicate (GIMP_ANCHOR (list->data));
    }

  new_stroke->closed = stroke->closed;

  return new_stroke;
}


GimpStroke *
gimp_stroke_make_bezier (const GimpStroke *stroke)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->make_bezier (stroke);
}

static GimpStroke *
gimp_stroke_real_make_bezier (const GimpStroke *stroke)
{
  g_printerr ("gimp_stroke_make_bezier: default implementation\n");

  return NULL;
}


void
gimp_stroke_translate (GimpStroke *stroke,
                       gdouble     offset_x,
                       gdouble     offset_y)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->translate (stroke, offset_x, offset_y);
}

static void
gimp_stroke_real_translate (GimpStroke *stroke,
                            gdouble     offset_x,
                            gdouble     offset_y)
{
  GList *list;

  for (list = stroke->anchors; list; list = g_list_next (list))
    {
      GimpAnchor *anchor = list->data;

      anchor->position.x += offset_x;
      anchor->position.y += offset_y;
    }
}


void
gimp_stroke_scale (GimpStroke *stroke,
                   gdouble     scale_x,
                   gdouble     scale_y)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->scale (stroke, scale_x, scale_y);
}

static void
gimp_stroke_real_scale (GimpStroke *stroke,
                        gdouble     scale_x,
                        gdouble     scale_y)
{
  GList *list;

  for (list = stroke->anchors; list; list = g_list_next (list))
    {
      GimpAnchor *anchor = list->data;

      anchor->position.x *= scale_x;
      anchor->position.y *= scale_y;
    }
}

void
gimp_stroke_transform (GimpStroke        *stroke,
                       const GimpMatrix3 *matrix)
{
  g_return_if_fail (GIMP_IS_STROKE (stroke));

  GIMP_STROKE_GET_CLASS (stroke)->transform (stroke, matrix);
}

static void
gimp_stroke_real_transform (GimpStroke        *stroke,
                            const GimpMatrix3 *matrix)
{
  GList *list;

  for (list = stroke->anchors; list; list = g_list_next (list))
    {
      GimpAnchor *anchor = list->data;

      gimp_matrix3_transform_point (matrix,
                                    anchor->position.x,
                                    anchor->position.y,
                                    &anchor->position.x,
                                    &anchor->position.y);
    }
}


GList *
gimp_stroke_get_draw_anchors (const GimpStroke  *stroke)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->get_draw_anchors (stroke);
}

static GList *
gimp_stroke_real_get_draw_anchors (const GimpStroke  *stroke)
{
  GList *list;
  GList *ret_list = NULL;

  for (list = stroke->anchors; list; list = g_list_next (list))
    {
      if (GIMP_ANCHOR (list->data)->type == GIMP_ANCHOR_ANCHOR)
        ret_list = g_list_prepend (ret_list, list->data);
    }

  return g_list_reverse (ret_list);
}


GList *
gimp_stroke_get_draw_controls (const GimpStroke  *stroke)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->get_draw_controls (stroke);
}

static GList *
gimp_stroke_real_get_draw_controls (const GimpStroke  *stroke)
{
  GList *list;
  GList *ret_list = NULL;

  for (list = stroke->anchors; list; list = g_list_next (list))
    {
      GimpAnchor *anchor = list->data;

      if (anchor->type == GIMP_ANCHOR_CONTROL)
        {
          GimpAnchor *next = list->next ? list->next->data : NULL;
          GimpAnchor *prev = list->prev ? list->prev->data : NULL;

          if (next && next->type == GIMP_ANCHOR_ANCHOR && next->selected)
            {
              /* Ok, this is a hack.
               * The idea is to give control points at the end of a
               * stroke a higher priority for the interactive tool. */
              if (prev)
                ret_list = g_list_prepend (ret_list, anchor);
              else
                ret_list = g_list_append (ret_list, anchor);
            }
          else if (prev && prev->type == GIMP_ANCHOR_ANCHOR && prev->selected)
            {
              /* same here... */
              if (next)
                ret_list = g_list_prepend (ret_list, anchor);
              else
                ret_list = g_list_append (ret_list, anchor);
            }
        }
    }

  return g_list_reverse (ret_list);
}


GArray *
gimp_stroke_get_draw_lines (const GimpStroke  *stroke)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->get_draw_lines (stroke);
}

static GArray *
gimp_stroke_real_get_draw_lines (const GimpStroke  *stroke)
{
  GList  *list;
  GArray *ret_lines = NULL;
  gint    count = 0;

  for (list = stroke->anchors; list; list = g_list_next (list))
    {
      GimpAnchor *anchor = list->data;

      if (anchor->type == GIMP_ANCHOR_ANCHOR && anchor->selected)
        {
          if (list->next)
            {
              GimpAnchor *next = list->next->data;

              if (count == 0)
                ret_lines = g_array_new (FALSE, FALSE, sizeof (GimpCoords));

              ret_lines = g_array_append_val (ret_lines, anchor->position);
              ret_lines = g_array_append_val (ret_lines, next->position);
              count += 1;
            }

          if (list->prev)
            {
              GimpAnchor *prev = list->prev->data;

              if (count == 0)
                ret_lines = g_array_new (FALSE, FALSE, sizeof (GimpCoords));

              ret_lines = g_array_append_val (ret_lines, anchor->position);
              ret_lines = g_array_append_val (ret_lines, prev->position);
              count += 1;
            }
        }
    }

  return ret_lines;
}

GArray *
gimp_stroke_control_points_get (const GimpStroke *stroke,
                                gboolean         *ret_closed)
{
  g_return_val_if_fail (GIMP_IS_STROKE (stroke), NULL);

  return GIMP_STROKE_GET_CLASS (stroke)->control_points_get (stroke,
                                                             ret_closed);
}

static GArray *
gimp_stroke_real_control_points_get (const GimpStroke *stroke,
                                     gboolean         *ret_closed)
{
  guint num_anchors;
  GArray *ret_array;
  GList *list;

  num_anchors = g_list_length (stroke->anchors);
  list = g_list_first (stroke->anchors);

  ret_array = g_array_sized_new (FALSE, FALSE,
                                 sizeof (GimpAnchor), num_anchors);

  for (list = g_list_first (stroke->anchors); list; list = g_list_next (list))
    {
      g_array_append_vals (ret_array, list->data, 1);
    }

  if (ret_closed)
    *ret_closed = stroke->closed;

  return ret_array;
}

gboolean
gimp_stroke_get_point_at_dist (const GimpStroke *stroke,
                               const gdouble     dist,
                               const gdouble     precision,
                               GimpCoords       *position,
                               gdouble          *gradient)
{
   g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

   return GIMP_STROKE_GET_CLASS (stroke)->get_point_at_dist (stroke,
                                                             dist,
                                                             precision,
                                                             position,
                                                             gradient);
}


static gboolean
gimp_stroke_real_get_point_at_dist (const GimpStroke *stroke,
                                    const gdouble     dist,
                                    const gdouble     precision,
                                    GimpCoords       *position,
                                    gdouble          *gradient)
{
  GArray     *points;
  gint        i;
  gdouble     length;
  gdouble     segment_length;
  gboolean    ret = FALSE;
  GimpCoords  difference;

  g_return_val_if_fail (GIMP_IS_STROKE (stroke), FALSE);

  points = gimp_stroke_interpolate (stroke, precision, NULL);
  if (points == NULL)
    return ret;

  length = 0;
  for (i=0; i < points->len - 1; i++)
    {
      gimp_coords_difference (&(g_array_index (points, GimpCoords , i)),
                              &(g_array_index (points, GimpCoords , i+1)),
                              &difference);
      segment_length = gimp_coords_length (&difference);

      if (segment_length == 0 || length + segment_length < dist )
        {
          length += segment_length;
        }
      else
        {
          /* x = x1 + (x2 - x1 ) u  */
          /* x   = x1 (1-u) + u x2  */

          gdouble u = (dist - length) / segment_length;

          gimp_coords_mix (1 - u, &(g_array_index (points, GimpCoords , i)),
                               u, &(g_array_index (points, GimpCoords , i+1)),
                           position);

          if (difference.y == 0)
            *gradient = G_MAXDOUBLE;
          else 
            *gradient = difference.x / difference.y;

          ret = TRUE;
          break;
        }
    }
  
  g_array_free (points, TRUE);

  return ret;
}
