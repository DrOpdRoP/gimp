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

#include "actions-types.h"

#include "core/gimpimage.h"
#include "core/gimplayer.h"
#include "core/gimplist.h"

#include "text/gimptextlayer.h"

#include "widgets/gimphelp-ids.h"
#include "widgets/gimpactiongroup.h"
#include "widgets/gimpitemtreeview.h"

#include "display/gimpdisplay.h"
#include "display/gimpdisplayshell.h"

#include "layers-actions.h"
#include "layers-commands.h"

#include "gimp-intl.h"


static GimpActionEntry layers_actions[] =
{
  { "layers-popup", GIMP_STOCK_LAYERS, N_("Layers Menu")   },

  { "layers-menu",              NULL, N_("_Layer")        },
  { "layers-stack-menu",        NULL, N_("Stac_k")        },
  { "layers-colors-menu",       NULL, N_("_Colors")       },
  { "layers-colors-auto-menu",  NULL, N_("_Auto")         },
  { "layers-mask-menu",         NULL, N_("_Mask")         },
  { "layers-transparency-menu", NULL, N_("Tr_ansparency") },
  { "layers-transform-menu",    NULL, N_("_Transform")    },

  { "layers-text-tool", GIMP_STOCK_TOOL_TEXT,
    N_("Te_xt Tool"), NULL, NULL,
    G_CALLBACK (layers_text_tool_cmd_callback),
    GIMP_HELP_TOOL_TEXT },

  { "layers-edit-attributes", GIMP_STOCK_EDIT,
    N_("_Edit Layer Attributes..."), NULL, NULL,
    G_CALLBACK (layers_edit_attributes_cmd_callback),
    GIMP_HELP_LAYER_EDIT },

  { "layers-new", GTK_STOCK_NEW,
    N_("_New Layer..."), "", NULL,
    G_CALLBACK (layers_new_cmd_callback),
    GIMP_HELP_LAYER_NEW },

  { "layers-duplicate", GIMP_STOCK_DUPLICATE,
    N_("D_uplicate Layer"), NULL, NULL,
    G_CALLBACK (layers_duplicate_cmd_callback),
    GIMP_HELP_LAYER_DUPLICATE },

  { "layers-delete", GTK_STOCK_DELETE,
    N_("_Delete Layer"), "", NULL,
    G_CALLBACK (layers_delete_cmd_callback),
    GIMP_HELP_LAYER_DELETE },

  { "layers-select-previous", NULL,
    N_("Select _Previous Layer"), "Prior", NULL,
    G_CALLBACK (layers_select_previous_cmd_callback),
    GIMP_HELP_LAYER_PREVIOUS },

  { "layers-select-top", NULL,
    N_("Select _Top Layer"), "Home", NULL,
    G_CALLBACK (layers_select_top_cmd_callback),
    GIMP_HELP_LAYER_TOP },

  { "layers-select-next", NULL,
    N_("Select _Next Layer"), "Next", NULL,
    G_CALLBACK (layers_select_next_cmd_callback),
    GIMP_HELP_LAYER_NEXT },

  { "layers-select-bottom", NULL,
    N_("Select _Bottom Layer"), "End", NULL,
    G_CALLBACK (layers_select_bottom_cmd_callback),
    GIMP_HELP_LAYER_BOTTOM },

  { "layers-raise", GTK_STOCK_GO_UP,
    N_("_Raise Layer"), "", NULL,
    G_CALLBACK (layers_raise_cmd_callback),
    GIMP_HELP_LAYER_RAISE },

  { "layers-raise-to-top", GTK_STOCK_GOTO_TOP,
    N_("Layer to _Top"), "", NULL,
    G_CALLBACK (layers_raise_to_top_cmd_callback),
    GIMP_HELP_LAYER_RAISE_TO_TOP },

  { "layers-lower", GTK_STOCK_GO_DOWN,
    N_("_Lower Layer"), "", NULL,
    G_CALLBACK (layers_lower_cmd_callback),
    GIMP_HELP_LAYER_LOWER },

  { "layers-lower-to-bottom", GTK_STOCK_GOTO_BOTTOM,
    N_("Layer to _Bottom"), "", NULL,
    G_CALLBACK (layers_lower_to_bottom_cmd_callback),
    GIMP_HELP_LAYER_LOWER_TO_BOTTOM },

  { "layers-anchor", GIMP_STOCK_ANCHOR,
    N_("_Anchor Layer"), NULL, NULL,
    G_CALLBACK (layers_anchor_cmd_callback),
    GIMP_HELP_LAYER_ANCHOR },

  { "layers-merge-down", GIMP_STOCK_MERGE_DOWN,
    N_("Merge Do_wn"), NULL, NULL,
    G_CALLBACK (layers_merge_down_cmd_callback),
    GIMP_HELP_LAYER_MERGE_DOWN },

  { "layers-merge-layers", NULL,
    N_("Merge _Visible Layers..."), NULL, NULL,
    G_CALLBACK (layers_merge_layers_cmd_callback),
    GIMP_HELP_IMAGE_MERGE_LAYERS },

  { "layers-flatten-image", NULL,
    N_("_Flatten Image"), NULL, NULL,
    G_CALLBACK (layers_flatten_image_cmd_callback),
    GIMP_HELP_IMAGE_FLATTEN },

  { "layers-text-discard", GIMP_STOCK_TOOL_TEXT,
    N_("_Discard Text Information"), NULL, NULL,
    G_CALLBACK (layers_text_discard_cmd_callback),
    GIMP_HELP_LAYER_TEXT_DISCARD },

  { "layers-resize", GIMP_STOCK_RESIZE,
    N_("Layer B_oundary Size..."), NULL, NULL,
    G_CALLBACK (layers_resize_cmd_callback),
    GIMP_HELP_LAYER_RESIZE },

  { "layers-resize-to-image", GIMP_STOCK_LAYER_TO_IMAGESIZE,
    N_("Layer to _Image Size"), NULL, NULL,
    G_CALLBACK (layers_resize_to_image_cmd_callback),
    GIMP_HELP_LAYER_RESIZE_TO_IMAGE },

  { "layers-scale", GIMP_STOCK_SCALE,
    N_("_Scale Layer..."), NULL, NULL,
    G_CALLBACK (layers_scale_cmd_callback),
    GIMP_HELP_LAYER_SCALE },

  { "layers-crop", GIMP_STOCK_TOOL_CROP,
    N_("Cr_op Layer"), NULL, NULL,
    G_CALLBACK (layers_crop_cmd_callback),
    GIMP_HELP_LAYER_CROP },

  { "layers-mask-add", GIMP_STOCK_LAYER_MASK,
    N_("Add La_yer Mask..."), NULL, NULL,
    G_CALLBACK (layers_mask_add_cmd_callback),
    GIMP_HELP_LAYER_MASK_ADD },

  { "layers-mask-apply", NULL,
    N_("Apply Layer _Mask"), NULL, NULL,
    G_CALLBACK (layers_mask_apply_cmd_callback),
    GIMP_HELP_LAYER_MASK_APPLY },

  { "layers-mask-delete", GTK_STOCK_DELETE,
    N_("Delete Layer Mas_k"), "", NULL,
    G_CALLBACK (layers_mask_delete_cmd_callback),
    GIMP_HELP_LAYER_MASK_DELETE },

  { "layers-alpha-add", GIMP_STOCK_TRANSPARENCY,
    N_("Add Alpha C_hannel"), NULL, NULL,
    G_CALLBACK (layers_alpha_add_cmd_callback),
    GIMP_HELP_LAYER_ALPHA_ADD }
};

