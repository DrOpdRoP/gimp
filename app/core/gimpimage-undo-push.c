/* GIMP - The GNU Image Manipulation Program
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

#include <glib-object.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpconfig/gimpconfig.h"

#include "core-types.h"

#include "base/tile-manager.h"

#include "gimp.h"
#include "gimp-parasites.h"
#include "gimpchannelpropundo.h"
#include "gimpchannelundo.h"
#include "gimpdrawableundo.h"
#include "gimpgrid.h"
#include "gimpguide.h"
#include "gimpimage.h"
#include "gimpimage-guides.h"
#include "gimpimage-sample-points.h"
#include "gimpimage-undo.h"
#include "gimpimage-undo-push.h"
#include "gimpimageundo.h"
#include "gimpitempropundo.h"
#include "gimplayer.h"
#include "gimplayer-floating-sel.h"
#include "gimplayermask.h"
#include "gimplayerpropundo.h"
#include "gimplist.h"
#include "gimpparasitelist.h"
#include "gimpselection.h"

#include "text/gimptextlayer.h"
#include "text/gimptextundo.h"

#include "vectors/gimpvectors.h"

#include "gimp-intl.h"


/*********************/
/*  Image Type Undo  */
/*********************/

GimpUndo *
gimp_image_undo_push_image_type (GimpImage   *image,
                                 const gchar *undo_desc)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_IMAGE_UNDO,
                               0, 0,
                               GIMP_UNDO_IMAGE_TYPE, undo_desc,
                               GIMP_DIRTY_IMAGE,
                               NULL, NULL,
                               NULL);
}


/*********************/
/*  Image Size Undo  */
/*********************/

GimpUndo *
gimp_image_undo_push_image_size (GimpImage   *image,
                                 const gchar *undo_desc)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_IMAGE_UNDO,
                               0, 0,
                               GIMP_UNDO_IMAGE_SIZE, undo_desc,
                               GIMP_DIRTY_IMAGE | GIMP_DIRTY_IMAGE_SIZE,
                               NULL, NULL,
                               NULL);
}


/***************************/
/*  Image Resolution Undo  */
/***************************/

GimpUndo *
gimp_image_undo_push_image_resolution (GimpImage   *image,
                                       const gchar *undo_desc)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_IMAGE_UNDO,
                               0, 0,
                               GIMP_UNDO_IMAGE_RESOLUTION, undo_desc,
                               GIMP_DIRTY_IMAGE,
                               NULL, NULL,
                               NULL);
}


/****************/
/*  Guide Undo  */
/****************/

typedef struct _GuideUndo GuideUndo;

struct _GuideUndo
{
  GimpGuide           *guide;
  GimpOrientationType  orientation;
  gint                 position;
};

static gboolean undo_pop_image_guide  (GimpUndo            *undo,
                                       GimpUndoMode         undo_mode,
                                       GimpUndoAccumulator *accum);
static void     undo_free_image_guide (GimpUndo            *undo,
                                       GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_image_guide (GimpImage   *image,
                                  const gchar *undo_desc,
                                  GimpGuide   *guide)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_GUIDE (guide), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (GuideUndo),
                                   sizeof (GuideUndo),
                                   GIMP_UNDO_IMAGE_GUIDE, undo_desc,
                                   GIMP_DIRTY_IMAGE_META,
                                   undo_pop_image_guide,
                                   undo_free_image_guide,
                                   NULL)))
    {
      GuideUndo *gu = new->data;

      gu->guide       = g_object_ref (guide);
      gu->orientation = gimp_guide_get_orientation (guide);
      gu->position    = gimp_guide_get_position (guide);

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_image_guide (GimpUndo            *undo,
                      GimpUndoMode         undo_mode,
                      GimpUndoAccumulator *accum)
{
  GuideUndo           *gu = undo->data;
  GimpOrientationType  old_orientation;
  gint                 old_position;

  old_orientation = gimp_guide_get_orientation (gu->guide);
  old_position    = gimp_guide_get_position (gu->guide);

  /*  add and move guides manually (nor using the gimp_image_guide
   *  API), because we might be in the middle of an image resizing
   *  undo group and the guide's position might be temporarily out of
   *  image.
   */

  if (old_position == -1)
    {
      undo->image->guides = g_list_prepend (undo->image->guides, gu->guide);
      gimp_guide_set_position (gu->guide, gu->position);
      g_object_ref (gu->guide);
      gimp_image_update_guide (undo->image, gu->guide);
    }
  else if (gu->position == -1)
    {
      gimp_image_remove_guide (undo->image, gu->guide, FALSE);
    }
  else
    {
      gimp_image_update_guide (undo->image, gu->guide);
      gimp_guide_set_position (gu->guide, gu->position);
      gimp_image_update_guide (undo->image, gu->guide);
    }

  gimp_guide_set_orientation (gu->guide, gu->orientation);

  gu->position    = old_position;
  gu->orientation = old_orientation;

  return TRUE;
}

static void
undo_free_image_guide (GimpUndo     *undo,
                       GimpUndoMode  undo_mode)
{
  GuideUndo *gu = undo->data;

  g_object_unref (gu->guide);
  g_free (gu);
}


/****************/
/*  Grid Undo   */
/****************/

GimpUndo *
gimp_image_undo_push_image_grid (GimpImage   *image,
                                 const gchar *undo_desc,
                                 GimpGrid    *grid)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_GRID (grid), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_IMAGE_UNDO,
                               0, 0,
                               GIMP_UNDO_IMAGE_GRID, undo_desc,
                               GIMP_DIRTY_IMAGE_META,
                               NULL, NULL,
                               "grid", grid,
                               NULL);
}


/**********************/
/*  Sampe Point Undo  */
/**********************/

typedef struct _SamplePointUndo SamplePointUndo;

struct _SamplePointUndo
{
  GimpSamplePoint *sample_point;
  gint             x;
  gint             y;
};

static gboolean undo_pop_image_sample_point  (GimpUndo            *undo,
                                              GimpUndoMode         undo_mode,
                                              GimpUndoAccumulator *accum);
static void     undo_free_image_sample_point (GimpUndo            *undo,
                                              GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_image_sample_point (GimpImage       *image,
                                         const gchar     *undo_desc,
                                         GimpSamplePoint *sample_point)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (sample_point != NULL, NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (SamplePointUndo),
                                   sizeof (SamplePointUndo),
                                   GIMP_UNDO_IMAGE_SAMPLE_POINT, undo_desc,
                                   GIMP_DIRTY_IMAGE_META,
                                   undo_pop_image_sample_point,
                                   undo_free_image_sample_point,
                                   NULL)))
    {
      SamplePointUndo *spu = new->data;

      spu->sample_point = gimp_image_sample_point_ref (sample_point);
      spu->x            = sample_point->x;
      spu->y            = sample_point->y;

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_image_sample_point (GimpUndo            *undo,
                             GimpUndoMode         undo_mode,
                             GimpUndoAccumulator *accum)
{
  SamplePointUndo *spu = undo->data;
  gint             old_x;
  gint             old_y;

  old_x = spu->sample_point->x;
  old_y = spu->sample_point->y;

  /*  add and move sample points manually (nor using the
   *  gimp_image_sample_point API), because we might be in the middle
   *  of an image resizing undo group and the sample point's position
   *  might be temporarily out of image.
   */

  if (spu->sample_point->x == -1)
    {
      undo->image->sample_points = g_list_append (undo->image->sample_points,
                                                   spu->sample_point);

      spu->sample_point->x = spu->x;
      spu->sample_point->y = spu->y;
      gimp_image_sample_point_ref (spu->sample_point);

      gimp_image_sample_point_added (undo->image, spu->sample_point);
      gimp_image_update_sample_point (undo->image, spu->sample_point);
    }
  else if (spu->x == -1)
    {
      gimp_image_remove_sample_point (undo->image, spu->sample_point, FALSE);
    }
  else
    {
      gimp_image_update_sample_point (undo->image, spu->sample_point);
      spu->sample_point->x = spu->x;
      spu->sample_point->y = spu->y;
      gimp_image_update_sample_point (undo->image, spu->sample_point);
    }

  spu->x = old_x;
  spu->y = old_y;

  return TRUE;
}

static void
undo_free_image_sample_point (GimpUndo     *undo,
                              GimpUndoMode  undo_mode)
{
  SamplePointUndo *gu = undo->data;

  gimp_image_sample_point_unref (gu->sample_point);
  g_free (gu);
}


/*******************/
/*  Colormap Undo  */
/*******************/

GimpUndo *
gimp_image_undo_push_image_colormap (GimpImage   *image,
                                     const gchar *undo_desc)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_IMAGE_UNDO,
                               0, 0,
                               GIMP_UNDO_IMAGE_COLORMAP, undo_desc,
                               GIMP_DIRTY_IMAGE,
                               NULL, NULL,
                               NULL);
}


