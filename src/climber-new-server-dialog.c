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
  GtkEntry *hostname;
  GtkSpinButton *port;
  GtkButton *filechooser;
};

G_DEFINE_FINAL_TYPE(ClimberNewServerDialog, climber_new_server_dialog,
                    GTK_TYPE_DIALOG)

static void climber_new_server_filechooser_click_handler(GtkButton *button,
                                                         gpointer user_data);

static void
climber_new_server_dialog_class_init(ClimberNewServerDialogClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class,
      CLIMBER_APPLICATION_PATH "/gtk/climber-new-server-dialog.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       hostname);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       port);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       filechooser);
}

static void climber_new_server_dialog_init(ClimberNewServerDialog *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-new-server-dialog.css");

  g_signal_connect(G_OBJECT(self->filechooser), "clicked",
                   G_CALLBACK(climber_new_server_filechooser_click_handler),
                   self);
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

static void on_filechoser_open_response(GtkDialog *dialog, int response,
                                        gpointer user_data) {
  if (response == GTK_RESPONSE_ACCEPT) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    GFile *file = gtk_file_chooser_get_file(chooser);
    if (file != NULL) {
      gchar *path = g_file_get_path(file);
      g_object_unref(file);
      g_print("file path: %s\n", path);
      g_free(path);
    }
  }

  gtk_window_destroy(GTK_WINDOW(dialog));
}

static void climber_new_server_filechooser_click_handler(GtkButton *button,
                                                         gpointer user_data) {
  ClimberNewServerDialog *self = CLIMBER_NEW_SERVER_DIALOG(user_data);
  GtkWidget *dialog = NULL;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_add_suffix(filter, "crt");
  dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(self), action,
                                       "_Cancel", GTK_RESPONSE_CANCEL, "_Open",
                                       GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);

  gtk_window_present(GTK_WINDOW(dialog));
  g_signal_connect(G_OBJECT(dialog), "response",
                   G_CALLBACK(on_filechoser_open_response), NULL);
}

