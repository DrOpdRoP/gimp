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

#include <glib-object.h>

#include "core-types.h"

#include "gimp.h"
#include "gimpimage.h"
#include "gimpimage-projection.h"
#include "gimpimage-rotate.h"
#include "gimpimage-guides.h"
#include "gimpimage-undo.h"
#include "gimpimage-undo-push.h"
#include "gimpitem.h"
#include "gimplayer-floating-sel.h"
#include "gimplist.h"


static void  gimp_image_rotate_item_offset (GimpImage        *gimage,
					    GimpRotationType  rotate_type,
					    GimpItem         *item,
					    gint              off_x,
					    gint              off_y);
static void  gimp_image_rotate_guides      (GimpImage        *gimage,
					    GimpRotationType  rotate_type);


void
gimp_image_rotate (GimpImage        *gimage,
                   GimpRotationType  rotate_type,
                   GimpProgressFunc  progress_func,
                   gpointer          progress_data)
{
  GimpLayer *floating_layer;
  GimpItem  *item;
  GList     *list;
  gdouble    center_x;
  gdouble    center_y;
  gint       progress_max;
  gint       progress_current = 1;
  gint       new_image_width;
  gint       new_image_height;
  gboolean   size_changed;

  g_return_if_fail (GIMP_IS_IMAGE (gimage));

  gimp_set_busy (gimage->gimp);

  center_x = (gdouble) gimage->width  / 2.0;
  center_y = (gdouble) gimage->height / 2.0;

  progress_max = (gimage->channels->num_children +
                  gimage->layers->num_children   +
                  gimage->vectors->num_children  +
                  1 /* selection */);

  /*  Get the floating layer if one exists  */
  floating_layer = gimp_image_floating_sel (gimage);

  gimp_image_undo_group_start (gimage, GIMP_UNDO_GROUP_IMAGE_ROTATE, NULL);

  /*  Relax the floating selection  */
  if (floating_layer)
    floating_sel_relax (floating_layer, TRUE);

  /*  Resize the image (if needed)  */
  switch (rotate_type)
    {
    case GIMP_ROTATE_90:
    case GIMP_ROTATE_270:
      new_image_width  = gimage->height;
      new_image_height = gimage->width;
      size_changed     = TRUE;
      break;

    case GIMP_ROTATE_180:
      new_image_width  = gimage->width;
      new_image_height = gimage->height;
      size_changed     = FALSE;
     break;

    default:
      g_assert_not_reached ();
      return;
    }

  /*  Rotate all channels  */
  for (list = GIMP_LIST (gimage->channels)->list;
       list;
       list = g_list_next (list))
    {
      item = (GimpItem *) list->data;

      gimp_item_rotate (item, rotate_type, center_x, center_y, FALSE);

      item->offset_x = 0;
      item->offset_y = 0;

      if (progress_func)
        (* progress_func) (0, progress_max, progress_current++, progress_data);
    }

  /*  Rotate all vectors  */
  for (list = GIMP_LIST (gimage->vectors)->list;
       list;
       list = g_list_next (list))
    {
      item = (GimpItem *) list->data;

      gimp_item_rotate (item, rotate_type, center_x, center_y, FALSE);

      item->width    = new_image_width;
      item->height   = new_image_height;
      item->offset_x = 0;
      item->offset_y = 0;

      gimp_item_translate (item,
			   (new_image_width  - gimage->width)  / 2,
			   (new_image_height - gimage->height) / 2,
			   FALSE);

      if (progress_func)
        (* progress_func) (0, progress_max, progress_current++, progress_data);
    }

  /*  Don't forget the selection mask!  */
  gimp_item_rotate (GIMP_ITEM (gimp_image_get_mask (gimage)),
                    rotate_type, center_x, center_y, FALSE);

  GIMP_ITEM (gimage->selection_mask)->offset_x = 0;
  GIMP_ITEM (gimage->selection_mask)->offset_y = 0;

  if (progress_func)
    (* progress_func) (0, progress_max, progress_current++, progress_data);

  /*  Rotate all layers  */
  for (list = GIMP_LIST (gimage->layers)->list;
       list;
       list = g_list_next (list))
    {
      gint off_x, off_y;

      item = (GimpItem *) list->data;

      gimp_item_offsets (item, &off_x, &off_y);

      gimp_item_rotate (item, rotate_type, center_x, center_y, FALSE);

      gimp_image_rotate_item_offset (gimage, rotate_type, item, off_x, off_y);

      if (progress_func)
        (* progress_func) (0, progress_max, progress_current++, progress_data);
    }

  /*  Rotate all Guides  */
  gimp_image_rotate_guides (gimage, rotate_type);

  /*  Resize the image (if needed)  */
  if (size_changed)
    {
      gimp_image_undo_push_image_size (gimage, NULL);

      gimage->width  = new_image_width;
      gimage->height = new_image_height;

      if (gimage->xresolution != gimage->yresolution)
        {
          gdouble tmp;

          gimp_image_undo_push_image_resolution (gimage, NULL);

          tmp                 = gimage->xresolution;
          gimage->yresolution = gimage->xresolution;
          gimage->xresolution = tmp;
        }
    }

  /*  Make sure the projection matches the gimage size  */
  gimp_image_projection_allocate (gimage);

  /*  Rigor the floating selection  */
  if (floating_layer)
    floating_sel_rigor (floating_layer, TRUE);

  gimp_image_undo_group_end (gimage);

  if (size_changed)
    gimp_viewable_size_changed (GIMP_VIEWABLE (gimage));

  gimp_unset_busy (gimage->gimp);
}


