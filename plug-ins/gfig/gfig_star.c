/*
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This is a plug-in for the GIMP.
 *
 * Generates images containing vector type drawings.
 *
 * Copyright (C) 1997 Andy Thomas  alt@picnic.demon.co.uk
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
 * 
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <gtk/gtk.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "config.h"
#include "libgimp/stdplugins-intl.h"

#include "gfig.h"

static gint star_num_sides    = 3; /* Default to three sided object */

static void       d_save_star             (Dobject * obj, FILE *to);
static void       d_draw_star             (Dobject *obj);
static void       d_paint_star            (Dobject *obj);
static Dobject  * d_copy_star             (Dobject * obj);
static Dobject  * d_new_star              (gint x, gint y);

gint
star_button_press (GtkWidget      *widget,
		   GdkEventButton *event,
		   gpointer        data)
{
  if ((event->type == GDK_2BUTTON_PRESS) &&
      (event->button == 1))
    num_sides_dialog (_("Star Number of Points"),
		      &star_num_sides, NULL, 3, 200);
  return FALSE;
}

static void
d_save_star (Dobject *obj,
	     FILE    *to)
{
  DobjPoints *spnt;
  
  spnt = obj->points;

  if (!spnt)
    return; /* End-of-line */

  fprintf (to, "<STAR>\n");

  while (spnt)
    {
      fprintf (to, "%d %d\n",
	      spnt->pnt.x,
	      spnt->pnt.y);
      spnt = spnt->next;
    }
  
  fprintf (to, "<EXTRA>\n");
  fprintf (to, "%d\n</EXTRA>\n", obj->type_data);
  fprintf (to, "</STAR>\n");
}

Dobject *
d_load_star (FILE *from)
{
  Dobject *new_obj = NULL;
  gint xpnt;
  gint ypnt;
  gchar buf[MAX_LOAD_LINE];

  while (get_line (buf, MAX_LOAD_LINE, from, 0))
    {
      if (sscanf (buf, "%d %d", &xpnt, &ypnt) != 2)
	{
	  /* Must be the end */
	  if (!strcmp ("<EXTRA>", buf))
	    {
	      gint nsides = 3;
	      /* Number of sides - data item */
	      if (!new_obj)
		{
		  g_warning ("[%d] Internal load error while loading star (extra area)",
			    line_no);
		  return (NULL);
		}
	      get_line (buf, MAX_LOAD_LINE, from, 0);
	      if (sscanf (buf, "%d", &nsides) != 1)
		{
		  g_warning ("[%d] Internal load error while loading star (extra area scanf)",
			    line_no);
		  return (NULL);
		}
	      new_obj->type_data = nsides;
	      get_line (buf, MAX_LOAD_LINE, from, 0);
	      if (strcmp ("</EXTRA>", buf))
		{
		  g_warning ("[%d] Internal load error while loading star",
			    line_no);
		  return NULL;
		} 
	      /* Go around and read the last line */
	      continue;
	    }
	  else if (strcmp ("</STAR>", buf))
	    {
	      g_warning ("[%d] Internal load error while loading star",
			line_no);
	      return NULL;
	    }
	  return new_obj;
	}
      
      if (!new_obj)
	new_obj = d_new_star (xpnt, ypnt);
      else
	d_pnt_add_line (new_obj, xpnt, ypnt,-1);
    }
  return new_obj;
}

