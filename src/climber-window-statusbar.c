/* climber-window-statusbar.c
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
#include "config.h"

#include "climber-window-statusbar.h"
#include "style.h"

struct _ClimberWindowStatusbar {
  GtkBox parent_instance;
  GtkLabel *socks5_label;
  GtkLabel *http_label;
};

G_DEFINE_FINAL_TYPE(ClimberWindowStatusbar, climber_window_statusbar,
                    GTK_TYPE_BOX)

static void
climber_window_statusbar_class_init(ClimberWindowStatusbarClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class,
      CLIMBER_APPLICATION_PATH "/gtk/climber-window-statusbar.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberWindowStatusbar,
                                       socks5_label);
  gtk_widget_class_bind_template_child(widget_class, ClimberWindowStatusbar,
                                       http_label);
}

static void climber_window_statusbar_init(ClimberWindowStatusbar *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-window-statusbar.css");
}

void climber_window_statusbar_update(ClimberWindowStatusbar *self,
                                     gint socks5_port, gint http_port) {
  gchar strbuf[1024];
  g_snprintf(strbuf, sizeof(strbuf) / sizeof(gchar), "SOCKS5 port: <b>%d</b>",
             socks5_port);
  gtk_label_set_markup(self->socks5_label, strbuf);
  g_snprintf(strbuf, sizeof(strbuf) / sizeof(gchar), "HTTP port: <b>%d</b>",
             http_port);
  gtk_label_set_markup(self->http_label, strbuf);
}
