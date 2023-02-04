/* request.c
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
#include "request.h"

struct _MtopRequest {
  GObject parent_instance;
  guchar version;
  guchar method;
  gchar *username;
  gchar *password;
  GInetAddress *inet_address;
  gchar *domain;
  gushort port;
};

G_DEFINE_FINAL_TYPE(MtopRequest, mtop_request, G_TYPE_OBJECT)

static void mtop_request_finalize(GObject *object) {
  MtopRequest *self = MTOP_REQUEST(object);
  g_free(self->username);
  g_free(self->password);
  g_free(self->domain);
  if (self->inet_address) {
    g_object_unref(self->inet_address);
  }
  G_OBJECT_CLASS(mtop_request_parent_class)->finalize(object);
}

static void mtop_request_class_init(MtopRequestClass *klass) {
  G_OBJECT_CLASS(klass)->finalize = mtop_request_finalize;
}

static void mtop_request_init(MtopRequest *self) {}

MtopRequest *mtop_request_new(guchar version, guchar method,
                              const gchar *username, const gchar *password,
                              const gchar *addr, gushort port) {
  GInetAddress *address = NULL;
  MtopRequest *request = MTOP_REQUEST(g_object_new(MTOP_TYPE_REQUEST, NULL));
  request->version = version;
  request->method = method;
  request->username = g_strdup(username);
  request->password = g_strdup(password);
  request->domain = NULL;
  request->inet_address = NULL;
  address = g_inet_address_new_from_string(addr);
  if (address == NULL) {
    request->domain = g_strdup(addr);
  } else {
    request->inet_address = address;
  }
  request->port = port;
  return request;
}

GBytes *mtop_request_build_bytes(MtopRequest *request) {
  guint8 size;
  gushort port;
  GByteArray *array = g_byte_array_new();
  g_byte_array_append(array, &(request->version), 1);
  g_byte_array_append(array, &(request->method), 1);
  size = strlen(request->username);
  g_byte_array_append(array, &size, 1);
  g_byte_array_append(array, (guint8 *)request->username, size);
  size = strlen(request->password);
  g_byte_array_append(array, &size, 1);
  g_byte_array_append(array, (guint8 *)request->password, size);
  if (request->inet_address != NULL) {
    guint8 type = 0;
    if (g_inet_address_get_family(request->inet_address) ==
        G_SOCKET_FAMILY_IPV4) {
      type = MTOP_ADDRESS_TYPE_IPV4;
    } else {
      type = MTOP_ADDRESS_TYPE_IPV6;
    }
    g_byte_array_append(array, &type, 1);
    g_byte_array_append(array, g_inet_address_to_bytes(request->inet_address),
                        g_inet_address_get_native_size(request->inet_address));
  } else {
    guint8 type = MTOP_ADDRESS_TYPE_DOMAIN;
    g_byte_array_append(array, &type, 1);
    g_byte_array_append(array, (const guint8 *)request->domain,
                        strlen(request->domain));
  }
  port = GUINT16_TO_BE(request->port);
  g_byte_array_append(array, (const guint8 *)&port, 2);
  return g_byte_array_free_to_bytes(array);
}
