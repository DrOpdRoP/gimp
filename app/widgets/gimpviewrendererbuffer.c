/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * gimpviewrendererbuffer.c
 * Copyright (C) 2004-2006 Michael Natterer <mitch@gimp.org>
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

#include "widgets-types.h"

#include "base/temp-buf.h"

#include "core/gimpviewable.h"

#include "gimpviewrendererbuffer.h"


static void   gimp_view_renderer_buffer_render (GimpViewRenderer *renderer,
                                                GtkWidget        *widget);


G_DEFINE_TYPE (GimpViewRendererBuffer, gimp_view_renderer_buffer,
               GIMP_TYPE_VIEW_RENDERER)

#define parent_class gimp_view_renderer_buffer_class_init


static void
gimp_view_renderer_buffer_class_init (GimpViewRendererBufferClass *klass)
{
  GimpViewRendererClass *renderer_class = GIMP_VIEW_RENDERER_CLASS (klass);

  renderer_class->render = gimp_view_renderer_buffer_render;
}

static void
gimp_view_renderer_buffer_init (GimpViewRendererBuffer *renderer)
{
}

static void
gimp_view_renderer_buffer_render (GimpViewRenderer *renderer,
                                  GtkWidget        *widget)
{
  gint      buffer_width;
  gint      buffer_height;
  gint      view_width;
  gint      view_height;
  gboolean  scaling_up;
  TempBuf  *render_buf = NULL;

  gimp_viewable_get_size (renderer->viewable, &buffer_width, &buffer_height);

  gimp_viewable_calc_preview_size (buffer_width,
                                   buffer_height,
                                   renderer->width,
                                   renderer->height,
                                   TRUE, 1.0, 1.0,
                                   &view_width,
                                   &view_height,
                                   &scaling_up);

  if (scaling_up)
    {
      TempBuf *temp_buf;

      temp_buf = gimp_viewable_get_new_preview (renderer->viewable,
                                                renderer->context,
                                                buffer_width, buffer_height);

      if (temp_buf)
        {
          render_buf = temp_buf_scale (temp_buf, view_width, view_height);

          temp_buf_free (temp_buf);
        }
    }
  else
    {
      render_buf = gimp_viewable_get_new_preview (renderer->viewable,
                                                  renderer->context,
                                                  view_width, view_height);
    }

  if (render_buf)
    {
      gimp_view_renderer_default_render_buffer (renderer, widget, render_buf);

      temp_buf_free (render_buf);
    }
  else /* no preview available */
    {
      const gchar  *stock_id;

      stock_id = gimp_viewable_get_stock_id (renderer->viewable);

      gimp_view_renderer_default_render_stock (renderer, widget, stock_id);
    }
}