/*******************/
/*  Drawable Undo  */
/*******************/

GimpUndo *
gimp_image_undo_push_drawable (GimpImage    *image,
                               const gchar  *undo_desc,
                               GimpDrawable *drawable,
                               TileManager  *tiles,
                               gboolean      sparse,
                               gint          x,
                               gint          y,
                               gint          width,
                               gint          height)
{
  GimpItem *item;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_DRAWABLE (drawable), NULL);
  g_return_val_if_fail (tiles != NULL, NULL);
  g_return_val_if_fail (sparse == TRUE ||
                        tile_manager_width (tiles) == width, NULL);
  g_return_val_if_fail (sparse == TRUE ||
                        tile_manager_height (tiles) == height, NULL);

  item = GIMP_ITEM (drawable);

  g_return_val_if_fail (gimp_item_is_attached (item), NULL);
  g_return_val_if_fail (sparse == FALSE ||
                        tile_manager_width (tiles) == gimp_item_width (item),
                        NULL);
  g_return_val_if_fail (sparse == FALSE ||
                        tile_manager_height (tiles) == gimp_item_height (item),
                        NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_DRAWABLE_UNDO,
                               0, 0,
                               GIMP_UNDO_DRAWABLE, undo_desc,
                               GIMP_DIRTY_ITEM | GIMP_DIRTY_DRAWABLE,
                               NULL, NULL,
                               "item",   item,
                               "tiles",  tiles,
                               "sparse", sparse,
                               "x",      x,
                               "y",      y,
                               "width",  width,
                               "height", height,
                               NULL);
}


/***********************/
/*  Drawable Mod Undo  */
/***********************/

typedef struct _DrawableModUndo DrawableModUndo;

struct _DrawableModUndo
{
  TileManager   *tiles;
  GimpImageType  type;
  gint           offset_x;
  gint           offset_y;
};

static gboolean undo_pop_drawable_mod  (GimpUndo            *undo,
                                        GimpUndoMode         undo_mode,
                                        GimpUndoAccumulator *accum);
static void     undo_free_drawable_mod (GimpUndo            *undo,
                                        GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_drawable_mod (GimpImage    *image,
                                   const gchar  *undo_desc,
                                   GimpDrawable *drawable)
{
  GimpUndo *new;
  gint64    size;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_DRAWABLE (drawable), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (drawable)), NULL);

  size = sizeof (DrawableModUndo) + tile_manager_get_memsize (drawable->tiles,
                                                              FALSE);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (DrawableModUndo),
                                   GIMP_UNDO_DRAWABLE_MOD, undo_desc,
                                   GIMP_DIRTY_ITEM | GIMP_DIRTY_DRAWABLE,
                                   undo_pop_drawable_mod,
                                   undo_free_drawable_mod,
                                   "item", drawable,
                                   NULL)))
    {
      DrawableModUndo *drawable_undo = new->data;

      drawable_undo->tiles    = tile_manager_ref (drawable->tiles);
      drawable_undo->type     = drawable->type;
      drawable_undo->offset_x = GIMP_ITEM (drawable)->offset_x;
      drawable_undo->offset_y = GIMP_ITEM (drawable)->offset_y;

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_drawable_mod (GimpUndo            *undo,
                       GimpUndoMode         undo_mode,
                       GimpUndoAccumulator *accum)
{
  DrawableModUndo *drawable_undo = undo->data;
  GimpDrawable    *drawable      = GIMP_DRAWABLE (GIMP_ITEM_UNDO (undo)->item);
  TileManager     *tiles;
  GimpImageType    drawable_type;
  gint             offset_x, offset_y;

  undo->size -= tile_manager_get_memsize (drawable_undo->tiles, FALSE);

  tiles         = drawable_undo->tiles;
  drawable_type = drawable_undo->type;
  offset_x      = drawable_undo->offset_x;
  offset_y      = drawable_undo->offset_y;

  drawable_undo->tiles    = tile_manager_ref (drawable->tiles);
  drawable_undo->type     = drawable->type;
  drawable_undo->offset_x = GIMP_ITEM (drawable)->offset_x;
  drawable_undo->offset_y = GIMP_ITEM (drawable)->offset_y;

  gimp_drawable_set_tiles_full (drawable, FALSE, NULL,
                                tiles, drawable_type, offset_x, offset_y);
  tile_manager_unref (tiles);

  undo->size += tile_manager_get_memsize (drawable_undo->tiles, FALSE);

  return TRUE;
}

static void
undo_free_drawable_mod (GimpUndo     *undo,
                        GimpUndoMode  undo_mode)
{
  DrawableModUndo *drawable_undo = undo->data;

  tile_manager_unref (drawable_undo->tiles);
  g_free (drawable_undo);
}


/***************/
/*  Mask Undo  */
/***************/

GimpUndo *
gimp_image_undo_push_mask (GimpImage   *image,
                           const gchar *undo_desc,
                           GimpChannel *mask)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_CHANNEL (mask), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (mask)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_CHANNEL_UNDO,
                               0, 0,
                               GIMP_UNDO_MASK, undo_desc,
                               GIMP_IS_SELECTION (mask) ?
                               GIMP_DIRTY_SELECTION :
                               GIMP_DIRTY_ITEM | GIMP_DIRTY_DRAWABLE,
                               NULL, NULL,
                               "item", mask,
                               NULL);
}


/**********************/
/*  Item Rename Undo  */
/**********************/

GimpUndo *
gimp_image_undo_push_item_rename (GimpImage   *image,
                                  const gchar *undo_desc,
                                  GimpItem    *item)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_ITEM (item), NULL);
  g_return_val_if_fail (gimp_item_is_attached (item), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_ITEM_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_ITEM_RENAME, undo_desc,
                               GIMP_DIRTY_ITEM_META,
                               NULL, NULL,
                               "item", item,
                               NULL);
}


/****************************/
/*  Item displacement Undo  */
/****************************/

GimpUndo *
gimp_image_undo_push_item_displace (GimpImage   *image,
                                    const gchar *undo_desc,
                                    GimpItem    *item)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_ITEM (item), NULL);
  g_return_val_if_fail (gimp_item_is_attached (item), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_ITEM_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_ITEM_DISPLACE, undo_desc,
                               GIMP_IS_DRAWABLE (item) ?
                               GIMP_DIRTY_ITEM | GIMP_DIRTY_DRAWABLE :
                               GIMP_DIRTY_ITEM | GIMP_DIRTY_VECTORS,
                               NULL, NULL,
                               "item", item,
                               NULL);
}


/******************************/
/*  Item Visibility Undo  */
/******************************/

GimpUndo *
gimp_image_undo_push_item_visibility (GimpImage   *image,
                                      const gchar *undo_desc,
                                      GimpItem    *item)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_ITEM (item), NULL);
  g_return_val_if_fail (gimp_item_is_attached (item), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_ITEM_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_ITEM_VISIBILITY, undo_desc,
                               GIMP_DIRTY_ITEM_META,
                               NULL, NULL,
                               "item", item,
                               NULL);
}


/**********************/
/*  Item linked Undo  */
/**********************/

