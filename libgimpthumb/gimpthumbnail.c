/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * Thumbnail handling according to the Thumbnail Managing Standard.
 * http://triq.net/~pearl/thumbnail-spec/
 *
 * Copyright (C) 2001-2003  Sven Neumann <sven@gimp.org>
 *                          Michael Natterer <mitch@gimp.org>
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

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "gimpthumb-types.h"
#include "gimpthumb-utils.h"
#include "gimpthumbnail.h"

#include "libgimp/libgimp-intl.h"


#define TAG_DESCRIPTION         "tEXt::Description"
#define TAG_SOFTWARE            "tEXt::Software"
#define TAG_THUMB_URI           "tEXt::Thumb::URI"
#define TAG_THUMB_MTIME         "tEXt::Thumb::MTime"
#define TAG_THUMB_FILESIZE      "tEXt::Thumb::Size"
#define TAG_THUMB_IMAGE_WIDTH   "tEXt::Thumb::Image::Width"
#define TAG_THUMB_IMAGE_HEIGHT  "tEXt::Thumb::Image::Height"
#define TAG_THUMB_GIMP_TYPE     "tEXt::Thumb::X-GIMP::Type"
#define TAG_THUMB_GIMP_LAYERS   "tEXt::Thumb::X-GIMP::Layers"


enum
{
  PROP_0,
  PROP_IMAGE_STATE,
  PROP_IMAGE_URI,
  PROP_IMAGE_MTIME,
  PROP_IMAGE_FILESIZE,
  PROP_IMAGE_WIDTH,
  PROP_IMAGE_HEIGHT,
  PROP_IMAGE_TYPE,
  PROP_IMAGE_NUM_LAYERS,
  PROP_THUMB_STATE
};


static void        gimp_thumbnail_class_init       (GimpThumbnailClass *klass);
static void        gimp_thumbnail_init             (GimpThumbnail  *thumbnail);
static void        gimp_thumbnail_finalize         (GObject        *object);
static void        gimp_thumbnail_set_property     (GObject        *object,
                                                    guint           property_id,
                                                    const GValue   *value,
                                                    GParamSpec     *pspec);
static void        gimp_thumbnail_get_property     (GObject        *object,
                                                    guint           property_id,
                                                    GValue         *value,
                                                    GParamSpec     *pspec);
static void        gimp_thumbnail_invalidate_thumb (GimpThumbnail  *thumbnail);
static void        gimp_thumbnail_reset_info       (GimpThumbnail  *thumbnail);

static void        gimp_thumbnail_update_image     (GimpThumbnail  *thumbnail);
static void        gimp_thumbnail_update_thumb     (GimpThumbnail  *thumbnail);

static GdkPixbuf * gimp_thumbnail_read_thumb       (GimpThumbnail  *thumbnail,
                                                    GimpThumbSize   thumb_size,
                                                    GError        **error);


static GObjectClass *parent_class = NULL;