static void
d_draw_star (Dobject *obj)
{
  DobjPoints * center_pnt;
  DobjPoints * outer_radius_pnt;
  DobjPoints * inner_radius_pnt;
  gint16 shift_x;
  gint16 shift_y;
  gdouble ang_grid;
  gdouble ang_loop;
  gdouble outer_radius;
  gdouble inner_radius;
  gdouble offset_angle;
  gint loop;
  GdkPoint start_pnt;
  GdkPoint first_pnt;
  gint do_line = 0;

  center_pnt = obj->points;

  if (!center_pnt)
    return; /* End-of-line */

  /* First point is the center */
  /* Just draw a control point around it */

  draw_sqr (&center_pnt->pnt);

  /* Next point defines the radius */
  outer_radius_pnt = center_pnt->next; /* this defines the vetices */

  if (!outer_radius_pnt)
    {
#ifdef DEBUG
      g_warning ("Internal error in star - no outer vertice point \n");
#endif /* DEBUG */
      return;
    }

  inner_radius_pnt = outer_radius_pnt->next; /* this defines the vetices */

  if (!inner_radius_pnt)
    {
#ifdef DEBUG
      g_warning ("Internal error in star - no inner vertice point \n");
#endif /* DEBUG */
      return;
    }

  /* Other control points */
  draw_sqr (&outer_radius_pnt->pnt);
  draw_sqr (&inner_radius_pnt->pnt);

  /* Have center and radius - draw star */

  shift_x = outer_radius_pnt->pnt.x - center_pnt->pnt.x;
  shift_y = outer_radius_pnt->pnt.y - center_pnt->pnt.y;

  outer_radius = sqrt ((shift_x*shift_x) + (shift_y*shift_y));

  /* Lines */
  ang_grid = 2*G_PI/(2.0*(gdouble) obj->type_data);
  offset_angle = atan2 (shift_y, shift_x);

  shift_x = inner_radius_pnt->pnt.x - center_pnt->pnt.x;
  shift_y = inner_radius_pnt->pnt.y - center_pnt->pnt.y;

  inner_radius = sqrt ((shift_x*shift_x) + (shift_y*shift_y));

  for (loop = 0 ; loop < 2 * obj->type_data ; loop++)
    {
      gdouble lx, ly;
      GdkPoint calc_pnt;

      ang_loop = (gdouble)loop * ang_grid + offset_angle;
	
      if (loop%2)
	{
	  lx = inner_radius * cos (ang_loop);
	  ly = inner_radius * sin (ang_loop);
	}
      else
	{
	  lx = outer_radius * cos (ang_loop);
	  ly = outer_radius * sin (ang_loop);
	}

      calc_pnt.x = RINT (lx + center_pnt->pnt.x);
      calc_pnt.y = RINT (ly + center_pnt->pnt.y);

      if (do_line)
	{

	  /* Miss out points that come to the same location */
	  if (calc_pnt.x == start_pnt.x && calc_pnt.y == start_pnt.y)
	    continue;

	  if (drawing_pic)
	    {
	      gdk_draw_line (pic_preview->window,
			     pic_preview->style->black_gc,			    
			     adjust_pic_coords (calc_pnt.x,
						preview_width),
			     adjust_pic_coords (calc_pnt.y,
						preview_height),
			     adjust_pic_coords (start_pnt.x,
						preview_width),
			     adjust_pic_coords (start_pnt.y,
						preview_height));
	    }
	  else
	    {
	      gdk_draw_line (gfig_preview->window,
			     gfig_gc,
			     gfig_scale_x (calc_pnt.x),
			     gfig_scale_y (calc_pnt.y),
			     gfig_scale_x (start_pnt.x),
			     gfig_scale_y (start_pnt.y));
	    }
	}
      else
	{
	  do_line = 1;
	  first_pnt.x = calc_pnt.x;
	  first_pnt.y = calc_pnt.y;
	}
      start_pnt.x = calc_pnt.x;
      start_pnt.y = calc_pnt.y;
    }

  /* Join up */
  if (drawing_pic)
    {
      gdk_draw_line (pic_preview->window,
		     pic_preview->style->black_gc,
		     adjust_pic_coords (first_pnt.x, preview_width),
		     adjust_pic_coords (first_pnt.y, preview_width),
		     adjust_pic_coords (start_pnt.x, preview_width),
		     adjust_pic_coords (start_pnt.y, preview_width));
    }
  else
    {
      gdk_draw_line (gfig_preview->window,
		     gfig_gc,
		     gfig_scale_x (first_pnt.x),
		     gfig_scale_y (first_pnt.y),
		     gfig_scale_x (start_pnt.x),
		     gfig_scale_y (start_pnt.y));
    }
}