GimpUndo *
gimp_image_undo_push_item_linked (GimpImage   *image,
                                  const gchar *undo_desc,
                                  GimpItem    *item)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_ITEM (item), NULL);
  g_return_val_if_fail (gimp_item_is_attached (item), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_ITEM_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_ITEM_LINKED, undo_desc,
                               GIMP_DIRTY_ITEM_META,
                               NULL, NULL,
                               "item", item,
                               NULL);
}


/***************************/
/*  Layer Add/Remove Undo  */
/***************************/

typedef struct _LayerUndo LayerUndo;

struct _LayerUndo
{
  gint       prev_position;   /*  former position in list  */
  GimpLayer *prev_layer;      /*  previous active layer    */
};

static GimpUndo * undo_push_layer (GimpImage           *image,
                                   const gchar         *undo_desc,
                                   GimpUndoType         type,
                                   GimpLayer           *layer,
                                   gint                 prev_position,
                                   GimpLayer           *prev_layer);
static gboolean   undo_pop_layer  (GimpUndo            *undo,
                                   GimpUndoMode         undo_mode,
                                   GimpUndoAccumulator *accum);
static void       undo_free_layer (GimpUndo            *undo,
                                   GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_layer_add (GimpImage   *image,
                                const gchar *undo_desc,
                                GimpLayer   *layer,
                                gint         prev_position,
                                GimpLayer   *prev_layer)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (! gimp_item_is_attached (GIMP_ITEM (layer)), NULL);
  g_return_val_if_fail (prev_layer == NULL || GIMP_IS_LAYER (prev_layer),
                        NULL);

  return undo_push_layer (image, undo_desc, GIMP_UNDO_LAYER_ADD,
                          layer, prev_position, prev_layer);
}

GimpUndo *
gimp_image_undo_push_layer_remove (GimpImage   *image,
                                   const gchar *undo_desc,
                                   GimpLayer   *layer,
                                   gint         prev_position,
                                   GimpLayer   *prev_layer)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);
  g_return_val_if_fail (prev_layer == NULL || GIMP_IS_LAYER (prev_layer),
                        NULL);

  return undo_push_layer (image, undo_desc, GIMP_UNDO_LAYER_REMOVE,
                          layer, prev_position, prev_layer);
}

static GimpUndo *
undo_push_layer (GimpImage    *image,
                 const gchar  *undo_desc,
                 GimpUndoType  type,
                 GimpLayer    *layer,
                 gint          prev_position,
                 GimpLayer    *prev_layer)
{
  GimpUndo *new;
  gint64    size;

  size = sizeof (LayerUndo);

  if (type == GIMP_UNDO_LAYER_REMOVE)
    size += gimp_object_get_memsize (GIMP_OBJECT (layer), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (LayerUndo),
                                   type, undo_desc,
                                   GIMP_DIRTY_IMAGE_STRUCTURE,
                                   undo_pop_layer,
                                   undo_free_layer,
                                   "item", layer,
                                   NULL)))
    {
      LayerUndo *lu = new->data;

      lu->prev_position = prev_position;
      lu->prev_layer    = prev_layer;

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_layer (GimpUndo            *undo,
                GimpUndoMode         undo_mode,
                GimpUndoAccumulator *accum)
{
  LayerUndo *lu    = undo->data;
  GimpLayer *layer = GIMP_LAYER (GIMP_ITEM_UNDO (undo)->item);
  gboolean   old_has_alpha;

  old_has_alpha = gimp_image_has_alpha (undo->image);

  if ((undo_mode       == GIMP_UNDO_MODE_UNDO &&
       undo->undo_type == GIMP_UNDO_LAYER_ADD) ||
      (undo_mode       == GIMP_UNDO_MODE_REDO &&
       undo->undo_type == GIMP_UNDO_LAYER_REMOVE))
    {
      /*  remove layer  */

      undo->size += gimp_object_get_memsize (GIMP_OBJECT (layer), NULL);

      /*  record the current position  */
      lu->prev_position = gimp_image_get_layer_index (undo->image, layer);

      gimp_container_remove (undo->image->layers, GIMP_OBJECT (layer));
      undo->image->layer_stack = g_slist_remove (undo->image->layer_stack,
                                                  layer);

      if (gimp_layer_is_floating_sel (layer))
        {
          /*  invalidate the boundary *before* setting the
           *  floating_sel pointer to NULL because the selection's
           *  outline is affected by the floating_sel and won't be
           *  completely cleared otherwise (bug #160247).
           */
          gimp_drawable_invalidate_boundary (GIMP_DRAWABLE (layer));

          undo->image->floating_sel = NULL;

          /*  activate the underlying drawable  */
          floating_sel_activate_drawable (layer);

          gimp_image_floating_selection_changed (undo->image);
        }
      else if (layer == gimp_image_get_active_layer (undo->image))
        {
          if (lu->prev_layer)
            {
              gimp_image_set_active_layer (undo->image, lu->prev_layer);
            }
          else if (undo->image->layer_stack)
            {
              gimp_image_set_active_layer (undo->image,
                                           undo->image->layer_stack->data);
            }
          else
            {
              gimp_image_set_active_layer (undo->image, NULL);
            }
        }

      gimp_item_removed (GIMP_ITEM (layer));
    }
  else
    {
      /*  restore layer  */

      undo->size -= gimp_object_get_memsize (GIMP_OBJECT (layer), NULL);

      /*  record the active layer  */
      lu->prev_layer = gimp_image_get_active_layer (undo->image);

      /*  if this is a floating selection, set the fs pointer  */
      if (gimp_layer_is_floating_sel (layer))
        undo->image->floating_sel = layer;

      gimp_container_insert (undo->image->layers,
                             GIMP_OBJECT (layer), lu->prev_position);
      gimp_image_set_active_layer (undo->image, layer);

      if (gimp_layer_is_floating_sel (layer))
        gimp_image_floating_selection_changed (undo->image);

      GIMP_ITEM (layer)->removed = FALSE;

      if (layer->mask)
        GIMP_ITEM (layer->mask)->removed = FALSE;
    }

  if (old_has_alpha != gimp_image_has_alpha (undo->image))
    accum->alpha_changed = TRUE;

  return TRUE;
}

static void
undo_free_layer (GimpUndo     *undo,
                 GimpUndoMode  undo_mode)
{
  g_free (undo->data);
}


/***************************/
/* Layer re-position Undo  */
/***************************/

GimpUndo *
gimp_image_undo_push_layer_reposition (GimpImage   *image,
                                       const gchar *undo_desc,
                                       GimpLayer   *layer)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_LAYER_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_LAYER_REPOSITION, undo_desc,
                               GIMP_DIRTY_IMAGE_STRUCTURE,
                               NULL, NULL,
                               "item", layer,
                               NULL);
}


/*********************/
/*  Layer Mode Undo  */
/*********************/

GimpUndo *
gimp_image_undo_push_layer_mode (GimpImage   *image,
                                 const gchar *undo_desc,
                                 GimpLayer   *layer)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_LAYER_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_LAYER_MODE, undo_desc,
                               GIMP_DIRTY_ITEM_META,
                               NULL, NULL,
                               "item", layer,
                               NULL);
}


/************************/
/*  Layer Opacity Undo  */
/************************/

GimpUndo *
gimp_image_undo_push_layer_opacity (GimpImage   *image,
                                    const gchar *undo_desc,
                                    GimpLayer   *layer)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_LAYER_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_LAYER_OPACITY, undo_desc,
                               GIMP_DIRTY_ITEM_META,
                               NULL, NULL,
                               "item", layer,
                               NULL);
}


/***************************/
/*  Layer Lock Alpha Undo  */
/***************************/

GimpUndo *
gimp_image_undo_push_layer_lock_alpha (GimpImage   *image,
                                       const gchar *undo_desc,
                                       GimpLayer   *layer)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_LAYER_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_LAYER_LOCK_ALPHA, undo_desc,
                               GIMP_DIRTY_ITEM_META,
                               NULL, NULL,
                               "item", layer,
                               NULL);
}


/*********************/
/*  Text Layer Undo  */
/*********************/

