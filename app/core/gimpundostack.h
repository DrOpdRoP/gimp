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

#ifndef __GIMP_UNDO_STACK_H__
#define __GIMP_UNDO_STACK_H__


#include "gimpundo.h"


#define GIMP_TYPE_UNDO_STACK            (gimp_undo_stack_get_type ())
#define GIMP_UNDO_STACK(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_UNDO_STACK, GimpUndoStack))
#define GIMP_UNDO_STACK_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_UNDO_STACK, GimpUndoStackClass))
#define GIMP_IS_UNDO_STACK(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_UNDO_STACK))
#define GIMP_IS_UNDO_STACK_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_UNDO_STACK))


typedef struct _GimpUndoStackClass GimpUndoStackClass;

struct _GimpUndoStack
{
  GimpUndo       parent_instance;
  
  GimpImage     *gimage;
  GimpContainer *undos;
};

struct _GimpUndoStackClass
{
  GimpUndoClass  parent_class;
};


GtkType         gimp_undo_stack_get_type (void);
GimpUndoStack * gimp_undo_stack_new      (GimpImage     *gimage);
void            gimp_undo_stack_push     (GimpUndoStack *stack, 
                                          GimpUndo      *undo);
GimpUndo      * gimp_undo_stack_pop      (GimpUndoStack *stack);
GimpUndo      * gimp_undo_stack_peek     (GimpUndoStack *stack);


#endif /* __GIMP_UNDO_STACK_H__ */
