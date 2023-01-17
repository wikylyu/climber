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

#include "config.h"

#include "climber-preferences-dialog.h"
#include "style.h"

static ClimberPreferencesDialog *instance = NULL;

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
      widget_class, "/xyz/wikylyu/climber/gtk/climber-preferences-dialog.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberPreferencesDialog,
                                       socks5_port);
  gtk_widget_class_bind_template_child(widget_class, ClimberPreferencesDialog,
                                       http_port);
}

static void climber_preferences_dialog_init(ClimberPreferencesDialog *self) {
  gtk_widget_init_template(GTK_WIDGET(self));
}

/*
 * Returns FALSE if error occurs.
 */
static gboolean
climber_preferences_dialog_confirm(ClimberPreferencesDialog *self) {
  g_print("Confirm!\n");
  return true;
}

static void climber_preferences_dialog_response_handler(
    ClimberPreferencesDialog *self, gint response_id, gpointer user_data) {

  if (response_id == GTK_RESPONSE_OK) {
    if (!climber_preferences_dialog_confirm(self)) {
      return;
    }
  }
  gtk_window_destroy(GTK_WINDOW(self));
  instance = NULL;
}

ClimberPreferencesDialog *climber_preferences_dialog_new(GtkApplication *app) {
  GtkCssProvider *css_provider;
  GObject *obj;

  if (instance != NULL) {
    return instance;
  }

  css_provider = gtk_css_provider_new();
  obj = g_object_new(CLIMBER_TYPE_PREFERENCES_DIALOG, "use-header-bar", TRUE,
                     "application", app, "modal", TRUE, "transient-for",
                     gtk_application_get_active_window(app), NULL);
  g_signal_connect(obj, "response",
                   G_CALLBACK(climber_preferences_dialog_response_handler),
                   NULL);
  gtk_css_provider_load_from_resource(
      css_provider, "/xyz/wikylyu/climber/gtk/climber-preferences-dialog.css");

  gtk_widget_apply_css_all(GTK_WIDGET(obj), GTK_STYLE_PROVIDER(css_provider));
  instance = CLIMBER_PREFERENCES_DIALOG(obj);

  gtk_spin_button_set_value(instance->socks5_port, 1080);
  gtk_spin_button_set_value(instance->http_port, 1081);

  return instance;
}