GimpUndo *
gimp_image_undo_push_text_layer (GimpImage        *image,
                                 const gchar      *undo_desc,
                                 GimpTextLayer    *layer,
                                 const GParamSpec *pspec)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_TEXT_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_TEXT_UNDO,
                               0, 0,
                               GIMP_UNDO_TEXT_LAYER, undo_desc,
                               GIMP_DIRTY_ITEM | GIMP_DIRTY_DRAWABLE,
                               NULL, NULL,
                               "item",  layer,
                               "param", pspec,
                               NULL);
}


/******************************/
/*  Text Layer Modified Undo  */
/******************************/

typedef struct _TextLayerModifiedUndo TextLayerModifiedUndo;

struct _TextLayerModifiedUndo
{
  gboolean old_modified;
};

static gboolean undo_pop_text_layer_modified  (GimpUndo            *undo,
                                               GimpUndoMode         undo_mode,
                                               GimpUndoAccumulator *accum);
static void     undo_free_text_layer_modified (GimpUndo            *undo,
                                               GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_text_layer_modified (GimpImage     *image,
                                          const gchar   *undo_desc,
                                          GimpTextLayer *layer)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_TEXT_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   sizeof (TextLayerModifiedUndo),
                                   sizeof (TextLayerModifiedUndo),
                                   GIMP_UNDO_TEXT_LAYER_MODIFIED, undo_desc,
                                   GIMP_DIRTY_ITEM_META,
                                   undo_pop_text_layer_modified,
                                   undo_free_text_layer_modified,
                                   "item", layer,
                                   NULL)))
    {
      TextLayerModifiedUndo *modified_undo = new->data;

      modified_undo->old_modified = layer->modified;

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_text_layer_modified (GimpUndo            *undo,
                              GimpUndoMode         undo_mode,
                              GimpUndoAccumulator *accum)
{
  TextLayerModifiedUndo *modified_undo = undo->data;
  GimpTextLayer         *layer;
  gboolean               modified;

  layer = GIMP_TEXT_LAYER (GIMP_ITEM_UNDO (undo)->item);

#if 0
  g_print ("setting layer->modified from %s to %s\n",
           layer->modified ? "TRUE" : "FALSE",
           modified_undo->old_modified ? "TRUE" : "FALSE");
#endif

  modified = layer->modified;
  g_object_set (layer, "modified", modified_undo->old_modified, NULL);
  modified_undo->old_modified = modified;

  gimp_viewable_invalidate_preview (GIMP_VIEWABLE (layer));

  return TRUE;
}

static void
undo_free_text_layer_modified (GimpUndo     *undo,
                               GimpUndoMode  undo_mode)
{
  g_free (undo->data);
}


/********************************/
/*  Layer Mask Add/Remove Undo  */
/********************************/

typedef struct _LayerMaskUndo LayerMaskUndo;

struct _LayerMaskUndo
{
  GimpLayerMask *mask;
};

static GimpUndo * undo_push_layer_mask (GimpImage           *image,
                                        const gchar         *undo_desc,
                                        GimpUndoType         type,
                                        GimpLayer           *layer,
                                        GimpLayerMask       *mask);
static gboolean   undo_pop_layer_mask  (GimpUndo            *undo,
                                        GimpUndoMode         undo_mode,
                                        GimpUndoAccumulator *accum);
static void       undo_free_layer_mask (GimpUndo            *undo,
                                        GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_layer_mask_add (GimpImage     *image,
                                     const gchar   *undo_desc,
                                     GimpLayer     *layer,
                                     GimpLayerMask *mask)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER_MASK (mask), NULL);
  g_return_val_if_fail (! gimp_item_is_attached (GIMP_ITEM (mask)), NULL);

  return undo_push_layer_mask (image, undo_desc, GIMP_UNDO_LAYER_MASK_ADD,
                               layer, mask);
}

GimpUndo *
gimp_image_undo_push_layer_mask_remove (GimpImage     *image,
                                        const gchar   *undo_desc,
                                        GimpLayer     *layer,
                                        GimpLayerMask *mask)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (layer), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (layer)), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER_MASK (mask), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (mask)), NULL);
  g_return_val_if_fail (mask->layer == layer, NULL);
  g_return_val_if_fail (layer->mask == mask, NULL);

  return undo_push_layer_mask (image, undo_desc, GIMP_UNDO_LAYER_MASK_REMOVE,
                               layer, mask);
}


static GimpUndo *
undo_push_layer_mask (GimpImage     *image,
                      const gchar   *undo_desc,
                      GimpUndoType   type,
                      GimpLayer     *layer,
                      GimpLayerMask *mask)
{
  GimpUndo *new;
  gint64    size;

  size = sizeof (LayerMaskUndo);

  if (type == GIMP_UNDO_LAYER_MASK_REMOVE)
    size += gimp_object_get_memsize (GIMP_OBJECT (mask), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (LayerMaskUndo),
                                   type, undo_desc,
                                   GIMP_DIRTY_IMAGE_STRUCTURE,
                                   undo_pop_layer_mask,
                                   undo_free_layer_mask,
                                   "item", layer,
                                   NULL)))
    {
      LayerMaskUndo *lmu = new->data;

      lmu->mask = g_object_ref (mask);

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_layer_mask (GimpUndo            *undo,
                     GimpUndoMode         undo_mode,
                     GimpUndoAccumulator *accum)
{
  LayerMaskUndo *lmu   = undo->data;
  GimpLayer     *layer = GIMP_LAYER (GIMP_ITEM_UNDO (undo)->item);

  if ((undo_mode       == GIMP_UNDO_MODE_UNDO &&
       undo->undo_type == GIMP_UNDO_LAYER_MASK_ADD) ||
      (undo_mode       == GIMP_UNDO_MODE_REDO &&
       undo->undo_type == GIMP_UNDO_LAYER_MASK_REMOVE))
    {
      /*  remove layer mask  */

      undo->size += gimp_object_get_memsize (GIMP_OBJECT (lmu->mask), NULL);

      gimp_layer_apply_mask (layer, GIMP_MASK_DISCARD, FALSE);
    }
  else
    {
      /*  restore layer  */

      undo->size -= gimp_object_get_memsize (GIMP_OBJECT (lmu->mask), NULL);

      gimp_layer_add_mask (layer, lmu->mask, FALSE);

      GIMP_ITEM (lmu->mask)->removed = FALSE;
    }

  return TRUE;
}

static void
undo_free_layer_mask (GimpUndo     *undo,
                      GimpUndoMode  undo_mode)
{
  LayerMaskUndo *lmu = undo->data;

  g_object_unref (lmu->mask);
  g_free (lmu);
}


/******************************/
/*  Layer Mask Property Undo  */
/******************************/

typedef struct _LayerMaskPropertyUndo LayerMaskPropertyUndo;

struct _LayerMaskPropertyUndo
{
  gboolean old_apply;
  gboolean old_show;
};

static GimpUndo * undo_push_layer_mask_properties (GimpImage           *image,
                                                   GimpUndoType         undo_type,
                                                   const gchar         *undo_desc,
                                                   GimpLayerMask       *mask);
static gboolean   undo_pop_layer_mask_properties  (GimpUndo            *undo,
                                                   GimpUndoMode         undo_mode,
                                                   GimpUndoAccumulator *accum);
static void       undo_free_layer_mask_properties (GimpUndo            *undo,
                                                   GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_layer_mask_apply (GimpImage     *image,
                                       const gchar   *undo_desc,
                                       GimpLayerMask *mask)
{
  return undo_push_layer_mask_properties (image, GIMP_UNDO_LAYER_MASK_APPLY,
                                          undo_desc, mask);
}

GimpUndo *
gimp_image_undo_push_layer_mask_show (GimpImage     *image,
                                      const gchar   *undo_desc,
                                      GimpLayerMask *mask)
{
  return undo_push_layer_mask_properties (image, GIMP_UNDO_LAYER_MASK_SHOW,
                                          undo_desc, mask);
}

static GimpUndo *
undo_push_layer_mask_properties (GimpImage     *image,
                                 GimpUndoType   undo_type,
                                 const gchar   *undo_desc,
                                 GimpLayerMask *mask)
{
  GimpUndo *new;
  gint64    size;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER_MASK (mask), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (mask)), NULL);

  size = sizeof (LayerMaskPropertyUndo);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (LayerMaskPropertyUndo),
                                   undo_type, undo_desc,
                                   GIMP_DIRTY_ITEM_META,
                                   undo_pop_layer_mask_properties,
                                   undo_free_layer_mask_properties,
                                   "item", mask,
                                   NULL)))
    {
      LayerMaskPropertyUndo *lmp_undo = new->data;

      lmp_undo->old_apply = gimp_layer_mask_get_apply (mask);
      lmp_undo->old_show  = gimp_layer_mask_get_show (mask);

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_layer_mask_properties (GimpUndo            *undo,
                                GimpUndoMode         undo_mode,
                                GimpUndoAccumulator *accum)
{
  LayerMaskPropertyUndo *lmp_undo = undo->data;
  GimpLayerMask         *mask;
  gboolean               val;

  mask = GIMP_LAYER_MASK (GIMP_ITEM_UNDO (undo)->item);

  switch (undo->undo_type)
    {
    case GIMP_UNDO_LAYER_MASK_APPLY:
      val = gimp_layer_mask_get_apply (mask);
      gimp_layer_mask_set_apply (mask, lmp_undo->old_apply, FALSE);
      lmp_undo->old_apply = val;
      break;

    case GIMP_UNDO_LAYER_MASK_SHOW:
      val = gimp_layer_mask_get_show (mask);
      gimp_layer_mask_set_show (mask, lmp_undo->old_show, FALSE);
      lmp_undo->old_show = val;
      break;

    default:
      g_return_val_if_reached (FALSE);
      break;
    }

  return TRUE;
}

static void
undo_free_layer_mask_properties (GimpUndo     *undo,
                                 GimpUndoMode  undo_mode)
{
  g_free (undo->data);
}


/*****************************/
/*  Add/Remove Channel Undo  */
/*****************************/

typedef struct _ChannelUndo ChannelUndo;

struct _ChannelUndo
{
  gint         prev_position;   /*  former position in list     */
  GimpChannel *prev_channel;    /*  previous active channel     */
};

static GimpUndo * undo_push_channel (GimpImage           *image,
                                     const gchar         *undo_desc,
                                     GimpUndoType         type,
                                     GimpChannel         *channel,
                                     gint                 prev_position,
                                     GimpChannel         *prev_channel);
static gboolean   undo_pop_channel  (GimpUndo            *undo,
                                     GimpUndoMode         undo_mode,
                                     GimpUndoAccumulator *accum);
static void       undo_free_channel (GimpUndo            *undo,
                                     GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_channel_add (GimpImage   *image,
                                  const gchar *undo_desc,
                                  GimpChannel *channel,
                                  gint         prev_position,
                                  GimpChannel *prev_channel)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), NULL);
  g_return_val_if_fail (! gimp_item_is_attached (GIMP_ITEM (channel)), NULL);
  g_return_val_if_fail (prev_channel == NULL || GIMP_IS_CHANNEL (prev_channel),
                        NULL);

  return undo_push_channel (image, undo_desc, GIMP_UNDO_CHANNEL_ADD,
                            channel, prev_position, prev_channel);
}

