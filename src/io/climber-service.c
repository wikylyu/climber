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

#include "address.h"
#include "climber-service.h"
#include "climber-tunnel.h"
#include "http/request.h"

struct _ClimberService {
  GObject parent_instance;

  gint socks5_port;
  gint http_port;

  GSocketService *socks5_service;
  GSocketService *http_service;
};

G_DEFINE_FINAL_TYPE(ClimberService, climber_service, G_TYPE_OBJECT)

/* Emit log signal */
static void climber_service_emit_log(ClimberService *self, const gchar *format,
                                     ...);
static gboolean climber_service_http_incoming_handler(GSocketService *service,
                                                      GSocketConnection *conn,
                                                      GObject *source_object,
                                                      gpointer user_data);
static gboolean climber_service_socks5_incoming_handler(GSocketService *service,
                                                        GSocketConnection *conn,
                                                        GObject *source_object,
                                                        gpointer user_data);

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
    climber_service_emit_log(
        self, "<span color=\"red\">failed to start SOCKS5 proxy: %s</span>",
        error->message);
    g_error_free(error);
    g_object_unref(self->socks5_service);
    self->socks5_service = NULL;
  } else {
    g_socket_service_start(G_SOCKET_SERVICE(self->socks5_service));
    g_signal_connect(G_OBJECT(self->socks5_service), "incoming",
                     G_CALLBACK(climber_service_socks5_incoming_handler), self);
    climber_service_emit_log(
        self, "SOCKS5 proxy started on port <span color=\"green\">%d</span>",
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
    climber_service_emit_log(
        self, "<span color=\"red\">failed to start HTTP proxy: %s</span>",
        error->message);
    g_error_free(error);
    g_object_unref(self->http_service);
    self->http_service = NULL;

  } else {
    g_socket_service_start(G_SOCKET_SERVICE(self->http_service));
    g_signal_connect(G_OBJECT(self->http_service), "incoming",
                     G_CALLBACK(climber_service_http_incoming_handler), self);
    climber_service_emit_log(
        self, "HTTP proxy started on port <span color=\"green\">%d</span>",
        self->http_port);
  }
}

