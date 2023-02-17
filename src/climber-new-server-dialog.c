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
#include "io/mtop/server-config.h"

#include "style.h"

struct _ClimberNewServerDialog {
  GtkDialog parent_instance;

  /* Template widgets */

  GtkEntry *hostname;
  GtkSpinButton *port;
  GtkEntry *username;
  GtkEntry *password;
  GtkEntry *proto;
  GtkDropDown *type;
  GtkButton *filechooser;
  GtkButton *fileclear;
  GFile *ca_file;
};

G_DEFINE_FINAL_TYPE(ClimberNewServerDialog, climber_new_server_dialog,
                    GTK_TYPE_DIALOG)

static void climber_new_server_filechooser_click_handler(GtkButton *button,
                                                         gpointer user_data);
static void climber_new_server_fileclear_click_handler(GtkButton *button,
                                                       gpointer user_data);
static void climber_new_server_dialog_finalize(GObject *object);
static void climber_new_server_dialog_response_handler(
    ClimberNewServerDialog *dialog, gint response_id, gpointer user_data);
static void
climber_new_server_dialog_text_changed_handler(GtkEditable *editable,
                                               gpointer user_data);
static void
climber_new_server_dialog_spin_changed_handler(GtkSpinButton *spin_button,
                                               gpointer user_data);