GimpUndo *
gimp_image_undo_push_channel_remove (GimpImage   *image,
                                     const gchar *undo_desc,
                                     GimpChannel *channel,
                                     gint         prev_position,
                                     GimpChannel *prev_channel)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (channel)), NULL);
  g_return_val_if_fail (prev_channel == NULL || GIMP_IS_CHANNEL (prev_channel),
                        NULL);

  return undo_push_channel (image, undo_desc, GIMP_UNDO_CHANNEL_REMOVE,
                            channel, prev_position, prev_channel);
}

static GimpUndo *
undo_push_channel (GimpImage    *image,
                   const gchar  *undo_desc,
                   GimpUndoType  type,
                   GimpChannel  *channel,
                   gint          prev_position,
                   GimpChannel  *prev_channel)
{
  GimpUndo *new;
  gint64    size;

  size = sizeof (ChannelUndo);

  if (type == GIMP_UNDO_CHANNEL_REMOVE)
    size += gimp_object_get_memsize (GIMP_OBJECT (channel), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (ChannelUndo),
                                   type, undo_desc,
                                   GIMP_DIRTY_IMAGE_STRUCTURE,
                                   undo_pop_channel,
                                   undo_free_channel,
                                   "item", channel,
                                   NULL)))
    {
      ChannelUndo *cu = new->data;

      cu->prev_position = prev_position;
      cu->prev_channel  = prev_channel;

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_channel (GimpUndo            *undo,
                  GimpUndoMode         undo_mode,
                  GimpUndoAccumulator *accum)
{
  ChannelUndo *cu      = undo->data;
  GimpChannel *channel = GIMP_CHANNEL (GIMP_ITEM_UNDO (undo)->item);

  if ((undo_mode       == GIMP_UNDO_MODE_UNDO &&
       undo->undo_type == GIMP_UNDO_CHANNEL_ADD) ||
      (undo_mode       == GIMP_UNDO_MODE_REDO &&
       undo->undo_type == GIMP_UNDO_CHANNEL_REMOVE))
    {
      /*  remove channel  */

      undo->size += gimp_object_get_memsize (GIMP_OBJECT (channel), NULL);

      /*  record the current position  */
      cu->prev_position = gimp_image_get_channel_index (undo->image,
                                                        channel);

      gimp_container_remove (undo->image->channels, GIMP_OBJECT (channel));
      gimp_item_removed (GIMP_ITEM (channel));

      if (channel == gimp_image_get_active_channel (undo->image))
        {
          if (cu->prev_channel)
            gimp_image_set_active_channel (undo->image, cu->prev_channel);
          else
            gimp_image_unset_active_channel (undo->image);
        }
    }
  else
    {
      /*  restore channel  */

      undo->size -= gimp_object_get_memsize (GIMP_OBJECT (channel), NULL);

      /*  record the active channel  */
      cu->prev_channel = gimp_image_get_active_channel (undo->image);

      gimp_container_insert (undo->image->channels,
                             GIMP_OBJECT (channel), cu->prev_position);
      gimp_image_set_active_channel (undo->image, channel);

      GIMP_ITEM (channel)->removed = FALSE;
    }

  return TRUE;
}

static void
undo_free_channel (GimpUndo     *undo,
                   GimpUndoMode  undo_mode)
{
  g_free (undo->data);
}


/******************************/
/*  Channel re-position Undo  */
/******************************/

GimpUndo *
gimp_image_undo_push_channel_reposition (GimpImage   *image,
                                         const gchar *undo_desc,
                                         GimpChannel *channel)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (channel)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_CHANNEL_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_CHANNEL_REPOSITION, undo_desc,
                               GIMP_DIRTY_IMAGE_STRUCTURE,
                               NULL, NULL,
                               "item", channel,
                               NULL);
}


/************************/
/*  Channel color Undo  */
/************************/

GimpUndo *
gimp_image_undo_push_channel_color (GimpImage   *image,
                                    const gchar *undo_desc,
                                    GimpChannel *channel)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_CHANNEL (channel), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (channel)), NULL);

  return gimp_image_undo_push (image, GIMP_TYPE_CHANNEL_PROP_UNDO,
                               0, 0,
                               GIMP_UNDO_CHANNEL_COLOR, undo_desc,
                               GIMP_DIRTY_ITEM | GIMP_DIRTY_DRAWABLE,
                               NULL, NULL,
                               "item", channel,
                               NULL);
}


/*****************************/
/*  Add/Remove Vectors Undo  */
/*****************************/

typedef struct _VectorsUndo VectorsUndo;

struct _VectorsUndo
{
  gint         prev_position;   /*  former position in list     */
  GimpVectors *prev_vectors;    /*  previous active vectors     */
};

