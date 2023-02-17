/* climber-window.c
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

#include "climber-log-view.h"
#include "climber-new-server-dialog.h"
#include "climber-preferences-dialog.h"
#include "climber-service-switch.h"
#include "climber-window-statusbar.h"
#include "climber-window.h"
#include "io/climber-service.h"
#include "io/mtop/server-config.h"
#include "message.h"
#include "style.h"

struct _ClimberWindow {
  AdwApplicationWindow parent_instance;

  /* Template widgets */
  GtkHeaderBar *header_bar;
  GtkLabel *label;
  ClimberPreferencesDialog *preferences_dialog;
  ClimberNewServerDialog *new_server_dialog;
  ClimberServiceSwitch *service_switch;
  ClimberLogView *logview;
  ClimberWindowStatusbar *statusbar;

  ClimberService *service;
};

G_DEFINE_FINAL_TYPE(ClimberWindow, climber_window, ADW_TYPE_APPLICATION_WINDOW)

static void climber_service_switch_state_changed_handler(
    ClimberServiceSwitch *widget, gboolean state, gpointer user_data);
static void climber_service_log_handler(ClimberService *service,
                                        const gchar *message,
                                        gpointer user_data);
static void climber_window_finalize(GObject *object);

static void climber_preferences_dialog_response_handler(
    ClimberPreferencesDialog *dialog, gint response_id, gpointer user_data);
static void
climber_new_server_dialog_confirm_handler(ClimberNewServerDialog *dialog,
                                          MtopServerConfig *config,
                                          gpointer user_data);

static void climber_window_class_init(ClimberWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class, CLIMBER_APPLICATION_PATH "/gtk/climber-window.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberWindow, header_bar);
  gtk_widget_class_bind_template_child(widget_class, ClimberWindow, label);
  gtk_widget_class_bind_template_child(widget_class, ClimberWindow, statusbar);
  gtk_widget_class_bind_template_child(widget_class, ClimberWindow, logview);

  G_OBJECT_CLASS(widget_class)->finalize = climber_window_finalize;
}

static void climber_window_new_server_action(GSimpleAction *action,
                                             GVariant *parameter,
                                             gpointer user_data) {
  ClimberWindow *self = user_data;

  g_assert(CLIMBER_IS_WINDOW(self));

  if (self->new_server_dialog == NULL) {
    self->new_server_dialog = climber_new_server_dialog_new(GTK_WINDOW(self));
    g_signal_connect(G_OBJECT(self->new_server_dialog), "confirm",
                     G_CALLBACK(climber_new_server_dialog_confirm_handler),
                     self);
  }

  gtk_window_present(GTK_WINDOW(self->new_server_dialog));
}

static void climber_window_new_subscription_action(GSimpleAction *action,
                                                   GVariant *parameter,
                                                   gpointer user_data) {
  ClimberWindow *self = user_data;

  g_assert(CLIMBER_IS_WINDOW(self));
}

static void climber_window_save_preferences(gint socks5_port, gint http_port) {
  GSettings *settings;
  settings = g_settings_new(CLIMBER_APPLICATION_ID);
  g_settings_set_int(settings, "socks5-port", socks5_port);
  g_settings_set_int(settings, "http-port", http_port);
  g_object_unref(settings);
}

/*
 * Open preferences dialog
 */
static void climber_window_preference_action(GSimpleAction *action,
                                             GVariant *parameter,
                                             gpointer user_data) {
  ClimberWindow *self = user_data;

  g_assert(CLIMBER_IS_WINDOW(self));

  if (self->preferences_dialog == NULL) {
    self->preferences_dialog = climber_preferences_dialog_new(
        gtk_window_get_application(GTK_WINDOW(self)));
    g_signal_connect(G_OBJECT(self->preferences_dialog), "response",
                     G_CALLBACK(climber_preferences_dialog_response_handler),
                     self);
  }
  gtk_window_present(GTK_WINDOW(self->preferences_dialog));
}

static const GActionEntry win_actions[] = {
    {"new-server", climber_window_new_server_action},
    {"new-subscription", climber_window_new_subscription_action},
    {"preferences", climber_window_preference_action},
};

