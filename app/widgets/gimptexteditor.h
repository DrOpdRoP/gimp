/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * GimpTextEditor
 * Copyright (C) 2002-2003  Sven Neumann <sven@gimp.org>
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

#ifndef __GIMP_TEXT_EDITOR_H__
#define __GIMP_TEXT_EDITOR_H__


typedef void  (* GimpTextEditorCallback) (GtkWidget       *editor,
					  GtkResponseType response,
					  gpointer        data);

GtkWidget * gimp_text_editor_new (GtkTextBuffer          *buffer,
				  const gchar            *title,
				  GimpTextEditorCallback  callback,
				  gpointer                callback_data);


#endif  /* __GIMP_TEXT_EDITOR_H__ */