static GimpUndo * undo_push_vectors (GimpImage           *image,
                                     const gchar         *undo_desc,
                                     GimpUndoType         type,
                                     GimpVectors         *vectors,
                                     gint                 prev_position,
                                     GimpVectors         *prev_vectors);
static gboolean   undo_pop_vectors  (GimpUndo            *undo,
                                     GimpUndoMode         undo_mode,
                                     GimpUndoAccumulator *accum);
static void       undo_free_vectors (GimpUndo            *undo,
                                     GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_vectors_add (GimpImage   *image,
                                  const gchar *undo_desc,
                                  GimpVectors *vectors,
                                  gint         prev_position,
                                  GimpVectors *prev_vectors)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_VECTORS (vectors), NULL);
  g_return_val_if_fail (! gimp_item_is_attached (GIMP_ITEM (vectors)), NULL);
  g_return_val_if_fail (prev_vectors == NULL || GIMP_IS_VECTORS (prev_vectors),
                        NULL);

  return undo_push_vectors (image, undo_desc, GIMP_UNDO_VECTORS_ADD,
                            vectors, prev_position, prev_vectors);
}

GimpUndo *
gimp_image_undo_push_vectors_remove (GimpImage   *image,
                                     const gchar *undo_desc,
                                     GimpVectors *vectors,
                                     gint         prev_position,
                                     GimpVectors *prev_vectors)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_VECTORS (vectors), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (vectors)), NULL);
  g_return_val_if_fail (prev_vectors == NULL || GIMP_IS_VECTORS (prev_vectors),
                        NULL);

  return undo_push_vectors (image, undo_desc, GIMP_UNDO_VECTORS_REMOVE,
                            vectors, prev_position, prev_vectors);
}

static GimpUndo *
undo_push_vectors (GimpImage    *image,
                   const gchar  *undo_desc,
                   GimpUndoType  type,
                   GimpVectors  *vectors,
                   gint          prev_position,
                   GimpVectors  *prev_vectors)
{
  GimpUndo *new;
  gint64    size;

  size = sizeof (VectorsUndo);

  if (type == GIMP_UNDO_VECTORS_REMOVE)
    size += gimp_object_get_memsize (GIMP_OBJECT (vectors), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (VectorsUndo),
                                   type, undo_desc,
                                   GIMP_DIRTY_IMAGE_STRUCTURE,
                                   undo_pop_vectors,
                                   undo_free_vectors,
                                   "item", vectors,
                                   NULL)))
    {
      VectorsUndo *vu = new->data;

      vu->prev_position = prev_position;
      vu->prev_vectors  = prev_vectors;

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_vectors (GimpUndo            *undo,
                  GimpUndoMode         undo_mode,
                  GimpUndoAccumulator *accum)
{
  VectorsUndo *vu      = undo->data;
  GimpVectors *vectors = GIMP_VECTORS (GIMP_ITEM_UNDO (undo)->item);

  if ((undo_mode       == GIMP_UNDO_MODE_UNDO &&
       undo->undo_type == GIMP_UNDO_VECTORS_ADD) ||
      (undo_mode       == GIMP_UNDO_MODE_REDO &&
       undo->undo_type == GIMP_UNDO_VECTORS_REMOVE))
    {
      /*  remove vectors  */

      undo->size += gimp_object_get_memsize (GIMP_OBJECT (vectors), NULL);

      /*  record the current position  */
      vu->prev_position = gimp_image_get_vectors_index (undo->image,
                                                        vectors);

      gimp_container_remove (undo->image->vectors, GIMP_OBJECT (vectors));
      gimp_item_removed (GIMP_ITEM (vectors));

      if (vectors == gimp_image_get_active_vectors (undo->image))
        gimp_image_set_active_vectors (undo->image, vu->prev_vectors);
    }
  else
    {
      /*  restore vectors  */

      undo->size -= gimp_object_get_memsize (GIMP_OBJECT (vectors), NULL);

      /*  record the active vectors  */
      vu->prev_vectors = gimp_image_get_active_vectors (undo->image);

      gimp_container_insert (undo->image->vectors,
                             GIMP_OBJECT (vectors), vu->prev_position);
      gimp_image_set_active_vectors (undo->image, vectors);

      GIMP_ITEM (vectors)->removed = FALSE;
    }

  return TRUE;
}

static void
undo_free_vectors (GimpUndo     *undo,
                   GimpUndoMode  undo_mode)
{
  g_free (undo->data);
}


/**********************/
/*  Vectors Mod Undo  */
/**********************/

typedef struct _VectorsModUndo VectorsModUndo;

struct _VectorsModUndo
{
  GimpVectors *vectors;
};

static gboolean undo_pop_vectors_mod  (GimpUndo            *undo,
                                       GimpUndoMode         undo_mode,
                                       GimpUndoAccumulator *accum);
static void     undo_free_vectors_mod (GimpUndo            *undo,
                                       GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_vectors_mod (GimpImage   *image,
                                  const gchar *undo_desc,
                                  GimpVectors *vectors)
{
  GimpVectors *copy;
  GimpUndo    *new;
  gint64       size;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_VECTORS (vectors), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (vectors)), NULL);

  copy = GIMP_VECTORS (gimp_item_duplicate (GIMP_ITEM (vectors),
                                            G_TYPE_FROM_INSTANCE (vectors),
                                            FALSE));

  size = (sizeof (VectorsModUndo) +
          gimp_object_get_memsize (GIMP_OBJECT (copy), NULL));

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   size, sizeof (VectorsModUndo),
                                   GIMP_UNDO_VECTORS_MOD, undo_desc,
                                   GIMP_DIRTY_ITEM | GIMP_DIRTY_VECTORS,
                                   undo_pop_vectors_mod,
                                   undo_free_vectors_mod,
                                   "item", vectors,
                                   NULL)))
    {
      VectorsModUndo *vmu = new->data;

      vmu->vectors = copy;

      return new;
    }

  if (copy)
    g_object_unref (copy);

  return NULL;
}

static gboolean
undo_pop_vectors_mod (GimpUndo            *undo,
                      GimpUndoMode         undo_mode,
                      GimpUndoAccumulator *accum)
{
  VectorsModUndo *vmu     = undo->data;
  GimpVectors    *vectors = GIMP_VECTORS (GIMP_ITEM_UNDO (undo)->item);
  GimpVectors    *temp;

  undo->size -= gimp_object_get_memsize (GIMP_OBJECT (vmu->vectors), NULL);

  temp = vmu->vectors;

  vmu->vectors =
    GIMP_VECTORS (gimp_item_duplicate (GIMP_ITEM (vectors),
                                       G_TYPE_FROM_INSTANCE (vectors),
                                       FALSE));

  gimp_vectors_freeze (vectors);

  gimp_vectors_copy_strokes (temp, vectors);

  GIMP_ITEM (vectors)->width    = GIMP_ITEM (temp)->width;
  GIMP_ITEM (vectors)->height   = GIMP_ITEM (temp)->height;
  GIMP_ITEM (vectors)->offset_x = GIMP_ITEM (temp)->offset_x;
  GIMP_ITEM (vectors)->offset_y = GIMP_ITEM (temp)->offset_y;

  g_object_unref (temp);

  gimp_vectors_thaw (vectors);

  undo->size += gimp_object_get_memsize (GIMP_OBJECT (vmu->vectors), NULL);

  return TRUE;
}

static void
undo_free_vectors_mod (GimpUndo     *undo,
                       GimpUndoMode  undo_mode)
{
  VectorsModUndo *vmu = undo->data;

  g_object_unref (vmu->vectors);
  g_free (vmu);
}


/******************************/
/*  Vectors re-position Undo  */
/******************************/

typedef struct _VectorsRepositionUndo VectorsRepositionUndo;

struct _VectorsRepositionUndo
{
  gint old_position;
};

static gboolean undo_pop_vectors_reposition  (GimpUndo            *undo,
                                              GimpUndoMode         undo_mode,
                                              GimpUndoAccumulator *accum);
