/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimppatternpreview.c
 * Copyright (C) 2001 Michael Natterer <mitch@gimp.org>
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

#include "widgets-types.h"

#include "base/temp-buf.h"

#include "core/gimppattern.h"

#include "gimppatternpreview.h"


static void   gimp_pattern_preview_class_init (GimpPatternPreviewClass *klass);
static void   gimp_pattern_preview_init       (GimpPatternPreview      *preview);

static void        gimp_pattern_preview_render       (GimpPreview *preview);
static GtkWidget * gimp_pattern_preview_create_popup (GimpPreview *preview);
static gboolean    gimp_pattern_preview_needs_popup  (GimpPreview *preview);


static GimpPreviewClass *parent_class = NULL;


GType
gimp_pattern_preview_get_type (void)
{
  static GType preview_type = 0;

  if (! preview_type)
    {
      static const GTypeInfo preview_info =
      {
        sizeof (GimpPatternPreviewClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) gimp_pattern_preview_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GimpPatternPreview),
        0,              /* n_preallocs */
        (GInstanceInitFunc) gimp_pattern_preview_init,
      };

      preview_type = g_type_register_static (GIMP_TYPE_PREVIEW,
                                             "GimpPatternPreview",
                                             &preview_info, 0);
    }
  
  return preview_type;
}

static void
gimp_pattern_preview_class_init (GimpPatternPreviewClass *klass)
{
  GimpPreviewClass *preview_class;

  preview_class = GIMP_PREVIEW_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  preview_class->render       = gimp_pattern_preview_render;
  preview_class->create_popup = gimp_pattern_preview_create_popup;
  preview_class->needs_popup  = gimp_pattern_preview_needs_popup;
}

static void
gimp_pattern_preview_init (GimpPatternPreview *pattern_preview)
{
}

static void
gimp_pattern_preview_render (GimpPreview *preview)
{
  GimpPattern *pattern;
  TempBuf     *temp_buf;
  gint         width;
  gint         height;
  gint         pattern_width;
  gint         pattern_height;

  pattern        = GIMP_PATTERN (preview->viewable);
  pattern_width  = pattern->mask->width;
  pattern_height = pattern->mask->height;

  width  = preview->width;
  height = preview->height;

  if (width  == pattern_width &&
      height == pattern_height)
    {
      gimp_preview_render_and_flush (preview,
				     pattern->mask,
				     -1);

      return;
    }
  else if (width  <= pattern_width &&
	   height <= pattern_height)
    {
      gimp_preview_render_and_flush (preview,
				     pattern->mask,
				     -1);

      return;
    }

  temp_buf = gimp_viewable_get_new_preview (preview->viewable,
					    width, height);

  gimp_preview_render_and_flush (preview,
                                 temp_buf,
                                 -1);

  temp_buf_free (temp_buf);
}

static GtkWidget *
gimp_pattern_preview_create_popup (GimpPreview *preview)
{
  gint popup_width;
  gint popup_height;

  popup_width  = GIMP_PATTERN (preview->viewable)->mask->width;
  popup_height = GIMP_PATTERN (preview->viewable)->mask->height;

  return gimp_preview_new_full (preview->viewable,
				popup_width,
				popup_height,
				0,
				TRUE, FALSE, FALSE);
}

static gboolean
gimp_pattern_preview_needs_popup (GimpPreview *preview)
{
  GimpPattern *pattern;
  gint         pattern_width;
  gint         pattern_height;

  pattern        = GIMP_PATTERN (preview->viewable);
  pattern_width  = pattern->mask->width;
  pattern_height = pattern->mask->height;

  if (pattern_width > preview->width || pattern_height > preview->height)
    return TRUE;

  return FALSE;
}
