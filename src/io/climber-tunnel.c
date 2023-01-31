/* climber-tunnel.c
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
#include "climber-tunnel.h"

struct _ClimberTunnel {
  GObject parent_instance;

  GSocketConnection *client_conn;
  GSocketConnection *remote_conn;
};

G_DEFINE_FINAL_TYPE(ClimberTunnel, climber_tunnel, G_TYPE_OBJECT)

static void climber_tunnel_finalize(GObject *object) {
  ClimberTunnel *self = CLIMBER_TUNNEL(object);
  if (self->client_conn) {
    g_object_unref(self->client_conn);
  }
  if (self->remote_conn) {
    g_object_unref(self->remote_conn);
  }
  G_OBJECT_CLASS(climber_tunnel_parent_class)->finalize(object);
}

static void climber_tunnel_class_init(ClimberTunnelClass *klass) {
  G_OBJECT_CLASS(klass)->finalize = climber_tunnel_finalize;
}

static void climber_tunnel_init(ClimberTunnel *self) {}

ClimberTunnel *climber_tunnel_new(GSocketConnection *client_conn,
                                  GSocketConnection *remote_conn) {
  ClimberTunnel *self = g_object_new(CLIMBER_TYPE_TUNNEL, NULL);
  self->client_conn = client_conn;
  g_object_ref(self->client_conn);
  self->remote_conn = remote_conn;
  g_object_ref(self->remote_conn);
  return self;
}

GSocketConnection *climber_tunnel_get_client_conn(ClimberTunnel *tunnel) {
  return tunnel->client_conn;
}
GSocketConnection *climber_tunnel_get_remote_conn(ClimberTunnel *tunnel) {
  return tunnel->remote_conn;
}

void climber_tunnel_close(ClimberTunnel *tunnel) {
  g_io_stream_close(G_IO_STREAM(tunnel->client_conn), NULL, NULL);
  g_io_stream_close(G_IO_STREAM(tunnel->remote_conn), NULL, NULL);
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

void climber_tunnel_run(ClimberTunnel *tunnel) {
  g_input_stream_read_bytes_async(
      g_io_stream_get_input_stream(G_IO_STREAM(tunnel->client_conn)), 4096,
      G_PRIORITY_DEFAULT, NULL, read_from_client_async_callback, tunnel);
  g_input_stream_read_bytes_async(
      g_io_stream_get_input_stream(G_IO_STREAM(tunnel->remote_conn)), 4096,
      G_PRIORITY_DEFAULT, NULL, read_from_remote_async_callback, tunnel);
}

