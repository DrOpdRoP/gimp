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

#include "libgimpbase/gimpbase.h"

#include "plug-in-types.h"

#include "core/gimp.h"

#include "plug-in.h"
#include "plug-ins.h"
#include "plug-in-proc.h"

#include "gimp-intl.h"


PlugInProcDef *
plug_in_proc_def_new (void)
{
  return g_new0 (PlugInProcDef, 1);
}

void
plug_in_proc_def_free (PlugInProcDef *proc_def)
{
  gint i;

  g_return_if_fail (proc_def != NULL);

  g_free (proc_def->db_info.name);
  g_free (proc_def->db_info.blurb);
  g_free (proc_def->db_info.help);
  g_free (proc_def->db_info.author);
  g_free (proc_def->db_info.copyright);
  g_free (proc_def->db_info.date);

  for (i = 0; i < proc_def->db_info.num_args; i++)
    {
      g_free (proc_def->db_info.args[i].name);
      g_free (proc_def->db_info.args[i].description);
    }

  for (i = 0; i < proc_def->db_info.num_values; i++)
    {
      g_free (proc_def->db_info.values[i].name);
      g_free (proc_def->db_info.values[i].description);
    }

  g_free (proc_def->db_info.args);
  g_free (proc_def->db_info.values);

  g_free (proc_def->prog);
  g_free (proc_def->menu_label);

  g_list_foreach (proc_def->menu_paths, (GFunc) g_free, NULL);
  g_list_free (proc_def->menu_paths);

  g_free (proc_def->extensions);
  g_free (proc_def->prefixes);
  g_free (proc_def->magics);
  g_free (proc_def->mime_type);
  g_free (proc_def->image_types);

  g_free (proc_def);
}

const ProcRecord *
plug_in_proc_def_get_proc (const PlugInProcDef *proc_def)
{
  g_return_val_if_fail (proc_def != NULL, NULL);

  return &proc_def->db_info;
}

const gchar *
plug_in_proc_def_get_progname (const PlugInProcDef *proc_def)
{
  g_return_val_if_fail (proc_def != NULL, NULL);

  switch (proc_def->db_info.proc_type)
    {
    case GIMP_PLUGIN:
    case GIMP_EXTENSION:
      return proc_def->prog;

    case GIMP_TEMPORARY:
      return ((PlugIn *) proc_def->db_info.exec_method.temporary.plug_in)->prog;

    default:
      break;
    }

  return NULL;
}

gchar *
plug_in_proc_def_get_help_id (const PlugInProcDef *proc_def,
                              const gchar         *help_domain)
{
  gchar *help_id;
  gchar *p;

  g_return_val_if_fail (proc_def != NULL, NULL);

  help_id = g_strdup (proc_def->db_info.name);

  for (p = help_id; p && *p; p++)
    if (*p == '_')
      *p = '-';

  if (help_domain)
    {
      gchar *domain_and_id;

      domain_and_id = g_strconcat (help_domain, "?", help_id, NULL);
      g_free (help_id);

      return domain_and_id;
    }

  return help_id;
}
