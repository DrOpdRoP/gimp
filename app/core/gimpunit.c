/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1999 Spencer Kimball and Peter Mattis
 *
 * gimpunit.c
 * Copyright (C) 1999-2000 Michael Natterer <mitch@gimp.org>
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

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include "libgimpbase/gimpbase.h"

#include "core-types.h"

#include "gimp.h"
#include "gimpunit.h"

#include "gimprc.h"

#include "libgimp/gimpintl.h"


/* internal structures */

typedef struct
{
  gboolean  delete_on_exit;
  gdouble   factor;
  gint      digits;
  gchar    *identifier;
  gchar    *symbol;
  gchar    *abbreviation;
  gchar    *singular;
  gchar    *plural;
} GimpUnitDef;

/*  these are the built-in units
 */
static GimpUnitDef gimp_unit_defs[GIMP_UNIT_END] =
{
  /* pseudo unit */
  { FALSE,  0.0, 0, "pixels",      "px", "px", N_("pixel"),      N_("pixels") },

  /* standard units */
  { FALSE,  1.0, 2, "inches",      "''", "in", N_("inch"),       N_("inches") },
  { FALSE, 25.4, 1, "millimeters", "mm", "mm", N_("millimeter"), N_("millimeters") },

  /* professional units */
  { FALSE, 72.0, 0, "points",      "pt", "pt", N_("point"),      N_("points") },
  { FALSE,  6.0, 1, "picas",       "pc", "pc", N_("pica"),       N_("picas") },
};

/*  not a unit at all but kept here to have the strings in one place
 */
static GimpUnitDef gimp_unit_percent =
{
  FALSE,    0.0, 0, "percent",     "%",  "%",  N_("percent"),    N_("percent")
};


/* private functions */

static GimpUnitDef *
_gimp_unit_get_user_unit (Gimp     *gimp,
			  GimpUnit  unit)
{
  return g_list_nth_data (gimp->user_units, unit - GIMP_UNIT_END);
}


/* public functions */

gint
_gimp_unit_get_number_of_units (Gimp *gimp)
{
  return GIMP_UNIT_END + gimp->n_user_units;
}

gint
_gimp_unit_get_number_of_built_in_units (Gimp *gimp)
{
  return GIMP_UNIT_END;
}

GimpUnit
_gimp_unit_new (Gimp    *gimp,
		gchar   *identifier,
		gdouble  factor,
		gint     digits,
		gchar   *symbol,
		gchar   *abbreviation,
		gchar   *singular,
		gchar   *plural)
{
  GimpUnitDef *user_unit;

  user_unit = g_new0 (GimpUnitDef, 1);

  user_unit->delete_on_exit = TRUE;
  user_unit->factor         = factor;
  user_unit->digits         = digits;
  user_unit->identifier     = g_strdup (identifier);
  user_unit->symbol         = g_strdup (symbol);
  user_unit->abbreviation   = g_strdup (abbreviation);
  user_unit->singular       = g_strdup (singular);
  user_unit->plural         = g_strdup (plural);

  gimp->user_units = g_list_append (gimp->user_units, user_unit);
  gimp->n_user_units++;

  return GIMP_UNIT_END + gimp->n_user_units - 1;
}

gboolean
_gimp_unit_get_deletion_flag (Gimp     *gimp,
			      GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) && 
			(unit < (GIMP_UNIT_END + gimp->n_user_units)), FALSE);

  if (unit < GIMP_UNIT_END)
    return FALSE;

  return _gimp_unit_get_user_unit (gimp, unit)->delete_on_exit;
}

void
_gimp_unit_set_deletion_flag (Gimp     *gimp,
			      GimpUnit  unit,
			      gboolean  deletion_flag)
{
  g_return_if_fail ((unit >= GIMP_UNIT_END) && 
		    (unit < (GIMP_UNIT_END + gimp->n_user_units)));

  _gimp_unit_get_user_unit (gimp, unit)->delete_on_exit = 
    deletion_flag ? TRUE : FALSE;
}

gdouble
_gimp_unit_get_factor (Gimp     *gimp,
		       GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) && 
			(unit < (GIMP_UNIT_END + gimp->n_user_units)),
			gimp_unit_defs[GIMP_UNIT_INCH].factor);

  if (unit < GIMP_UNIT_END)
    return gimp_unit_defs[unit].factor;

  return _gimp_unit_get_user_unit (gimp, unit)->factor;
}

gint
_gimp_unit_get_digits (Gimp     *gimp,
		       GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) &&
			(unit < (GIMP_UNIT_END + gimp->n_user_units)),
			gimp_unit_defs[GIMP_UNIT_INCH].digits);

  if (unit < GIMP_UNIT_END)
    return gimp_unit_defs[unit].digits;

  return _gimp_unit_get_user_unit (gimp, unit)->digits;
}

gchar * 
_gimp_unit_get_identifier (Gimp     *gimp,
			   GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) && 
			(unit < (GIMP_UNIT_END + gimp->n_user_units)) ||
			(unit == GIMP_UNIT_PERCENT),
			gimp_unit_defs[GIMP_UNIT_INCH].identifier);

  if (unit < GIMP_UNIT_END)
    return gimp_unit_defs[unit].identifier;

  if (unit == GIMP_UNIT_PERCENT)
    return gimp_unit_percent.identifier;

  return _gimp_unit_get_user_unit (gimp, unit)->identifier;
}