static void
climber_new_server_dialog_class_init(ClimberNewServerDialogClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  G_OBJECT_CLASS(klass)->finalize = climber_new_server_dialog_finalize;

  gtk_widget_class_set_template_from_resource(
      widget_class,
      CLIMBER_APPLICATION_PATH "/gtk/climber-new-server-dialog.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       hostname);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       port);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       username);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       password);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       proto);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       type);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       filechooser);
  gtk_widget_class_bind_template_child(widget_class, ClimberNewServerDialog,
                                       fileclear);

  g_signal_new("confirm", G_TYPE_FROM_CLASS(klass),
               G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
               0, /* class offset */
               NULL /* accumulator */, NULL /* accumulator data */,
               NULL /* C marshaller */, G_TYPE_NONE /* return_type */,
               1 /* n_params */, MTOP_TYPE_SERVER_CONFIG /* param_types */);
}

static void climber_new_server_dialog_confirm(GtkButton *button,
                                              gpointer user_data) {
  g_print("hello \n");
}

static void climber_new_server_dialog_init(ClimberNewServerDialog *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-new-server-dialog.css");

  g_signal_connect(G_OBJECT(self->filechooser), "clicked",
                   G_CALLBACK(climber_new_server_filechooser_click_handler),
                   self);
  g_signal_connect(G_OBJECT(self->fileclear), "clicked",
                   G_CALLBACK(climber_new_server_fileclear_click_handler),
                   self);

  g_signal_connect(G_OBJECT(self), "response",
                   G_CALLBACK(climber_new_server_dialog_response_handler),
                   self);
  g_signal_connect(G_OBJECT(self->hostname), "changed",
                   G_CALLBACK(climber_new_server_dialog_text_changed_handler),
                   self);
  g_signal_connect(G_OBJECT(self->username), "changed",
                   G_CALLBACK(climber_new_server_dialog_text_changed_handler),
                   self);
  g_signal_connect(G_OBJECT(self->password), "changed",
                   G_CALLBACK(climber_new_server_dialog_text_changed_handler),
                   self);
  g_signal_connect(G_OBJECT(self->port), "value-changed",
                   G_CALLBACK(climber_new_server_dialog_spin_changed_handler),
                   self);
}

static void climber_new_server_dialog_finalize(GObject *object) {
  ClimberNewServerDialog *self = CLIMBER_NEW_SERVER_DIALOG(object);
  g_object_unref(self->ca_file);
  G_OBJECT_CLASS(climber_new_server_dialog_parent_class)->finalize(object);
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
  ClimberNewServerDialog *self = CLIMBER_NEW_SERVER_DIALOG(user_data);
  if (response == GTK_RESPONSE_ACCEPT) {
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    GFile *file = gtk_file_chooser_get_file(chooser);
    if (file != NULL) {
      gchar *basename = NULL;
      if (self->ca_file != NULL) {
        g_object_unref(self->ca_file);
        self->ca_file = NULL;
      }
      self->ca_file = file;

      basename = g_file_get_basename(file);
      gtk_button_set_label(self->filechooser, basename);
      g_free(basename);
      gtk_widget_set_visible(GTK_WIDGET(self->fileclear), TRUE);
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
                   G_CALLBACK(on_filechoser_open_response), self);
}

static void climber_new_server_fileclear_click_handler(GtkButton *button,
                                                       gpointer user_data) {
  ClimberNewServerDialog *self = CLIMBER_NEW_SERVER_DIALOG(user_data);
  g_object_unref(self->ca_file);
  self->ca_file = NULL;
  gtk_button_set_label(self->filechooser, "None");
}

static gchar *getTrimText(const gchar *text) {
  if (text == NULL) {
    return NULL;
  }
  return g_strstrip(g_strdup(text));
}

static void climber_new_server_dialog_response_handler(
    ClimberNewServerDialog *self, gint response_id, gpointer user_data) {
  gchar *hostname = NULL;
  gchar *username = NULL;
  gchar *password = NULL;
  gchar *proto = NULL;
  gchar *type = NULL;
  gchar *ca = NULL;
  gint port = 0;
  gboolean error = FALSE;
  MtopServerConfig *config = NULL;
  if (response_id != GTK_RESPONSE_OK) {
    g_signal_emit_by_name(self, "confirm", NULL);
    return;
  }
  hostname = getTrimText(gtk_editable_get_text(GTK_EDITABLE(self->hostname)));
  if (hostname == NULL || strlen(hostname) == 0) {
    gtk_widget_add_css_class(GTK_WIDGET(self->hostname), "error");
    error = TRUE;
  }
  port = gtk_spin_button_get_value_as_int(self->port);
  if (port <= 0 || port > G_MAXUINT16) {
    gtk_widget_add_css_class(GTK_WIDGET(self->port), "error");
    error = TRUE;
  }
  username = getTrimText(gtk_editable_get_text(GTK_EDITABLE(self->username)));
  if (username == NULL || strlen(username) == 0) {
    gtk_widget_add_css_class(GTK_WIDGET(self->username), "error");
    error = TRUE;
  }
  password = getTrimText(gtk_editable_get_text(GTK_EDITABLE(self->password)));
  if (password == NULL || strlen(password) == 0) {
    gtk_widget_add_css_class(GTK_WIDGET(self->password), "error");
    error = TRUE;
  }
  if (error) {
    g_free(hostname);
    g_free(username);
    g_free(password);
    return;
  }
  proto = getTrimText(gtk_editable_get_text(GTK_EDITABLE(self->proto)));
  if (gtk_drop_down_get_selected(self->type) == 0) {
    type = g_strdup("tls");
  } else {
    type = g_strdup("quic");
  }
  if (self->ca_file != NULL) {
    ca = g_file_get_path(self->ca_file);
  }
  config = mtop_server_config_new(hostname, (gushort)port, username, password,
                                  type, proto, ca);
  g_free(hostname);
  g_free(username);
  g_free(password);
  g_free(proto);
  g_free(type);
  g_free(ca);
  g_signal_emit_by_name(self, "confirm", config);
}

static void
climber_new_server_dialog_text_changed_handler(GtkEditable *editable,
                                               gpointer user_data) {
  gchar *text = getTrimText(gtk_editable_get_text(editable));
  if (text == NULL || strlen(text) == 0) {
    g_free(text);
    return;
  }
  g_free(text);
  gtk_widget_remove_css_class(GTK_WIDGET(editable), "error");
}

static void
climber_new_server_dialog_spin_changed_handler(GtkSpinButton *spin_button,
                                               gpointer user_data) {
  gint port = gtk_spin_button_get_value_as_int(spin_button);
  if (port > 0 && port <= G_MAXUINT16) {
    gtk_widget_remove_css_class(GTK_WIDGET(spin_button), "error");
  }
}