static void     undo_free_vectors_reposition (GimpUndo            *undo,
                                              GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_vectors_reposition (GimpImage   *image,
                                         const gchar *undo_desc,
                                         GimpVectors *vectors)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_VECTORS (vectors), NULL);
  g_return_val_if_fail (gimp_item_is_attached (GIMP_ITEM (vectors)), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   sizeof (VectorsRepositionUndo),
                                   sizeof (VectorsRepositionUndo),
                                   GIMP_UNDO_VECTORS_REPOSITION, undo_desc,
                                   GIMP_DIRTY_IMAGE_STRUCTURE,
                                   undo_pop_vectors_reposition,
                                   undo_free_vectors_reposition,
                                   "item", vectors,
                                   NULL)))
    {
      VectorsRepositionUndo *vru = new->data;

      vru->old_position = gimp_image_get_vectors_index (image, vectors);

      return new;
    }

  return NULL;
}

static gboolean
undo_pop_vectors_reposition (GimpUndo            *undo,
                             GimpUndoMode         undo_mode,
                             GimpUndoAccumulator *accum)
{
  VectorsRepositionUndo *vru     = undo->data;
  GimpVectors           *vectors = GIMP_VECTORS (GIMP_ITEM_UNDO (undo)->item);
  gint                   pos;

  /* what's the vectors's current index? */
  pos = gimp_image_get_vectors_index (undo->image, vectors);
  gimp_image_position_vectors (undo->image, vectors, vru->old_position,
                               FALSE, NULL);
  vru->old_position = pos;

  return TRUE;
}

static void
undo_free_vectors_reposition (GimpUndo     *undo,
                              GimpUndoMode  undo_mode)
{
  g_free (undo->data);
}


/**************************************/
/*  Floating Selection to Layer Undo  */
/**************************************/

typedef struct _FStoLayerUndo FStoLayerUndo;

struct _FStoLayerUndo
{
  GimpLayer    *floating_layer; /*  the floating layer        */
  GimpDrawable *drawable;       /*  drawable of floating sel  */
};

static gboolean undo_pop_fs_to_layer  (GimpUndo            *undo,
                                       GimpUndoMode         undo_mode,
                                       GimpUndoAccumulator *accum);
static void     undo_free_fs_to_layer (GimpUndo            *undo,
                                       GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_fs_to_layer (GimpImage    *image,
                                  const gchar  *undo_desc,
                                  GimpLayer    *floating_layer,
                                  GimpDrawable *drawable)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (floating_layer), NULL);
  g_return_val_if_fail (GIMP_IS_DRAWABLE (drawable), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (FStoLayerUndo),
                                   sizeof (FStoLayerUndo),
                                   GIMP_UNDO_FS_TO_LAYER, undo_desc,
                                   GIMP_DIRTY_IMAGE_STRUCTURE,
                                   undo_pop_fs_to_layer,
                                   undo_free_fs_to_layer,
                                   NULL)))
    {
      FStoLayerUndo *fsu = new->data;

      fsu->floating_layer = floating_layer;
      fsu->drawable       = drawable;

      return new;
    }

  tile_manager_unref (floating_layer->fs.backing_store);
  floating_layer->fs.backing_store = NULL;

  return NULL;
}

static gboolean
undo_pop_fs_to_layer (GimpUndo            *undo,
                      GimpUndoMode         undo_mode,
                      GimpUndoAccumulator *accum)
{
  FStoLayerUndo *fsu = undo->data;

  switch (undo_mode)
    {
    case GIMP_UNDO_MODE_UNDO:
      /*  Update the preview for the floating sel  */
      gimp_viewable_invalidate_preview (GIMP_VIEWABLE (fsu->floating_layer));

      fsu->floating_layer->fs.drawable = fsu->drawable;
      gimp_image_set_active_layer (undo->image, fsu->floating_layer);
      undo->image->floating_sel = fsu->floating_layer;

      /*  store the contents of the drawable  */
      floating_sel_store (fsu->floating_layer,
                          GIMP_ITEM (fsu->floating_layer)->offset_x,
                          GIMP_ITEM (fsu->floating_layer)->offset_y,
                          GIMP_ITEM (fsu->floating_layer)->width,
                          GIMP_ITEM (fsu->floating_layer)->height);
      fsu->floating_layer->fs.initial = TRUE;

      /*  clear the selection  */
      gimp_drawable_invalidate_boundary (GIMP_DRAWABLE (fsu->floating_layer));
      break;

    case GIMP_UNDO_MODE_REDO:
      /*  restore the contents of the drawable  */
      floating_sel_restore (fsu->floating_layer,
                            GIMP_ITEM (fsu->floating_layer)->offset_x,
                            GIMP_ITEM (fsu->floating_layer)->offset_y,
                            GIMP_ITEM (fsu->floating_layer)->width,
                            GIMP_ITEM (fsu->floating_layer)->height);

      /*  Update the preview for the underlying drawable  */
      gimp_viewable_invalidate_preview (GIMP_VIEWABLE (fsu->floating_layer));

      /*  clear the selection  */
      gimp_drawable_invalidate_boundary (GIMP_DRAWABLE (fsu->floating_layer));

      /*  update the pointers  */
      fsu->floating_layer->fs.drawable = NULL;
      undo->image->floating_sel       = NULL;
      break;
    }

  gimp_object_name_changed (GIMP_OBJECT (fsu->floating_layer));

  gimp_drawable_update (GIMP_DRAWABLE (fsu->floating_layer),
                        0, 0,
                        GIMP_ITEM (fsu->floating_layer)->width,
                        GIMP_ITEM (fsu->floating_layer)->height);

  gimp_image_floating_selection_changed (undo->image);

  return TRUE;
}

static void
undo_free_fs_to_layer (GimpUndo     *undo,
                       GimpUndoMode  undo_mode)
{
  FStoLayerUndo *fsu = undo->data;

  if (undo_mode == GIMP_UNDO_MODE_UNDO)
    {
      tile_manager_unref (fsu->floating_layer->fs.backing_store);
      fsu->floating_layer->fs.backing_store = NULL;
    }

  g_free (fsu);
}


/***********************************/
/*  Floating Selection Rigor Undo  */
/***********************************/

static gboolean undo_pop_fs_rigor (GimpUndo            *undo,
                                   GimpUndoMode         undo_mode,
                                   GimpUndoAccumulator *accum);

GimpUndo *
gimp_image_undo_push_fs_rigor (GimpImage    *image,
                               const gchar  *undo_desc,
                               GimpLayer    *floating_layer)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (floating_layer), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   0, 0,
                                   GIMP_UNDO_FS_RIGOR, undo_desc,
                                   GIMP_DIRTY_NONE,
                                   undo_pop_fs_rigor,
                                   NULL,
                                   "item", floating_layer,
                                   NULL)))
    {
      return new;
    }

  return NULL;
}

static gboolean
undo_pop_fs_rigor (GimpUndo            *undo,
                   GimpUndoMode         undo_mode,
                   GimpUndoAccumulator *accum)
{
  GimpLayer *floating_layer;

  floating_layer = GIMP_LAYER (GIMP_ITEM_UNDO (undo)->item);

  if (! gimp_layer_is_floating_sel (floating_layer))
    return FALSE;

  switch (undo_mode)
    {
    case GIMP_UNDO_MODE_UNDO:
      floating_sel_relax (floating_layer, FALSE);
      break;

    case GIMP_UNDO_MODE_REDO:
      floating_sel_rigor (floating_layer, FALSE);
      break;
    }

  return TRUE;
}


/***********************************/
/*  Floating Selection Relax Undo  */
/***********************************/

static gboolean undo_pop_fs_relax (GimpUndo            *undo,
                                   GimpUndoMode         undo_mode,
                                   GimpUndoAccumulator *accum);

