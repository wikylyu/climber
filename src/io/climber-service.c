/* climber-service.c
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

#include "climber-service.h"

struct _ClimberService {
  GObject parent_instance;

  gint socks5_port;
  gint http_port;

  GSocketService *socks5_service;
  GSocketService *http_service;
};

G_DEFINE_FINAL_TYPE(ClimberService, climber_service, G_TYPE_OBJECT)

static void climber_service_emit_log(ClimberService *self, gchar *format, ...);

static void climber_service_finalize(GObject *object) {
  ClimberService *self = CLIMBER_SERVICE(object);
  if (self->socks5_service) {
    g_object_unref(self->socks5_service);
  }
  if (self->http_service) {
    g_object_unref(self->http_service);
  }
  G_OBJECT_CLASS(climber_service_parent_class)->finalize(object);
}

static void climber_service_class_init(ClimberServiceClass *klass) {
  G_OBJECT_CLASS(klass)->finalize = climber_service_finalize;

  g_signal_new("log", G_TYPE_FROM_CLASS(klass),
               G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
               0, /* class offset */
               NULL /* accumulator */, NULL /* accumulator data */,
               NULL /* C marshaller */, G_TYPE_NONE /* return_type */,
               1 /* n_params */, G_TYPE_STRING /* param_types */);
}

static void climber_service_init(ClimberService *self) {}

ClimberService *climber_service_new(gint socks5_port, gint http_port) {
  ClimberService *service =
      CLIMBER_SERVICE(g_object_new(CLIMBER_TYPE_SERVICE, NULL));

  service->socks5_port = socks5_port;
  service->http_port = http_port;
  service->socks5_service = NULL;
  service->http_service = NULL;
  return service;
}

static void climber_service_run_socks5(ClimberService *self) {
  GError *error = NULL;
  if (self->socks5_port <= 0) {
    return;
  }

  self->socks5_service = g_threaded_socket_service_new(-1);
  if (!g_socket_listener_add_inet_port(G_SOCKET_LISTENER(self->socks5_service),
                                       (guint16)(self->socks5_port), NULL,
                                       &error)) {
    climber_service_emit_log(self, "failed to start SOCKS5 proxy: %s",
                             error->message);
    g_error_free(error);
    g_object_unref(self->socks5_service);
    self->socks5_service = NULL;
  } else {
    g_socket_service_start(G_SOCKET_SERVICE(self->socks5_service));
    climber_service_emit_log(self, "SOCKS5 proxy started on port %d",
                             self->socks5_port);
  }
}

static void climber_service_run_http(ClimberService *self) {
  GError *error = NULL;
  if (self->http_port <= 0) {
    return;
  }

  self->http_service = g_threaded_socket_service_new(-1);
  if (!g_socket_listener_add_inet_port(G_SOCKET_LISTENER(self->http_service),
                                       (guint16)(self->http_port), NULL,
                                       &error)) {
    climber_service_emit_log(self, "failed to start HTTP proxy: %s",
                             error->message);
    g_error_free(error);
    g_object_unref(self->http_service);
    self->http_service = NULL;

  } else {
    g_socket_service_start(G_SOCKET_SERVICE(self->http_service));
    climber_service_emit_log(self, "HTTP proxy started on port %d",
                             self->http_port);
  }
}

void climber_service_run(ClimberService *self) {
  if (self->socks5_service != NULL || self->http_service != NULL) {
    g_debug("service already running");
    return;
  }
  climber_service_run_socks5(self);
  climber_service_run_http(self);
}

static void climber_service_stop_socks5(ClimberService *self) {
  if (self->socks5_service != NULL) {
    climber_service_emit_log(self, "SOCKS5 proxy stopped");
    g_socket_service_stop(G_SOCKET_SERVICE(self->socks5_service));
    g_socket_listener_close(G_SOCKET_LISTENER(self->socks5_service));
    g_object_unref(self->socks5_service);
    self->socks5_service = NULL;
  }
}

static void climber_service_stop_http(ClimberService *self) {
  if (self->http_service) {
    climber_service_emit_log(self, "HTTP proxy stopped");
    g_socket_service_stop(G_SOCKET_SERVICE(self->http_service));
    g_socket_listener_close(G_SOCKET_LISTENER(self->http_service));
    g_object_unref(self->http_service);
    self->http_service = NULL;
  }
}

void climber_service_pause(ClimberService *self) {
  climber_service_stop_socks5(self);
  climber_service_stop_http(self);
}

void climber_service_restart(ClimberService *self, gint socks5_port,
                             gint http_port) {
  if (self->socks5_port != socks5_port) {
    climber_service_stop_socks5(self);
    self->socks5_port = socks5_port;
    climber_service_run_socks5(self);
  }
  if (self->http_port != http_port) {
    climber_service_stop_http(self);
    self->http_port = http_port;
    climber_service_run_http(self);
  }
}

void climber_service_set_socks5_port(ClimberService *service,
                                     gint socks5_port) {
  service->socks5_port = socks5_port;
}
void climber_service_set_http_port(ClimberService *service, gint http_port) {
  service->http_port = http_port;
}

static void climber_service_emit_log(ClimberService *self, gchar *format, ...) {
  gchar strbuf[1024];
  va_list args;
  va_start(args, format);

  g_vsnprintf(strbuf, sizeof(strbuf) / sizeof(gchar), format, args);
  va_end(args);

  g_signal_emit_by_name(self, "log", strbuf);
}