static void
d_paint_star (Dobject *obj)
{
  /* first point center */
  /* Next point is radius */
  gdouble *line_pnts;
  gint seg_count = 0;
  gint i = 0;
  DobjPoints * center_pnt;
  DobjPoints * outer_radius_pnt;
  DobjPoints * inner_radius_pnt;
  gint16 shift_x;
  gint16 shift_y;
  gdouble ang_grid;
  gdouble ang_loop;
  gdouble outer_radius;
  gdouble inner_radius;

  gdouble offset_angle;
  gint loop;
  GdkPoint first_pnt, last_pnt;
  gint first = 1;

  g_assert (obj != NULL);

  /* count - add one to close polygon */
  seg_count = 2 * obj->type_data + 1;

  center_pnt = obj->points;

  if (!center_pnt || !seg_count)
    return; /* no-line */

  line_pnts = g_new0 (gdouble, 2 * seg_count + 1);
  
  /* Go around all the points drawing a line from one to the next */
  /* Next point defines the radius */
  outer_radius_pnt = center_pnt->next; /* this defines the vetices */

  if (!outer_radius_pnt)
    {
#ifdef DEBUG
      g_warning ("Internal error in star - no outer vertice point \n");
#endif /* DEBUG */
      return;
    }

  inner_radius_pnt = outer_radius_pnt->next; /* this defines the vetices */

  if (!inner_radius_pnt)
    {
#ifdef DEBUG
      g_warning ("Internal error in star - no inner vertice point \n");
#endif /* DEBUG */
      return;
    }

  shift_x = outer_radius_pnt->pnt.x - center_pnt->pnt.x;
  shift_y = outer_radius_pnt->pnt.y - center_pnt->pnt.y;

  outer_radius = sqrt ((shift_x*shift_x) + (shift_y*shift_y));

  /* Lines */
  ang_grid = 2*G_PI/(2.0*(gdouble) obj->type_data);
  offset_angle = atan2 (shift_y, shift_x);

  shift_x = inner_radius_pnt->pnt.x - center_pnt->pnt.x;
  shift_y = inner_radius_pnt->pnt.y - center_pnt->pnt.y;

  inner_radius = sqrt ((shift_x*shift_x) + (shift_y*shift_y));

  for (loop = 0 ; loop < 2*obj->type_data ; loop++)
    {
      gdouble lx, ly;
      GdkPoint calc_pnt;
      
      ang_loop = (gdouble)loop * ang_grid + offset_angle;
	
      if (loop%2)
	{
	  lx = inner_radius * cos (ang_loop);
	  ly = inner_radius * sin (ang_loop);
	}
      else
	{
	  lx = outer_radius * cos (ang_loop);
	  ly = outer_radius * sin (ang_loop);
	}

      calc_pnt.x = RINT (lx + center_pnt->pnt.x);
      calc_pnt.y = RINT (ly + center_pnt->pnt.y);

      /* Miss out duped pnts */
      if (!first)
	{
	  if (calc_pnt.x == last_pnt.x && calc_pnt.y == last_pnt.y)
	    {
	      continue;
	    }
	}

      last_pnt.x = line_pnts[i++] = calc_pnt.x;
      last_pnt.y = line_pnts[i++] = calc_pnt.y;

      if (first)
	{
	  first_pnt = calc_pnt;
	  first = 0;
	}
    }

  line_pnts[i++] = first_pnt.x;
  line_pnts[i++] = first_pnt.y;

  /* Reverse line if approp */
  if (selvals.reverselines)
    reverse_pairs_list (&line_pnts[0], i/2);

  /* Scale before drawing */
  if (selvals.scaletoimage)
    scale_to_original_xy (&line_pnts[0], i/2);
  else
    scale_to_xy (&line_pnts[0], i/2);

  /* One go */
  if (selvals.painttype == PAINT_BRUSH_TYPE)
    {
      gfig_paint (selvals.brshtype,
		  gfig_drawable,
		  i, line_pnts);
    }
  else
    {
      gimp_free_select (gfig_image,
			i, line_pnts,
			selopt.type,
			selopt.antia,
			selopt.feather,
			selopt.feather_radius);
    }

  g_free (line_pnts);
}

