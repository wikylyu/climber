/* climber-new-server-dialog.c
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
#include "climber-new-server-dialog.h"
#include "config.h"
#include "style.h"

struct _ClimberNewServerDialog {
  GtkDialog parent_instance;

  /* Template widgets */
  GtkSpinButton *port;
};

G_DEFINE_FINAL_TYPE(ClimberNewServerDialog, climber_new_server_dialog,
                    GTK_TYPE_DIALOG)

static void
climber_new_server_dialog_class_init(ClimberNewServerDialogClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class,
      CLIMBER_APPLICATION_PATH "/gtk/climber-new-server-dialog.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       port);
}

static void climber_new_server_dialog_init(ClimberNewServerDialog *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-new-server-dialog.css");
}

ClimberNewServerDialog *climber_new_server_dialog_new(GtkWindow *win) {
  GObject *obj;
  ClimberNewServerDialog *dialog;

  obj = g_object_new(CLIMBER_TYPE_NEW_SERVER_DIALOG, "use-header-bar", TRUE,
                     "application", gtk_window_get_application(win), "modal",
                     TRUE, "transient-for", win, NULL);

  dialog = CLIMBER_NEW_SERVER_DIALOG(obj);

  return dialog;
}