static void climber_window_init(ClimberWindow *self) {
  GSettings *settings;
  gint socks5_port;
  gint http_port;

  g_type_ensure(CLIMBER_TYPE_LOG_VIEW);
  g_type_ensure(CLIMBER_TYPE_WINDOW_STATUSBAR);
  gtk_widget_init_template(GTK_WIDGET(self));

  self->service_switch = climber_service_switch_new();
  gtk_header_bar_set_title_widget(self->header_bar,
                                  GTK_WIDGET(self->service_switch));
  g_signal_connect(G_OBJECT(self->service_switch), "state-changed",
                   G_CALLBACK(climber_service_switch_state_changed_handler),
                   self);

  gtk_widget_apply_css_all(GTK_WIDGET(self),
                           CLIMBER_APPLICATION_PATH "/gtk/climber-window.css");
  g_action_map_add_action_entries(G_ACTION_MAP(self), win_actions,
                                  G_N_ELEMENTS(win_actions), self);

  settings = g_settings_new(CLIMBER_APPLICATION_ID);
  socks5_port = g_settings_get_int(settings, "socks5-port");
  http_port = g_settings_get_int(settings, "http-port");
  self->service = climber_service_new(socks5_port, http_port);
  g_signal_connect(G_OBJECT(self->service), "log",
                   G_CALLBACK(climber_service_log_handler), self);
  g_object_unref(settings);
  climber_window_statusbar_update(self->statusbar, socks5_port, http_port);

  self->preferences_dialog = NULL;
  self->new_server_dialog = NULL;
}

static void climber_window_finalize(GObject *object) {
  ClimberWindow *self = CLIMBER_WINDOW(object);
  g_object_unref(self->service);
  G_OBJECT_CLASS(climber_window_parent_class)->finalize(object);
}

static void climber_service_switch_state_changed_handler(
    ClimberServiceSwitch *widget, gboolean state, gpointer user_data) {
  ClimberWindow *window = CLIMBER_WINDOW(user_data);
  if (state) {
    climber_service_run(window->service);
  } else {
    climber_service_pause(window->service);
  }
}

static void climber_service_log_handler(ClimberService *service,
                                        const gchar *message,
                                        gpointer user_data) {
  ClimberWindow *self = CLIMBER_WINDOW(user_data);
  climber_log_view_insert_markup(self->logview, message);
}

static void climber_preferences_dialog_response_handler(
    ClimberPreferencesDialog *dialog, gint response_id, gpointer user_data) {

  ClimberWindow *window = CLIMBER_WINDOW(user_data);
  if (response_id == GTK_RESPONSE_OK) {
    gint socks5_port = climber_preferences_dialog_get_socks5_port(dialog);
    gint http_port = climber_preferences_dialog_get_http_port(dialog);
    if (socks5_port < 0 || socks5_port > 65535) {
      show_message_dialog(GTK_WINDOW(dialog), GTK_MESSAGE_WARNING,
                          "Invalid SOCKS5 port: %d", socks5_port);
    } else if (http_port < 0 || http_port > 65535) {
      show_message_dialog(GTK_WINDOW(dialog), GTK_MESSAGE_WARNING,
                          "Invalid HTTP port: %d", http_port);
    } else if (socks5_port == http_port && socks5_port != 0) {
      show_message_dialog(GTK_WINDOW(dialog), GTK_MESSAGE_WARNING,
                          "SOCKS5 port and HTTP port cannot be the same");
      return;
    }
    climber_window_save_preferences(socks5_port, http_port);
    if (climber_service_switch_get_state(window->service_switch)) {
      /* When the service is running, restart it */
      climber_service_restart(window->service, socks5_port, http_port);
    } else {
      climber_service_set_socks5_port(window->service, socks5_port);
      climber_service_set_http_port(window->service, http_port);
    }

    climber_window_statusbar_update(window->statusbar, socks5_port, http_port);
  }
  gtk_window_destroy(GTK_WINDOW(dialog));
  window->preferences_dialog = NULL;
}

static void
climber_new_server_dialog_confirm_handler(ClimberNewServerDialog *dialog,
                                          MtopServerConfig *config,
                                          gpointer user_data) {
  GUri *uri = NULL;
  ClimberWindow *window = CLIMBER_WINDOW(user_data);
  gchar *uristring = NULL;
  gtk_window_destroy(GTK_WINDOW(dialog));
  window->new_server_dialog = NULL;
  if (config == NULL) {
    return;
  }

  uri = mtop_server_config_get_uri(config);
  uristring = g_uri_to_string(uri);
  g_print("confirm: %s\n", uristring);
  g_object_unref(config);
  g_uri_unref(uri);
  g_free(uristring);
}

