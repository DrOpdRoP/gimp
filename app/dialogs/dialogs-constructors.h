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

#ifndef __DIALOGS_CONSTRUCTORS_H__
#define __DIALOGS_CONSTRUCTORS_H__


/*  toplevel dialogs  */

GtkWidget * dialogs_toolbox_get            (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_device_status_get      (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_preferences_get        (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_module_browser_get     (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_undo_history_get       (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_display_filters_get    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_tips_get               (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_about_get              (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);


/*  docks & dockables  */

GtkWidget * dialogs_dock_new               (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);

GtkWidget * dialogs_tool_options_get       (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_error_console_get      (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);

GtkWidget * dialogs_image_list_view_new    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_brush_list_view_new    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_pattern_list_view_new  (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_gradient_list_view_new (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_palette_list_view_new  (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_tool_list_view_new     (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_buffer_list_view_new   (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);

GtkWidget * dialogs_image_grid_view_new    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_brush_grid_view_new    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_pattern_grid_view_new  (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_gradient_grid_view_new (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_palette_grid_view_new  (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_tool_grid_view_new     (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_buffer_grid_view_new   (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);

GtkWidget * dialogs_layer_list_view_new    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_channel_list_view_new  (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_vectors_list_view_new  (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_path_list_view_new     (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
GtkWidget * dialogs_indexed_palette_new    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);

GtkWidget * dialogs_document_history_new   (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);

GtkWidget * dialogs_brush_editor_get       (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
void        dialogs_edit_brush_func        (GimpData          *data);

GtkWidget * dialogs_gradient_editor_get    (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
void        dialogs_edit_gradient_func     (GimpData          *data);

GtkWidget * dialogs_palette_editor_get     (GimpDialogFactory *factory,
					    GimpContext       *context,
                                            gint               preview_size);
void        dialogs_edit_palette_func      (GimpData          *data);


#endif /* __DIALOGS_CONSTRUCTORS_H__ */