GimpUndo *
gimp_image_undo_push_fs_relax (GimpImage   *image,
                               const gchar *undo_desc,
                               GimpLayer   *floating_layer)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_LAYER (floating_layer), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_ITEM_UNDO,
                                   0, 0,
                                   GIMP_UNDO_FS_RELAX, undo_desc,
                                   GIMP_DIRTY_NONE,
                                   undo_pop_fs_relax,
                                   NULL,
                                   "item", floating_layer,
                                   NULL)))
    {
      return new;
    }

  return NULL;
}

static gboolean
undo_pop_fs_relax (GimpUndo            *undo,
                   GimpUndoMode         undo_mode,
                   GimpUndoAccumulator *accum)
{
  GimpLayer *floating_layer;

  floating_layer = GIMP_LAYER (GIMP_ITEM_UNDO (undo)->item);

  if (! gimp_layer_is_floating_sel (floating_layer))
    return FALSE;

  switch (undo_mode)
    {
    case GIMP_UNDO_MODE_UNDO:
      floating_sel_rigor (floating_layer, FALSE);
      break;

    case GIMP_UNDO_MODE_REDO:
      floating_sel_relax (floating_layer, FALSE);
      break;
    }

  return TRUE;
}


/*******************/
/*  Parasite Undo  */
/*******************/

typedef struct _ParasiteUndo ParasiteUndo;

struct _ParasiteUndo
{
  GimpImage    *image;
  GimpItem     *item;
  GimpParasite *parasite;
  gchar        *name;
};

static gboolean undo_pop_parasite  (GimpUndo            *undo,
                                    GimpUndoMode         undo_mode,
                                    GimpUndoAccumulator *accum);
static void     undo_free_parasite (GimpUndo            *undo,
                                    GimpUndoMode         undo_mode);

GimpUndo *
gimp_image_undo_push_image_parasite (GimpImage          *image,
                                     const gchar        *undo_desc,
                                     const GimpParasite *parasite)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (ParasiteUndo),
                                   sizeof (ParasiteUndo),
                                   GIMP_UNDO_PARASITE_ATTACH, undo_desc,
                                   GIMP_DIRTY_IMAGE_META,
                                   undo_pop_parasite,
                                   undo_free_parasite,
                                   NULL)))
    {
      ParasiteUndo *pu = new->data;

      pu->image    = image;
      pu->item     = NULL;
      pu->name     = g_strdup (gimp_parasite_name (parasite));
      pu->parasite = gimp_parasite_copy (gimp_image_parasite_find (image,
                                                                   pu->name));

      return new;
    }

  return NULL;
}

GimpUndo *
gimp_image_undo_push_image_parasite_remove (GimpImage   *image,
                                            const gchar *undo_desc,
                                            const gchar *name)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (ParasiteUndo),
                                   sizeof (ParasiteUndo),
                                   GIMP_UNDO_PARASITE_REMOVE, undo_desc,
                                   GIMP_DIRTY_IMAGE_META,
                                   undo_pop_parasite,
                                   undo_free_parasite,
                                   NULL)))
    {
      ParasiteUndo *pu = new->data;

      pu->image    = image;
      pu->item     = NULL;
      pu->name     = g_strdup (name);
      pu->parasite = gimp_parasite_copy (gimp_image_parasite_find (image,
                                                                   pu->name));

      return new;
    }

  return NULL;
}

GimpUndo *
gimp_image_undo_push_item_parasite (GimpImage          *image,
                                    const gchar        *undo_desc,
                                    GimpItem           *item,
                                    const GimpParasite *parasite)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_ITEM (item), NULL);
  g_return_val_if_fail (gimp_item_is_attached (item), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (ParasiteUndo),
                                   sizeof (ParasiteUndo),
                                   GIMP_UNDO_PARASITE_ATTACH, undo_desc,
                                   GIMP_DIRTY_ITEM_META,
                                   undo_pop_parasite,
                                   undo_free_parasite,
                                   NULL)))
    {
      ParasiteUndo *pu = new->data;

      pu->image    = NULL;
      pu->item     = item;
      pu->name     = g_strdup (gimp_parasite_name (parasite));
      pu->parasite = gimp_parasite_copy (gimp_item_parasite_find (item,
                                                                  pu->name));
      return new;
    }

  return NULL;
}

GimpUndo *
gimp_image_undo_push_item_parasite_remove (GimpImage   *image,
                                           const gchar *undo_desc,
                                           GimpItem    *item,
                                           const gchar *name)
{
  GimpUndo *new;

  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);
  g_return_val_if_fail (GIMP_IS_ITEM (item), NULL);
  g_return_val_if_fail (gimp_item_is_attached (item), NULL);

  if ((new = gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                                   sizeof (ParasiteUndo),
                                   sizeof (ParasiteUndo),
                                   GIMP_UNDO_PARASITE_REMOVE, undo_desc,
                                   GIMP_DIRTY_ITEM_META,
                                   undo_pop_parasite,
                                   undo_free_parasite,
                                   NULL)))
    {
      ParasiteUndo *pu = new->data;

      pu->image    = NULL;
      pu->item     = item;
      pu->name     = g_strdup (name);
      pu->parasite = gimp_parasite_copy (gimp_item_parasite_find (item,
                                                                  pu->name));
      return new;
    }

  return NULL;
}

static gboolean
undo_pop_parasite (GimpUndo            *undo,
                   GimpUndoMode         undo_mode,
                   GimpUndoAccumulator *accum)
{
  ParasiteUndo *pu = undo->data;
  GimpParasite *tmp;

  tmp = pu->parasite;

  if (pu->image)
    {
      pu->parasite = gimp_parasite_copy (gimp_image_parasite_find (undo->image,
                                                                   pu->name));
      if (tmp)
        gimp_parasite_list_add (pu->image->parasites, tmp);
      else
        gimp_parasite_list_remove (pu->image->parasites, pu->name);
    }
  else if (pu->item)
    {
      pu->parasite = gimp_parasite_copy (gimp_item_parasite_find (pu->item,
                                                                  pu->name));
      if (tmp)
        gimp_parasite_list_add (pu->item->parasites, tmp);
      else
        gimp_parasite_list_remove (pu->item->parasites, pu->name);
    }
  else
    {
      pu->parasite = gimp_parasite_copy (gimp_parasite_find (undo->image->gimp,
                                                             pu->name));
      if (tmp)
        gimp_parasite_attach (undo->image->gimp, tmp);
      else
        gimp_parasite_detach (undo->image->gimp, pu->name);
    }

  if (tmp)
    gimp_parasite_free (tmp);

  return TRUE;
}

static void
undo_free_parasite (GimpUndo     *undo,
                    GimpUndoMode  undo_mode)
{
  ParasiteUndo *pu = undo->data;

  if (pu->parasite)
    gimp_parasite_free (pu->parasite);
  if (pu->name)
    g_free (pu->name);

  g_free (pu);
}


/******************************************************************************/
/*  Something for which programmer is too lazy to write an undo function for  */
/******************************************************************************/

static gboolean undo_pop_cantundo (GimpUndo            *undo,
                                   GimpUndoMode         undo_mode,
                                   GimpUndoAccumulator *accum);

GimpUndo *
gimp_image_undo_push_cantundo (GimpImage   *image,
                               const gchar *undo_desc)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  /* This is the sole purpose of this type of undo: the ability to
   * mark an image as having been mutated, without really providing
   * any adequate undo facility.
   */

  return gimp_image_undo_push (image, GIMP_TYPE_UNDO,
                               0, 0,
                               GIMP_UNDO_CANT, undo_desc,
                               GIMP_DIRTY_ALL,
                               undo_pop_cantundo,
                               NULL,
                               NULL);
}

static gboolean
undo_pop_cantundo (GimpUndo            *undo,
                   GimpUndoMode         undo_mode,
                   GimpUndoAccumulator *accum)
{
  switch (undo_mode)
    {
    case GIMP_UNDO_MODE_UNDO:
      gimp_message (undo->image->gimp, NULL, GIMP_MESSAGE_WARNING,
                    _("Can't undo %s"), GIMP_OBJECT (undo)->name);
      break;

    case GIMP_UNDO_MODE_REDO:
      break;
    }

  return TRUE;
}