static GimpEnumActionEntry layers_mask_to_selection_actions[] =
{
  { "layers-mask-selection-replace", GIMP_STOCK_SELECTION_REPLACE,
    N_("_Mask to Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_REPLACE,
    GIMP_HELP_LAYER_MASK_SELECTION_REPLACE },

  { "layers-mask-selection-add", GIMP_STOCK_SELECTION_ADD,
    N_("_Add to Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_ADD,
    GIMP_HELP_LAYER_MASK_SELECTION_ADD },

  { "layers-mask-selection-subtract", GIMP_STOCK_SELECTION_SUBTRACT,
    N_("_Subtract from Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_SUBTRACT,
    GIMP_HELP_LAYER_MASK_SELECTION_SUBTRACT },

  { "layers-mask-selection-intersect", GIMP_STOCK_SELECTION_INTERSECT,
    N_("_Intersect with Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_INTERSECT,
    GIMP_HELP_LAYER_MASK_SELECTION_INTERSECT }
};

static GimpEnumActionEntry layers_alpha_to_selection_actions[] =
{
  { "layers-alpha-selection-replace", GIMP_STOCK_SELECTION_REPLACE,
    N_("Al_pha to Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_REPLACE,
    GIMP_HELP_LAYER_ALPHA_SELECTION_REPLACE },

  { "layers-alpha-selection-add", GIMP_STOCK_SELECTION_ADD,
    N_("A_dd to Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_ADD,
    GIMP_HELP_LAYER_ALPHA_SELECTION_ADD },

  { "layers-alpha-selection-subtract", GIMP_STOCK_SELECTION_SUBTRACT,
    N_("_Subtract from Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_SUBTRACT,
    GIMP_HELP_LAYER_ALPHA_SELECTION_SUBTRACT },

  { "layers-alpha-selection-intersect", GIMP_STOCK_SELECTION_INTERSECT,
    N_("_Intersect with Selection"), NULL, NULL,
    GIMP_CHANNEL_OP_INTERSECT,
    GIMP_HELP_LAYER_ALPHA_SELECTION_INTERSECT }
};

void
layers_actions_setup (GimpActionGroup *group,
                      gpointer         data)
{
  gimp_action_group_add_actions (group,
                                 layers_actions,
                                 G_N_ELEMENTS (layers_actions),
                                 data);

  gimp_action_group_add_enum_actions (group,
                                      layers_mask_to_selection_actions,
                                      G_N_ELEMENTS (layers_mask_to_selection_actions),
                                      G_CALLBACK (layers_mask_to_selection_cmd_callback),
                                      data);

  gimp_action_group_add_enum_actions (group,
                                      layers_alpha_to_selection_actions,
                                      G_N_ELEMENTS (layers_alpha_to_selection_actions),
                                      G_CALLBACK (layers_alpha_to_selection_cmd_callback),
                                      data);
}

void
layers_actions_update (GimpActionGroup *group,
                       gpointer         data)
{
  GimpImage *gimage     = NULL;
  GimpLayer *layer      = NULL;
  gboolean   fs         = FALSE;    /*  floating sel           */
  gboolean   ac         = FALSE;    /*  active channel         */
  gboolean   lm         = FALSE;    /*  layer mask             */
  gboolean   alpha      = FALSE;    /*  alpha channel present  */
  gboolean   indexed    = FALSE;    /*  is indexed             */
  gboolean   next_alpha = FALSE;
  gboolean   text_layer = FALSE;
  GList     *next       = NULL;
  GList     *prev       = NULL;

  if (GIMP_IS_ITEM_TREE_VIEW (data))
    gimage = GIMP_ITEM_TREE_VIEW (data)->gimage;
  else if (GIMP_IS_DISPLAY_SHELL (data))
    gimage = GIMP_DISPLAY_SHELL (data)->gdisp->gimage;
  else if (GIMP_IS_DISPLAY (data))
    gimage = GIMP_DISPLAY (data)->gimage;

  if (gimage)
    {
      GList *list;

      layer = gimp_image_get_active_layer (gimage);

      if (layer)
        lm = (gimp_layer_get_mask (layer)) ? TRUE : FALSE;

      fs = (gimp_image_floating_sel (gimage) != NULL);
      ac = (gimp_image_get_active_channel (gimage) != NULL);

      alpha = layer && gimp_drawable_has_alpha (GIMP_DRAWABLE (layer));

      indexed = (gimp_image_base_type (gimage) == GIMP_INDEXED);

      for (list = GIMP_LIST (gimage->layers)->list;
           list;
           list = g_list_next (list))
        {
          if (layer == (GimpLayer *) list->data)
            {
              prev = g_list_previous (list);
              next = g_list_next (list);
              break;
            }
        }

      if (next)
        next_alpha = gimp_drawable_has_alpha (GIMP_DRAWABLE (next->data));
      else
        next_alpha = FALSE;

      text_layer = (layer &&
                    gimp_drawable_is_text_layer (GIMP_DRAWABLE (layer)));
    }

#define SET_VISIBLE(action,condition) \
        gimp_action_group_set_action_visible (group, action, (condition) != 0)
#define SET_SENSITIVE(action,condition) \
        gimp_action_group_set_action_sensitive (group, action, (condition) != 0)

  SET_VISIBLE   ("layers-text-tool",       text_layer && !ac);
  SET_SENSITIVE ("layers-edit-attributes", layer && !fs && !ac);

  SET_SENSITIVE ("layers-new",             gimage);
  SET_SENSITIVE ("layers-duplicate",    layer && !fs && !ac);
  SET_SENSITIVE ("layers-delete",       layer && !ac);

  SET_SENSITIVE ("layers-select-previous", layer && !fs && !ac && prev);
  SET_SENSITIVE ("layers-select-top",      layer && !fs && !ac && prev);
  SET_SENSITIVE ("layers-select-next",     layer && !fs && !ac && next);
  SET_SENSITIVE ("layers-select-bottom",   layer && !fs && !ac && next);

  SET_SENSITIVE ("layers-raise",           layer && !fs && !ac && alpha && prev);
  SET_SENSITIVE ("layers-raise-to-top",    layer && !fs && !ac && alpha && prev);
  SET_SENSITIVE ("layers-lower",           layer && !fs && !ac && next && next_alpha);
  SET_SENSITIVE ("layers-lower-to-bottom", layer && !fs && !ac && next && next_alpha);

  SET_SENSITIVE ("layers-anchor",        layer &&  fs && !ac);
  SET_SENSITIVE ("layers-merge-down",    layer && !fs && !ac && next);
  SET_SENSITIVE ("layers-merge-layers",  layer && !fs && !ac);
  SET_SENSITIVE ("layers-flatten-image", layer && !fs && !ac);
  SET_VISIBLE   ("layers-text-discard",  text_layer && !ac);

  SET_SENSITIVE ("layers-resize",          layer && !ac);
  SET_SENSITIVE ("layers-resize-to-image", layer && !ac);
  SET_SENSITIVE ("layers-scale",           layer && !ac);

  SET_SENSITIVE ("layers-mask-add",    layer && !fs && !ac && !lm && alpha);
  SET_SENSITIVE ("layers-mask-apply",  layer && !fs && !ac &&  lm);
  SET_SENSITIVE ("layers-mask-delete", layer && !fs && !ac &&  lm);
  SET_SENSITIVE ("layers-alpha-add",   layer && !fs && !alpha);

  SET_SENSITIVE ("layers-mask-selection-replace",   layer && !fs && !ac && lm);
  SET_SENSITIVE ("layers-mask-selection-add",       layer && !fs && !ac && lm);
  SET_SENSITIVE ("layers-mask-selection-subtract",  layer && !fs && !ac && lm);
  SET_SENSITIVE ("layers-mask-selection-intersect", layer && !fs && !ac && lm);

  SET_SENSITIVE ("layers-alpha-selection-replace",   layer && !fs && !ac);
  SET_SENSITIVE ("layers-alpha-selection-add",       layer && !fs && !ac);
  SET_SENSITIVE ("layers-alpha-selection-subtract",  layer && !fs && !ac);
  SET_SENSITIVE ("layers-alpha-selection-intersect", layer && !fs && !ac);

#undef SET_VISIBLE
#undef SET_SENSITIVE
}
