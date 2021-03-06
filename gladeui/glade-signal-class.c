/* glade-signal-class.c
 *
 * Copyright (C) 2011 Openismus GmbH
 *
 * Authors:
 *      Tristan Van Berkom <tristanvb@openismus.com>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#include <config.h>
#include "glade-signal-class.h"
#include "glade-widget-adaptor.h"

struct _GladeSignalClass
{
  GladeWidgetAdaptor *adaptor; /* The adaptor that originated this signal.
                                */
  GSignalQuery        query;

  guint16             version_since_major; /* Version in which this signal was */
  guint16             version_since_minor; /* introduced
                                            */

  const gchar        *name;                /* Name of the signal, eg clicked */
  const gchar        *type;                /* Name of the object class that this signal 
                                            * belongs to eg GtkButton */

  guint deprecated : 1;                    /* True if this signal is deprecated */
};

GladeSignalClass *
glade_signal_class_new  (GladeWidgetAdaptor *adaptor,
                         GType               for_type,
                         guint               signal_id)
{
  GladeSignalClass *class;

  class = g_slice_new0 (GladeSignalClass);

  class->adaptor = adaptor;

  /* Since glib gave us this signal id... it should
   * exist no matter what.
   */
  g_signal_query (signal_id, &(class->query));
  if (class->query.signal_id == 0)
    {
      g_critical ("glade_signal_class_new() called with an invalid signal id");

      glade_signal_class_free (class);
      return NULL;
    }

  class->name = (class->query.signal_name);
  class->type = g_type_name (for_type);

  /* Initialize signal versions & deprecated to adaptor version */
  class->version_since_major = GWA_VERSION_SINCE_MAJOR (adaptor);
  class->version_since_minor = GWA_VERSION_SINCE_MINOR (adaptor);
  class->deprecated          = GWA_DEPRECATED (adaptor);

  return class;
}

void
glade_signal_class_free (GladeSignalClass *signal_class)
{
  g_slice_free (GladeSignalClass, signal_class);
}

void
glade_signal_class_update_from_node (GladeSignalClass *signal_class,
                                     GladeXmlNode     *node,
                                     const gchar      *domain)
{
  g_return_if_fail (signal_class != NULL);
  g_return_if_fail (node != NULL);

  glade_xml_get_property_version (node, GLADE_TAG_VERSION_SINCE,
                                  &signal_class->version_since_major, 
                                  &signal_class->version_since_minor);

  signal_class->deprecated =
    glade_xml_get_property_boolean (node,
                                    GLADE_TAG_DEPRECATED,
                                    signal_class->deprecated);
}

GladeWidgetAdaptor *
glade_signal_class_get_adaptor (const GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, NULL);

  return signal_class->adaptor;
}

G_CONST_RETURN gchar *
glade_signal_class_get_name (const GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, NULL);

  return signal_class->name;
}

G_CONST_RETURN gchar *
glade_signal_class_get_type (const GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, NULL);

  return signal_class->type;
}

GSignalFlags
glade_signal_class_get_flags (const GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, 0);

  return signal_class->query.signal_flags;
}

void
glade_signal_class_set_since (GladeSignalClass *signal_class,
                              guint16           since_major,
                              guint16           since_minor)
{
  g_return_if_fail (signal_class != NULL);

  signal_class->version_since_major = since_major;
  signal_class->version_since_minor = since_minor;
}

guint16
glade_signal_class_since_major (GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, 0);

  return signal_class->version_since_major;
}

guint16
glade_signal_class_since_minor (GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, 0);

  return signal_class->version_since_minor;
}

void
glade_signal_class_set_deprecated (GladeSignalClass *signal_class,
                                   gboolean          deprecated)
{
  g_return_if_fail (signal_class != NULL);

  signal_class->deprecated = deprecated;
}

gboolean
glade_signal_class_deprecated (GladeSignalClass *signal_class)
{
  g_return_val_if_fail (signal_class != NULL, FALSE);

  return signal_class->deprecated;
}