static Dobject *
d_copy_star (Dobject * obj)
{
  Dobject *np;

  if (!obj)
    return (NULL);

  g_assert (obj->type == STAR);

  np = d_new_star (obj->points->pnt.x, obj->points->pnt.y);

  np->points->next = d_copy_dobjpoints (obj->points->next);

  np->type_data = obj->type_data;

  return np;
}

static Dobject *
d_new_star (gint x,
	    gint y)
{
  Dobject *nobj;
  DobjPoints *npnt;
 
  /* Get new object and starting point */

  /* Start point */
  npnt = g_new0 (DobjPoints, 1);

  npnt->pnt.x = x;
  npnt->pnt.y = y;

  nobj = g_new0 (Dobject, 1);

  nobj->type = STAR;
  nobj->type_data = 3; /* Default to three sides 6 points*/
  nobj->points = npnt;
  nobj->drawfunc  = d_draw_star;
  nobj->loadfunc  = d_load_star;
  nobj->savefunc  = d_save_star;
  nobj->paintfunc = d_paint_star;
  nobj->copyfunc  = d_copy_star;

  return nobj;
}

void
d_update_star (GdkPoint *pnt)
{
  DobjPoints *center_pnt, *inner_pnt, *outer_pnt;
  gint saved_cnt_pnt = selvals.opts.showcontrol;

  /* Undraw last one then draw new one */
  center_pnt = obj_creating->points;
  
  if (!center_pnt)
    return; /* No points */

  /* Leave the first pnt alone -
   * Edge point defines "radius"
   * Only undraw if already have edge point.
   */

  /* Hack - turn off cnt points in draw routine 
   * Looking back over the other update routines I could
   * use this trick again and cut down on code size!
   */


  if ((outer_pnt = center_pnt->next))
    {
      /* Undraw */
      inner_pnt = outer_pnt->next;
      draw_circle (&inner_pnt->pnt);
      draw_circle (&outer_pnt->pnt);
      selvals.opts.showcontrol = 0;
      d_draw_star (obj_creating);
      outer_pnt->pnt.x = pnt->x;
      outer_pnt->pnt.y = pnt->y;
      inner_pnt->pnt.x = pnt->x + (2*(center_pnt->pnt.x - pnt->x))/3;
      inner_pnt->pnt.y = pnt->y + (2*(center_pnt->pnt.y - pnt->y))/3;
    }
  else
    {
      /* Radius is a few pixels away */
      /* First edge point */
      d_pnt_add_line (obj_creating, pnt->x, pnt->y,-1);
      outer_pnt = center_pnt->next;
      /* Inner radius */
      d_pnt_add_line (obj_creating,
		      pnt->x + (2*(center_pnt->pnt.x - pnt->x))/3,
		      pnt->y + (2*(center_pnt->pnt.y - pnt->y))/3,
		      -1);
      inner_pnt = outer_pnt->next;
    }

  /* draw it */
  selvals.opts.showcontrol = 0;
  d_draw_star (obj_creating);
  selvals.opts.showcontrol = saved_cnt_pnt;

  /* Realy draw the control points */
  draw_circle (&outer_pnt->pnt);
  draw_circle (&inner_pnt->pnt);
}

void
d_star_start (GdkPoint *pnt,
	      gint      shift_down)
{
  obj_creating = d_new_star ((gint) pnt->x, (gint) pnt->y);
  obj_creating->type_data = star_num_sides;
}

void
d_star_end (GdkPoint *pnt,
	    gint      shift_down)
{
  draw_circle (pnt);
  add_to_all_obj (current_obj, obj_creating);
  obj_creating = NULL;
}

