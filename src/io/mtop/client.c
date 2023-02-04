/* client.c
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
#include "client.h"
#include "request.h"
#include "response.h"

/*
 * Connect to mtop server
 * ca optional, if set, it's the path for custom ca file
 */
GIOStream *mtop_connect(const gchar *username, const gchar *password,
                        const gchar *server_host, gushort server_port,
                        const gchar *ca, const gchar *target_host,
                        gushort target_port) {
  GTlsClientConnection *tls_client = NULL;
  GSocketClient *socket_client = NULL;
  GSocketConnection *socket_conn = NULL;
  MtopRequest *request = NULL;
  MtopResponse *response = NULL;
  GBytes *data = NULL;
  gchar *host_and_port = g_strdup_printf("%s:%u", server_host, server_port);
  GSocketConnectable *address =
      g_network_address_parse(host_and_port, 443, NULL);
  g_free(host_and_port);
  if (address == NULL) {
    return NULL;
  }
  socket_client = g_socket_client_new();
  socket_conn = g_socket_client_connect(socket_client, address, NULL, NULL);
  if (socket_conn == NULL) {
    g_object_unref(socket_client);
    return NULL;
  }
  tls_client = G_TLS_CLIENT_CONNECTION(
      g_tls_client_connection_new(G_IO_STREAM(socket_conn), address, NULL));
  g_object_unref(socket_client);
  g_object_unref(socket_conn);
  if (tls_client == NULL) {
    return NULL;
  }
  if (ca != NULL) {
    GTlsDatabase *db = g_tls_file_database_new(ca, NULL);
    if (db == NULL) {
      g_object_unref(tls_client);
      return NULL;
    }
    g_tls_connection_set_database(G_TLS_CONNECTION(tls_client), db);
  }
  /* if(!g_tls_connection_handshake(G_TLS_CONNECTION(tls_client),NULL,NULL)){ */
  /*     g_object_unref(socket_client); */
  /*     g_object_unref(socket_conn); */
  /*     g_object_unref(tls_client); */
  /*     return NULL; */
  /* } */
  request = mtop_request_new(MTOP_VERION_1, MTOP_METHOD_CONNECT, username,
                             password, target_host, target_port);
  data = mtop_request_build_bytes(request);

  if (g_output_stream_write_bytes(
          g_io_stream_get_output_stream(G_IO_STREAM(tls_client)), data, NULL,
          NULL) <= 0) {
    g_object_unref(tls_client);
    g_object_unref(request);
    return NULL;
  }

  response = mtop_response_new_from_input_stream(
      g_io_stream_get_input_stream(G_IO_STREAM(tls_client)));
  if (response == NULL) {
    g_object_unref(tls_client);
    g_object_unref(request);
    return NULL;
  }

  g_object_unref(request);
  g_object_unref(response);
  return G_IO_STREAM(tls_client);
}