GType
gimp_thumbnail_get_type (void)
{
  static GType thumbnail_type = 0;

  if (!thumbnail_type)
    {
      static const GTypeInfo thumbnail_info =
      {
        sizeof (GimpThumbnailClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_thumbnail_class_init,
	NULL,           /* class_finalize */
	NULL,		/* class_data     */
	sizeof (GimpThumbnail),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_thumbnail_init,
      };

      thumbnail_type = g_type_register_static (G_TYPE_OBJECT,
					       "GimpThumbnail",
                                               &thumbnail_info, 0);
    }

  return thumbnail_type;
}

static void
gimp_thumbnail_class_init (GimpThumbnailClass *klass)
{
  GObjectClass *object_class;

  parent_class = g_type_class_peek_parent (klass);

  object_class = G_OBJECT_CLASS (klass);

  object_class->finalize     = gimp_thumbnail_finalize;
  object_class->set_property = gimp_thumbnail_set_property;
  object_class->get_property = gimp_thumbnail_get_property;

  g_object_class_install_property (object_class,
				   PROP_IMAGE_STATE,
				   g_param_spec_enum ("image-state", NULL,
                                                      NULL,
                                                      GIMP_TYPE_THUMB_STATE,
                                                      GIMP_THUMB_STATE_UNKNOWN,
                                                      G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_IMAGE_URI,
				   g_param_spec_string ("image-uri", NULL,
                                                        NULL,
							NULL,
							G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_IMAGE_MTIME,
				   g_param_spec_int64 ("image-mtime", NULL,
                                                       NULL,
                                                       0, G_MAXINT64, 0,
                                                       G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_IMAGE_FILESIZE,
				   g_param_spec_int64 ("image-filesize", NULL,
                                                       NULL,
                                                       0, G_MAXINT64, 0,
                                                       G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_IMAGE_WIDTH,
				   g_param_spec_int ("image-width", NULL,
                                                     NULL,
                                                     0, G_MAXINT, 0,
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_IMAGE_HEIGHT,
				   g_param_spec_int ("image-height", NULL,
                                                     NULL,
                                                     0, G_MAXINT, 0,
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_IMAGE_TYPE,
                                   g_param_spec_string ("image-type", NULL,
                                                        NULL,
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_IMAGE_NUM_LAYERS,
                                   g_param_spec_int ("image-num-layers", NULL,
                                                     NULL,
                                                     0, G_MAXINT, 0,
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
				   PROP_THUMB_STATE,
				   g_param_spec_enum ("thumb-state", NULL,
                                                      NULL,
                                                      GIMP_TYPE_THUMB_STATE,
                                                      GIMP_THUMB_STATE_UNKNOWN,
                                                      G_PARAM_READWRITE));
}

static void
gimp_thumbnail_init (GimpThumbnail *thumbnail)
{
  thumbnail->image_state      = GIMP_THUMB_STATE_UNKNOWN;
  thumbnail->image_uri        = NULL;
  thumbnail->image_filename   = NULL;
  thumbnail->image_mtime      = 0;
  thumbnail->image_filesize   = 0;
  thumbnail->image_width      = 0;
  thumbnail->image_height     = 0;
  thumbnail->image_type       = 0;
  thumbnail->image_num_layers = 0;

  thumbnail->thumb_state      = GIMP_THUMB_STATE_UNKNOWN;
  thumbnail->thumb_filename   = NULL;
  thumbnail->thumb_mtime      = 0;
  thumbnail->thumb_filesize   = 0;
}

static void
gimp_thumbnail_finalize (GObject *object)
{
  GimpThumbnail *thumbnail = GIMP_THUMBNAIL (object);

  if (thumbnail->image_uri)
    {
      g_free (thumbnail->image_uri);
      thumbnail->image_uri = NULL;
    }
  if (thumbnail->image_filename)
    {
      g_free (thumbnail->image_filename);
      thumbnail->image_filename = NULL;
    }
  if (thumbnail->image_type)
    {
      g_free (thumbnail->image_type);
      thumbnail->image_type = NULL;
    }
  if (thumbnail->thumb_filename)
    {
      g_free (thumbnail->thumb_filename);
      thumbnail->thumb_filename = NULL;
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gimp_thumbnail_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GimpThumbnail *thumbnail = GIMP_THUMBNAIL (object);
  gint64         int_value;

  switch (property_id)
    {
    case PROP_IMAGE_STATE:
      thumbnail->image_state = g_value_get_enum (value);
      break;
    case PROP_IMAGE_URI:
      gimp_thumbnail_set_uri (GIMP_THUMBNAIL (object),
                              g_value_get_string (value));
      break;
    case PROP_IMAGE_MTIME:
      int_value = g_value_get_int64 (value);
      if (thumbnail->image_mtime != int_value)
        {
          thumbnail->image_mtime = int_value;
          gimp_thumbnail_invalidate_thumb (thumbnail);
        }
      break;
    case PROP_IMAGE_FILESIZE:
      int_value = g_value_get_int64 (value);
      if (thumbnail->image_filesize != int_value)
        {
          thumbnail->image_filesize = int_value;
          gimp_thumbnail_invalidate_thumb (thumbnail);
        }
      break;
    case PROP_IMAGE_WIDTH:
      thumbnail->image_width = g_value_get_int (value);
      break;
    case PROP_IMAGE_HEIGHT:
      thumbnail->image_height = g_value_get_int (value);
      break;
    case PROP_IMAGE_TYPE:
      g_free (thumbnail->image_type);
      thumbnail->image_type = g_value_dup_string (value);
      break;
    case PROP_IMAGE_NUM_LAYERS:
      thumbnail->image_num_layers = g_value_get_int (value);
      break;
    case PROP_THUMB_STATE:
      thumbnail->thumb_state = g_value_get_enum (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_thumbnail_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GimpThumbnail *thumbnail = GIMP_THUMBNAIL (object);

  switch (property_id)
    {
    case PROP_IMAGE_STATE:
      g_value_set_enum (value, thumbnail->image_state);
      break;
    case PROP_IMAGE_URI:
      g_value_set_string (value, thumbnail->image_uri);
      break;
    case PROP_IMAGE_MTIME:
      g_value_set_int64 (value, thumbnail->image_mtime);
      break;
    case PROP_IMAGE_FILESIZE:
      g_value_set_int64 (value, thumbnail->image_filesize);
      break;
    case PROP_IMAGE_WIDTH:
      g_value_set_int (value, thumbnail->image_width);
      break;
    case PROP_IMAGE_HEIGHT:
      g_value_set_int (value, thumbnail->image_height);
      break;
    case PROP_IMAGE_TYPE:
      g_value_set_string (value, thumbnail->image_type);
      break;
    case PROP_IMAGE_NUM_LAYERS:
      g_value_set_int (value, thumbnail->image_height);
      break;
    case PROP_THUMB_STATE:
      g_value_set_enum (value, thumbnail->thumb_state);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_thumbnail_reset_info (GimpThumbnail *thumbnail)
{
  g_object_set (thumbnail,
                "image-width",      0,
                "image-height",     0,
                "image-type",       NULL,
                "image-num-layers", 0,
                NULL);
}

GimpThumbnail *
gimp_thumbnail_new (void)
{
  return g_object_new (GIMP_TYPE_THUMBNAIL, NULL);
}

void
gimp_thumbnail_set_uri (GimpThumbnail *thumbnail,
                        const gchar   *uri)
{
  g_return_if_fail (GIMP_IS_THUMBNAIL (thumbnail));

  g_object_set (thumbnail,
                "image-state",      GIMP_THUMB_STATE_UNKNOWN,
                "image-filesize",   0,
                "image-mtime",      0,
                "image-width",      0,
                "image-height",     0,
                "image-type",       NULL,
                "image-num-layers", 0,
                NULL);

  gimp_thumbnail_invalidate_thumb (thumbnail);
}

GimpThumbState
gimp_thumbnail_peek_image (GimpThumbnail *thumbnail)
{
  g_return_val_if_fail (GIMP_IS_THUMBNAIL (thumbnail),
                        GIMP_THUMB_STATE_UNKNOWN);

  g_object_freeze_notify (G_OBJECT (thumbnail));

  gimp_thumbnail_update_image (thumbnail);

  g_object_thaw_notify (G_OBJECT (thumbnail));

  return thumbnail->image_state;
}

GimpThumbState
gimp_thumbnail_peek_thumb (GimpThumbnail *thumbnail,
                           GimpThumbSize  size)
{
  g_return_val_if_fail (GIMP_IS_THUMBNAIL (thumbnail),
                        GIMP_THUMB_STATE_UNKNOWN);

  g_object_freeze_notify (G_OBJECT (thumbnail));

  gimp_thumbnail_update_image (thumbnail);
  gimp_thumbnail_update_thumb (thumbnail);

  g_object_thaw_notify (G_OBJECT (thumbnail));

  return thumbnail->thumb_state;
}

static void
gimp_thumbnail_invalidate_thumb (GimpThumbnail *thumbnail)
{
  if (thumbnail->thumb_filename)
    {
      g_free (thumbnail->thumb_filename);
      thumbnail->thumb_filename = NULL;
    }

  thumbnail->thumb_filesize = 0;
  thumbnail->thumb_mtime    = 0;

  if (thumbnail->thumb_state != GIMP_THUMB_STATE_UNKNOWN)
    {
      g_object_set (thumbnail,
                    "thumb-state", GIMP_THUMB_STATE_UNKNOWN,
                    NULL);
    }
}

static void
gimp_thumbnail_update_image (GimpThumbnail *thumbnail)
{
  GimpThumbState  state;
  gint64          filesize = 0;
  gint64          mtime    = 0;

  g_return_if_fail (thumbnail->image_uri != NULL);

  state = thumbnail->image_state;

  switch (state)
    {
    case GIMP_THUMB_STATE_UNKNOWN:
      g_return_if_fail (thumbnail->image_filename == NULL);

      thumbnail->image_filename = g_filename_from_uri (thumbnail->image_uri,
                                                       NULL, NULL);

      if (! thumbnail->image_filename)
        state = GIMP_THUMB_STATE_REMOTE;

      break;

    case GIMP_THUMB_STATE_REMOTE:
      break;

    default:
      g_return_if_fail (thumbnail->image_filename != NULL);
      break;
    }

  switch (state)
    {
    case GIMP_THUMB_STATE_REMOTE:
      break;

    default:
      if (gimp_thumb_file_test (thumbnail->image_filename, &mtime, &filesize))
        state = GIMP_THUMB_STATE_EXISTS;
      else
        state = GIMP_THUMB_STATE_NOT_FOUND;
      break;
    }

  if (state != thumbnail->image_state)
    {
      g_object_set (thumbnail,
                    "image-state",
                    state, NULL);
    }

  if (mtime != thumbnail->image_mtime || filesize != thumbnail->image_filesize)
    {
      g_object_set (thumbnail,
                    "image-mtime",    mtime,
                    "image-filesize", filesize,
                    NULL);
    }
}

static void
gimp_thumbnail_update_thumb (GimpThumbnail *thumbnail)
{
  GimpThumbState  state;
  GimpThumbSize   size;
  gint64          filesize;
  gint64          mtime;

  g_return_if_fail (thumbnail->image_uri != NULL);

  state = thumbnail->thumb_state;

  switch (state)
    {
    case GIMP_THUMB_STATE_UNKNOWN:
      g_return_if_fail (thumbnail->thumb_filename == NULL);

      thumbnail->thumb_filename =
        gimp_thumb_find_thumb (thumbnail->image_uri, &size);
      break;

    default:
      g_return_if_fail (thumbnail->thumb_filename != NULL);
      break;
    }

   switch (state)
     {
     default:
       if (gimp_thumb_file_test (thumbnail->thumb_filename, &mtime, &filesize))
         state = GIMP_THUMB_STATE_EXISTS;
       else
         state = GIMP_THUMB_STATE_NOT_FOUND;
       break;
     }

  if (state != thumbnail->thumb_state)
    g_object_set (thumbnail, "thumb-state", state, NULL);
}

gboolean
gimp_thumbnail_set_filename (GimpThumbnail  *thumbnail,
                             const gchar    *filename,
                             GError        **error)
{
  gchar *uri = NULL;

  g_return_val_if_fail (GIMP_IS_THUMBNAIL (thumbnail), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (filename)
    uri = g_filename_to_uri (filename, NULL, error);

  gimp_thumbnail_set_uri (thumbnail, uri);

  return (!filename || uri);
}

static void
gimp_thumbnail_set_info_from_pixbuf (GimpThumbnail *thumbnail,
                                     GdkPixbuf     *pixbuf)
{
  const gchar  *option;
  gint          n;

  g_object_freeze_notify (G_OBJECT (thumbnail));

  gimp_thumbnail_reset_info (thumbnail);

  option = gdk_pixbuf_get_option (pixbuf, TAG_THUMB_IMAGE_WIDTH);
  if (option && sscanf (option, "%d", &n) == 1)
    thumbnail->image_width = n;

  option = gdk_pixbuf_get_option (pixbuf, TAG_THUMB_IMAGE_HEIGHT);
  if (option && sscanf (option, "%d", &n) == 1)
    thumbnail->image_height = n;

  thumbnail->image_type =
    g_strdup (gdk_pixbuf_get_option (pixbuf, TAG_THUMB_GIMP_TYPE));

  option = gdk_pixbuf_get_option (pixbuf, TAG_THUMB_GIMP_LAYERS);
  if (option && sscanf (option, "%d", &n) == 1)
    thumbnail->image_num_layers = 0;

  g_object_thaw_notify (G_OBJECT (thumbnail));
}

GdkPixbuf *
gimp_thumbnail_load_thumb (GimpThumbnail  *thumbnail,
                           GimpThumbSize   size,
                           GError        **error)
{
  GdkPixbuf *pixbuf;

  g_return_val_if_fail (GIMP_IS_THUMBNAIL (thumbnail), NULL);
  g_return_val_if_fail (thumbnail->image_uri != NULL, NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (! thumbnail->image_uri)
    return NULL;

  pixbuf = gimp_thumbnail_read_thumb (thumbnail, size, error);

  if (pixbuf)
    {
      gint width  = gdk_pixbuf_get_width (pixbuf);
      gint height = gdk_pixbuf_get_height (pixbuf);

      if (width > size || height > size)
        {
          GdkPixbuf *scaled;

          /*  FIXME: aspect ratio  */
          scaled = gdk_pixbuf_scale_simple (pixbuf,
                                            size, size,
                                            GDK_INTERP_BILINEAR);

          g_object_unref (pixbuf);
          pixbuf = scaled;
        }
    }

  return pixbuf;
}

gboolean
gimp_thumbnail_save_thumb (GimpThumbnail  *thumbnail,
                           GdkPixbuf      *pixbuf,
                           const gchar    *software,
                           GError        **error)
{
  GimpThumbSize  size;
  gchar         *name;
  gchar         *desc;
  gchar         *time_str;
  gchar         *size_str;
  gchar         *width_str;
  gchar         *height_str;
  gchar         *num_str;
  gboolean       success;

  g_return_val_if_fail (GIMP_IS_THUMBNAIL (thumbnail), FALSE);
  g_return_val_if_fail (thumbnail->image_uri != NULL, FALSE);
  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), FALSE);
  g_return_val_if_fail (software != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (! gimp_thumb_ensure_thumb_dirs (error))
    return FALSE;

  size = MAX (gdk_pixbuf_get_width (pixbuf), gdk_pixbuf_get_height (pixbuf));
  if (size < 1)
    return TRUE;

  name = gimp_thumb_name_from_uri (thumbnail->image_uri, &size);
  if (! name)
    return TRUE;

  desc     = g_strdup_printf ("Thumbnail of %s",   thumbnail->image_uri);
  time_str = g_strdup_printf ("%" G_GINT64_FORMAT, thumbnail->image_mtime);
  size_str = g_strdup_printf ("%" G_GINT64_FORMAT, thumbnail->image_filesize);

  width_str  = g_strdup_printf ("%d", thumbnail->image_width);
  height_str = g_strdup_printf ("%d", thumbnail->image_height);
  num_str    = g_strdup_printf ("%d", thumbnail->image_num_layers);

  success =  gdk_pixbuf_save (pixbuf, name, "png", error,
                              TAG_DESCRIPTION,        desc,
                              TAG_SOFTWARE,           software,
                              TAG_THUMB_URI,          thumbnail->image_uri,
                              TAG_THUMB_MTIME,        time_str,
                              TAG_THUMB_FILESIZE,     size_str,
                              TAG_THUMB_IMAGE_WIDTH,  width_str,
                              TAG_THUMB_IMAGE_HEIGHT, height_str,

                              thumbnail->image_type ?
                              TAG_THUMB_GIMP_TYPE : NULL,
                              thumbnail->image_type,

                              thumbnail->image_num_layers > 0 ?
                              TAG_THUMB_GIMP_LAYERS : NULL,
                              num_str,

                              NULL);

  if (success)
    {
      success = chmod (name, 0600);

      if (! success)
        g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                     "Could not set permissions of thumbnail '%s': %s",
                     name, g_strerror (errno));
    }

  g_free (num_str);
  g_free (height_str);
  g_free (width_str);
  g_free (size_str);
  g_free (time_str);
  g_free (desc);
  g_free (name);

  return success;
}

gboolean
gimp_thumbnail_save_failure (GimpThumbnail  *thumbnail,
                             const gchar    *software,
                             GError        **error)
{
  GdkPixbuf     *pixbuf;
  GimpThumbSize  size = GIMP_THUMB_SIZE_FAIL;
  gchar         *name;
  gchar         *desc;
  gchar         *time_str;
  gchar         *size_str;
  gboolean       success;

  g_return_val_if_fail (GIMP_IS_THUMBNAIL (thumbnail), FALSE);
  g_return_val_if_fail (thumbnail->image_uri != NULL, FALSE);
  g_return_val_if_fail (software != NULL, FALSE);

  if (! gimp_thumb_ensure_thumb_dirs (error))
    return FALSE;

  name = gimp_thumb_name_from_uri (thumbnail->image_uri, &size);
  if (! name)
    return TRUE;

  pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 1, 1);

  desc = g_strdup_printf ("Thumbnail failure for %s", thumbnail->image_uri);
  time_str = g_strdup_printf ("%" G_GINT64_FORMAT, thumbnail->image_mtime);
  size_str = g_strdup_printf ("%" G_GINT64_FORMAT, thumbnail->image_filesize);

  success = gdk_pixbuf_save (pixbuf, name, "png", error,
                             TAG_DESCRIPTION,    desc,
                             TAG_SOFTWARE,       software,
                             TAG_THUMB_URI,      thumbnail->image_uri,
                             TAG_THUMB_MTIME,    time_str,
                             TAG_THUMB_FILESIZE, size_str,
                             NULL);
  if (success)
    {
      success = chmod (name, 0600);

      if (! success)
        g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                     "Could not set permissions of thumbnail '%s': %s",
                     name, g_strerror (errno));
    }

  g_object_unref (pixbuf);

  g_free (size_str);
  g_free (time_str);
  g_free (desc);
  g_free (name);

  return success;
}

static GdkPixbuf *
gimp_thumbnail_read_thumb (GimpThumbnail  *thumbnail,
                           GimpThumbSize   thumb_size,
                           GError        **error)
{
  GimpThumbState  state;
  GdkPixbuf      *pixbuf  = NULL;
  gchar          *name;
  const gchar    *option;
  gint64          image_mtime;
  gint64          image_size;

  g_return_val_if_fail (thumbnail->thumb_state < GIMP_THUMB_STATE_EXISTS, NULL);

  state = GIMP_THUMB_STATE_NOT_FOUND;

  name = gimp_thumb_find_thumb (thumbnail->image_uri, &thumb_size);

  if (!name)
    goto cleanup;

  pixbuf = gdk_pixbuf_new_from_file (name, error);

  g_free (name);

  if (! pixbuf)
    goto cleanup;

  /* URI and mtime from the thumbnail need to match our file */

  option = gdk_pixbuf_get_option (pixbuf, TAG_THUMB_URI);
  if (!option || strcmp (option, thumbnail->image_uri))
    goto cleanup;

  state = GIMP_THUMB_STATE_OLD;

  option = gdk_pixbuf_get_option (pixbuf, TAG_THUMB_MTIME);
  if (!option || sscanf (option, "%" G_GINT64_FORMAT, &image_mtime) != 1)
    goto cleanup;

  option = gdk_pixbuf_get_option (pixbuf, TAG_THUMB_FILESIZE);
  if (option && sscanf (option, "%" G_GINT64_FORMAT, &image_size) != 1)
    goto cleanup;

  /* TAG_THUMB_FILESIZE is optional but must match if present */
  if (image_mtime == thumbnail->image_mtime &&
      (option == NULL || image_size == thumbnail->image_filesize))
    {
      if (! thumb_size)
        state = GIMP_THUMB_STATE_FAILED;
      else
        state = GIMP_THUMB_STATE_OK;
    }

  if (! thumb_size)
    {
      gimp_thumbnail_reset_info (thumbnail);
      goto cleanup;
    }

  gimp_thumbnail_set_info_from_pixbuf (thumbnail, pixbuf);

 cleanup:
  if (state != thumbnail->thumb_state)
    g_object_set (thumbnail, "state", state, NULL);

  if (pixbuf && state != GIMP_THUMB_STATE_OK)
    {
      g_object_unref (pixbuf);
      return NULL;
    }

  return pixbuf;
}