gchar *
_gimp_unit_get_symbol (Gimp     *gimp,
		       GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) &&
			(unit < (GIMP_UNIT_END + gimp->n_user_units)) ||
			(unit == GIMP_UNIT_PERCENT),
			gimp_unit_defs[GIMP_UNIT_INCH].symbol);

  if (unit < GIMP_UNIT_END)
    return gimp_unit_defs[unit].symbol;

  if (unit == GIMP_UNIT_PERCENT)
    return gimp_unit_percent.symbol;

  return _gimp_unit_get_user_unit (gimp, unit)->symbol;
}

gchar *
_gimp_unit_get_abbreviation (Gimp     *gimp,
			     GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) &&
			(unit < (GIMP_UNIT_END + gimp->n_user_units)) ||
			(unit == GIMP_UNIT_PERCENT),
			gimp_unit_defs[GIMP_UNIT_INCH].abbreviation);

  if (unit < GIMP_UNIT_END)
    return gimp_unit_defs[unit].abbreviation;

  if (unit == GIMP_UNIT_PERCENT)
    return gimp_unit_percent.abbreviation;

  return _gimp_unit_get_user_unit (gimp, unit)->abbreviation;
}

gchar *
_gimp_unit_get_singular (Gimp     *gimp,
			 GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) &&
			(unit < (GIMP_UNIT_END + gimp->n_user_units)) ||
			(unit == GIMP_UNIT_PERCENT),
			gettext (gimp_unit_defs[GIMP_UNIT_INCH].singular));

  if (unit < GIMP_UNIT_END)
    return gettext (gimp_unit_defs[unit].singular);

  if (unit == GIMP_UNIT_PERCENT)
    return gettext (gimp_unit_percent.singular);

  return gettext (_gimp_unit_get_user_unit (gimp, unit)->singular);
}

gchar *
_gimp_unit_get_plural (Gimp     *gimp,
		       GimpUnit  unit)
{
  g_return_val_if_fail ((unit >= GIMP_UNIT_PIXEL) &&
			(unit < (GIMP_UNIT_END + gimp->n_user_units)) ||
			(unit == GIMP_UNIT_PERCENT),
			gettext (gimp_unit_defs[GIMP_UNIT_INCH].plural));

  if (unit < GIMP_UNIT_END)
    return gettext (gimp_unit_defs[unit].plural);

  if (unit == GIMP_UNIT_PERCENT)
    return gettext (gimp_unit_percent.plural);

  return gettext (_gimp_unit_get_user_unit (gimp, unit)->plural);
}


/*  unitrc functions  **********/

void
gimp_units_init (Gimp *gimp)
{
  g_return_if_fail (gimp != NULL);
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  gimp->user_units   = NULL;
  gimp->n_user_units = 0;
}

void
gimp_units_exit (Gimp *gimp)
{
  g_return_if_fail (gimp != NULL);
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (gimp->user_units)
    {
      g_list_foreach (gimp->user_units, (GFunc) g_free, NULL);
      g_list_free (gimp->user_units);

      gimp->user_units   = NULL;
      gimp->n_user_units = 0;
    }
}

void
gimp_unitrc_load (Gimp *gimp)
{
  gchar *filename;

  g_return_if_fail (gimp != NULL);
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  filename = gimp_personal_rc_file ("unitrc");
  gimprc_parse_file (filename);
  g_free (filename);
}

void
gimp_unitrc_save (Gimp *gimp)
{
  gint   i;
  gchar *filename;
  FILE  *fp;

  g_return_if_fail (gimp != NULL);
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  filename = gimp_personal_rc_file ("unitrc");

  fp = fopen (filename, "w");
  g_free (filename);
  if (!fp)
    return;

  fprintf (fp,
	   "# GIMP unitrc\n" 
	   "# This file contains your user unit database. You can\n"
	   "# modify this list with the unit editor. You are not\n"
	   "# supposed to edit it manually, but of course you can do.\n"
	   "# This file will be entirely rewritten every time you\n"
	   "# quit the gimp.\n\n");

  /*  save user defined units  */
  for (i = gimp_unit_get_number_of_built_in_units ();
       i < gimp_unit_get_number_of_units ();
       i++)
    if (gimp_unit_get_deletion_flag (i) == FALSE)
      {
	fprintf (fp,
		 "(unit-info \"%s\"\n"
		 "   (factor %f)\n"
		 "   (digits %d)\n"
		 "   (symbol \"%s\")\n"
		 "   (abbreviation \"%s\")\n"
		 "   (singular \"%s\")\n"
		 "   (plural \"%s\"))\n\n",
		 gimp_unit_get_identifier (i),
		 gimp_unit_get_factor (i),
		 gimp_unit_get_digits (i),
		 gimp_unit_get_symbol (i),
		 gimp_unit_get_abbreviation (i),
		 gimp_unit_get_singular (i),
		 gimp_unit_get_plural (i));
      }

  fclose (fp);
}