void climber_service_run(ClimberService *self) {
  if (self->socks5_service != NULL || self->http_service != NULL) {
    g_print("service already running");
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

static void climber_service_emit_log(ClimberService *self, const gchar *format,
                                     ...) {
  gchar strbuf[1024];
  va_list args;
  va_start(args, format);

  g_vsnprintf(strbuf, sizeof(strbuf) / sizeof(gchar), format, args);
  va_end(args);

  g_signal_emit_by_name(self, "log", strbuf);
}

static GNetworkAddress *read_http_connect_request(GInputStream *input_stream) {
  GNetworkAddress *address = NULL;
  HttpRequest *request = http_request_read_from_input_stream(input_stream);
  if (request == NULL) {

    return address;
  }
  GUri *uri = http_request_get_uri(request);
  const gchar *host = g_uri_get_host(uri);
  gint port = g_uri_get_port(uri);
  if (port <= 0) {
    if (g_strcmp0("http", g_uri_get_scheme(uri)) == 0) {
      port = 80;
    } else if (g_strcmp0("https", g_uri_get_scheme(uri)) == 0) {
      port = 443;
    }
  }
  gchar *host_and_port = g_strdup_printf("%s:%d", host, port);
  address = G_NETWORK_ADDRESS(g_network_address_parse(host_and_port, 80, NULL));

  http_request_free(request);
  g_free(host_and_port);

  return address;
}

static void read_from_client_async_callback(GObject *source_object,
                                            GAsyncResult *res,
                                            gpointer user_data);
static void read_from_remote_async_callback(GObject *source_object,
                                            GAsyncResult *res,
                                            gpointer user_data);

static void read_from_client_async_callback(GObject *source_object,
                                            GAsyncResult *res,
                                            gpointer user_data) {
  ClimberTunnel *tunnel = CLIMBER_TUNNEL(user_data);
  GSocketConnection *client_conn = climber_tunnel_get_client_conn(tunnel);
  GSocketConnection *remote_conn = climber_tunnel_get_remote_conn(tunnel);
  GBytes *bytes = g_input_stream_read_bytes_finish(
      G_INPUT_STREAM(source_object), res, NULL);
  if (bytes == NULL || g_bytes_get_size(bytes) == 0) {
    climber_tunnel_close(tunnel);
    g_object_unref(tunnel);
    return;
  }
  if (g_output_stream_write_bytes(
          g_io_stream_get_output_stream(G_IO_STREAM(remote_conn)), bytes, NULL,
          NULL) < 0) {
    climber_tunnel_close(tunnel);
    g_object_unref(tunnel);
    return;
  }

  g_bytes_unref(bytes);
  g_input_stream_read_bytes_async(
      g_io_stream_get_input_stream(G_IO_STREAM(client_conn)), 4096,
      G_PRIORITY_DEFAULT, NULL, read_from_client_async_callback, tunnel);
}

static void read_from_remote_async_callback(GObject *source_object,
                                            GAsyncResult *res,
                                            gpointer user_data) {
  ClimberTunnel *tunnel = CLIMBER_TUNNEL(user_data);
  GSocketConnection *client_conn = climber_tunnel_get_client_conn(tunnel);
  GSocketConnection *remote_conn = climber_tunnel_get_remote_conn(tunnel);
  GBytes *bytes = g_input_stream_read_bytes_finish(
      G_INPUT_STREAM(source_object), res, NULL);
  if (bytes == NULL || g_bytes_get_size(bytes) == 0) {
    climber_tunnel_close(tunnel);
    g_object_unref(tunnel);
    return;
  }
  if (g_output_stream_write_bytes(
          g_io_stream_get_output_stream(G_IO_STREAM(client_conn)), bytes, NULL,
          NULL) < 0) {
    climber_tunnel_close(tunnel);
    g_object_unref(tunnel);
    return;
  }
  g_bytes_unref(bytes);
  g_input_stream_read_bytes_async(
      g_io_stream_get_input_stream(G_IO_STREAM(remote_conn)), 4096,
      G_PRIORITY_DEFAULT, NULL, read_from_remote_async_callback, tunnel);
}

static gboolean climber_service_http_incoming_handler(GSocketService *service,
                                                      GSocketConnection *conn,
                                                      GObject *source_object,
                                                      gpointer user_data) {
  ClimberService *self = CLIMBER_SERVICE(user_data);
  ClimberTunnel *tunnel = NULL;
  GInputStream *input_stream = g_io_stream_get_input_stream(G_IO_STREAM(conn));
  GOutputStream *output_stream =
      g_io_stream_get_output_stream(G_IO_STREAM(conn));
  GSocketClient *remote_client = NULL;
  GSocketConnection *remote_conn = NULL;
  const gchar *response = "HTTP/1.1 200 OK\r\n\r\n";
  gchar *client_address = NULL;
  gchar *remote_address = NULL;
  GNetworkAddress *address = read_http_connect_request(input_stream);
  if (address == NULL) {
    return FALSE;
  }
  remote_address =
      g_strdup_printf("%s:%d", g_network_address_get_hostname(address),
                      g_network_address_get_port(address));

  remote_client = g_socket_client_new();
  remote_conn = g_socket_client_connect(
      remote_client, G_SOCKET_CONNECTABLE(address), NULL, NULL);
  g_object_unref(address);
  g_object_unref(remote_client);
  if (remote_conn == NULL) {
    return FALSE;
  }

  if (g_output_stream_write(output_stream, response, strlen(response), NULL,
                            NULL) < 0) {
    g_object_unref(remote_conn);
    return FALSE;
  }
  tunnel = climber_tunnel_new(conn, remote_conn);
  g_object_ref(tunnel);
  g_object_unref(remote_conn);
  g_input_stream_read_bytes_async(input_stream, 4096, G_PRIORITY_DEFAULT, NULL,
                                  read_from_client_async_callback, tunnel);
  g_input_stream_read_bytes_async(
      g_io_stream_get_input_stream(G_IO_STREAM(remote_conn)), 4096,
      G_PRIORITY_DEFAULT, NULL, read_from_remote_async_callback, tunnel);

  client_address = g_socket_connection_get_remote_address_string(conn);
  climber_service_emit_log(
      self, "[HTTP] connection established: <b>%s</b> &lt;=&gt; <b>%s</b>",
      client_address, remote_address);
  g_free(client_address);
  g_free(remote_address);
  return FALSE;
}

static gboolean climber_service_socks5_incoming_handler(GSocketService *service,
                                                        GSocketConnection *conn,
                                                        GObject *source_object,
                                                        gpointer user_data) {
  ClimberService *self = CLIMBER_SERVICE(user_data);

  return FALSE;
}

