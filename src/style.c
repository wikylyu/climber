/* style.c
 *
 * Copyright 2023 Wiky Lyu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "style.h"

static void _gtk_widget_apply_css_all(GtkWidget *widget,
                                      GtkStyleProvider *provider) {
  GtkWidget *child;

  gtk_style_context_add_provider(gtk_widget_get_style_context(widget), provider,
                                 GTK_STYLE_PROVIDER_PRIORITY_USER);
  for (child = gtk_widget_get_first_child(widget); child != NULL;
       child = gtk_widget_get_next_sibling(child))
    _gtk_widget_apply_css_all(child, provider);
}

void gtk_widget_apply_css_all(GtkWidget *widget, const gchar *css_path) {
  GtkCssProvider *css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(css_provider, css_path);
  _gtk_widget_apply_css_all(widget, GTK_STYLE_PROVIDER(css_provider));
}

