/* address.c
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

gchar *g_socket_connection_get_remote_address_string(GSocketConnection *conn){
  GInetAddress *inet_address=NULL;
  gchar *address_string=NULL;
  guint port;
  gchar *return_string=NULL;
  GInetSocketAddress *socket_address=G_INET_SOCKET_ADDRESS(g_socket_connection_get_remote_address(conn,NULL));
  if(socket_address==NULL){
    return g_strdup("");
  }
  inet_address=g_inet_socket_address_get_address(socket_address);
  address_string=g_inet_address_to_string(inet_address);
  port=g_inet_socket_address_get_port(socket_address);
  return_string=g_strdup_printf("%s:%d",address_string,port);
  g_free(address_string);
  g_object_unref(socket_address);
  return return_string;
}
