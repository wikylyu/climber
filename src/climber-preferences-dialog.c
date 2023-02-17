/* climber-preferences-dialog.c
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

#include "climber-preferences-dialog.h"
#include "config.h"
#include "style.h"

struct _ClimberPreferencesDialog {
  GtkDialog parent_instance;

  /* Template widgets */
  GtkSpinButton *socks5_port;
  GtkSpinButton *http_port;
};

G_DEFINE_FINAL_TYPE(ClimberPreferencesDialog, climber_preferences_dialog,
                    GTK_TYPE_DIALOG)

static void
climber_preferences_dialog_class_init(ClimberPreferencesDialogClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class,
      CLIMBER_APPLICATION_PATH "/gtk/climber-preferences-dialog.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberPreferencesDialog,
                                       socks5_port);
  gtk_widget_class_bind_template_child(widget_class, ClimberPreferencesDialog,
                                       http_port);
}

static void climber_preferences_dialog_init(ClimberPreferencesDialog *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-preferences-dialog.css");
}

ClimberPreferencesDialog *climber_preferences_dialog_new(GtkApplication *app) {
  GObject *obj;
  GSettings *settings;
  gint socks5_port, http_port;
  ClimberPreferencesDialog *dialog;

  obj = g_object_new(CLIMBER_TYPE_PREFERENCES_DIALOG, "use-header-bar", TRUE,
                     "application", app, "modal", TRUE, "transient-for",
                     gtk_application_get_active_window(app), NULL);

  dialog = CLIMBER_PREFERENCES_DIALOG(obj);

  settings = g_settings_new(CLIMBER_APPLICATION_ID);

  socks5_port = g_settings_get_int(settings, "socks5-port");
  http_port = g_settings_get_int(settings, "http-port");

  gtk_spin_button_set_value(dialog->socks5_port, socks5_port);
  gtk_spin_button_set_value(dialog->http_port, http_port);

  g_object_unref(settings);

  return dialog;
}

gint climber_preferences_dialog_get_socks5_port(
    ClimberPreferencesDialog *dialog) {
  return gtk_spin_button_get_value_as_int(dialog->socks5_port);
}

gint climber_preferences_dialog_get_http_port(
    ClimberPreferencesDialog *dialog) {
  return gtk_spin_button_get_value_as_int(dialog->http_port);
}