static void
gimp_image_rotate_item_offset (GimpImage        *gimage,
			       GimpRotationType  rotate_type,
			       GimpItem         *item,
			       gint              off_x,
			       gint              off_y)
{
  gint x = 0;
  gint y = 0;

  switch (rotate_type)
    {
    case GIMP_ROTATE_90:
      x = gimage->height - off_y - gimp_item_width (item);
      y = off_x;
      break;

    case GIMP_ROTATE_270:
      x = off_y;
      y = gimage->width - off_x - gimp_item_height (item);
      break;

    case GIMP_ROTATE_180:
      return;
    }

  gimp_item_offsets (item, &off_x, &off_y);

  x -= off_x;
  y -= off_y;

  if (x || y)
    gimp_item_translate (item, x, y, FALSE);
}

static void
gimp_image_rotate_guides (GimpImage        *gimage,
                          GimpRotationType  rotate_type)
{
  GList *list;

  /*  Rotate all Guides  */
  for (list = gimage->guides; list; list = g_list_next (list))
    {
      GimpGuide *guide = list->data;

      switch (rotate_type)
        {
        case GIMP_ROTATE_90:
          switch (guide->orientation)
            {
            case GIMP_ORIENTATION_HORIZONTAL:
              gimp_image_undo_push_image_guide (gimage, NULL, guide);
              guide->orientation = GIMP_ORIENTATION_VERTICAL;
              guide->position    = gimage->height - guide->position;
              break;

            case GIMP_ORIENTATION_VERTICAL:
              gimp_image_undo_push_image_guide (gimage, NULL, guide);
              guide->orientation = GIMP_ORIENTATION_HORIZONTAL;
              break;

            default:
              break;
            }
          break;

        case GIMP_ROTATE_180:
          switch (guide->orientation)
            {
            case GIMP_ORIENTATION_HORIZONTAL:
              gimp_image_move_guide (gimage, guide,
                                     gimage->height - guide->position, TRUE);
              break;

            case GIMP_ORIENTATION_VERTICAL:
              gimp_image_move_guide (gimage, guide,
                                     gimage->width - guide->position, TRUE);
              break;

            default:
              break;
            }
          break;

        case GIMP_ROTATE_270:
          switch (guide->orientation)
            {
            case GIMP_ORIENTATION_HORIZONTAL:
              gimp_image_undo_push_image_guide (gimage, NULL, guide);
              guide->orientation = GIMP_ORIENTATION_VERTICAL;
              break;

            case GIMP_ORIENTATION_VERTICAL:
              gimp_image_undo_push_image_guide (gimage, NULL, guide);
              guide->orientation = GIMP_ORIENTATION_HORIZONTAL;
              guide->position    = gimage->width - guide->position;
              break;

            default:
              break;
            }
          break;
	}
    }
}
