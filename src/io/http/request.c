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

struct _HttpRequest {
  gchar *method;
  GUri *uri;
  gchar *version;
  GHashTable *headers;
  GByteArray *body;
};

HttpRequest *http_request_read_from_input_stream(GInputStream *input_stream) {
  HttpRequest *request = NULL;
  gchar **lines = NULL;
  gchar **seps = NULL;
  gchar *method = NULL;
  gchar *version = NULL;
  GUri *uri = NULL;
  GHashTable *headers = NULL;
  GByteArray *body = NULL;
  gint i;
  guint64 content_length = 0;

  GDataInputStream *data_stream = g_data_input_stream_new(input_stream);
  g_filter_input_stream_set_close_base_stream(
      G_FILTER_INPUT_STREAM(data_stream), FALSE);
  gsize length;
  gchar *buf = g_data_input_stream_read_upto(data_stream, "\r\n\r\n", 4,
                                             &length, NULL, NULL);
  if (buf == NULL) {
    goto RETURN;
  }
  g_data_input_stream_read_byte(data_stream, NULL, NULL);
  g_data_input_stream_read_byte(data_stream, NULL, NULL);
  g_data_input_stream_read_byte(data_stream, NULL, NULL);
  g_data_input_stream_read_byte(data_stream, NULL, NULL);

  lines = g_strsplit(buf, "\r\n", -1);
  if (g_strv_length(lines) < 1) {
    goto RETURN;
  }
  seps = g_strsplit(g_strstrip(lines[0]), " ", -1);
  if (g_strv_length(seps) != 3) {
    goto RETURN;
  }

  if (g_str_has_prefix(seps[1], "http")) {
    uri = g_uri_parse(seps[1], G_URI_FLAGS_NONE, NULL);
  } else {
    gchar *tmp = g_strdup_printf("http://%s", seps[1]);
    uri = g_uri_parse(tmp, G_URI_FLAGS_NONE, NULL);
    g_free(tmp);
  }
  if (uri == NULL) { // parse uri failed
    goto RETURN;
  }
  method = g_strdup(seps[0]);
  version = g_strdup(seps[2]);
  headers = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
  i = 1;
  for (i = 1; i < g_strv_length(lines); i++) {
    gchar **kv = NULL;
    gchar *header = g_strstrip(lines[i]);
    gchar *k = NULL;
    gchar *v = NULL;
    kv = g_strsplit(header, ":", 2);

    if (g_strv_length(kv) == 1) {
      k = g_strdup(g_strstrip(kv[0]));
      v = g_strdup("");
    } else if (g_strv_length(kv) == 2) {
      k = g_strdup(g_strstrip(kv[0]));
      v = g_strdup(g_strstrip(kv[1]));
    } else {
      g_strfreev(kv);
      continue;
    }
    g_hash_table_insert(headers, k, v);
    if (g_ascii_strncasecmp(k, "content-length", -1) == 0) {
      content_length = g_ascii_strtoull(v, NULL, 64);
    }
    g_strfreev(kv);
  }
  body = g_byte_array_new();
  if (content_length > 0) {
    gsize bytes_read;
    gchar *body_buffer = g_malloc(sizeof(gchar) * content_length);
    if (!g_input_stream_read_all(G_INPUT_STREAM(data_stream), body_buffer,
                                 content_length, &bytes_read, NULL, NULL) ||
        bytes_read != content_length) {
      g_free(body_buffer);
      g_free(method);
      g_free(version);
      g_uri_unref(uri);
      g_hash_table_unref(headers);
      g_byte_array_free(body, TRUE);
      goto RETURN;
    }
    g_byte_array_append(body, body_buffer, bytes_read);
    g_free(body_buffer);
  }
  request = (HttpRequest *)g_malloc(sizeof(HttpRequest));
  request->method = method;
  request->uri = uri;
  request->version = version;
  request->headers = headers;
  request->body = body;

RETURN:

  g_strfreev(lines);
  g_strfreev(seps);
  g_object_unref(data_stream);
  g_free(buf);
  return request;
}

GUri *http_request_get_uri(HttpRequest *request) { return request->uri; }

const gchar *http_request_get_method(HttpRequest *request) {
  return request->method;
}

void http_request_free(HttpRequest *request) {
  g_free(request->method);
  g_free(request->version);
  g_uri_unref(request->uri);
  g_hash_table_unref(request->headers);
  g_byte_array_free(request->body, TRUE);
  g_free(request);
}

GBytes *http_request_build_bytes(HttpRequest *request) {
  GByteArray *array = NULL;
  GString *string = g_string_new(NULL);
  GHashTableIter iter;
  gpointer key, value;

  g_string_append(string, request->method);
  g_string_append(string, " ");
  g_string_append(string, g_uri_get_path(request->uri));
  if (g_uri_get_query(request->uri) != NULL) {
    g_string_append(string, "?");
    g_string_append(string, g_uri_get_query(request->uri));
  }
  g_string_append(string, " ");
  g_string_append(string, request->version);
  g_string_append(string, "\r\n");

  g_hash_table_iter_init(&iter, request->headers);
  while (g_hash_table_iter_next(&iter, &key, &value)) {
    g_string_append(string, (const gchar *)key);
    g_string_append(string, " : ");
    g_string_append(string, (const gchar *)value);
    g_string_append(string, "\r\n");
  }
  g_string_append(string, "\r\n");

  array = g_bytes_unref_to_array(g_string_free_to_bytes(string));
  if (request->body->len > 0) {
    g_byte_array_append(array, request->body->data, request->body->len);
  }

  return g_byte_array_free_to_bytes(array);
}

gchar *http_request_get_host_and_port(HttpRequest *request) {
  GUri *uri = request->uri;
  const gchar *host = g_uri_get_host(uri);
  gint port = g_uri_get_port(uri);
  if (port <= 0) {
    if (g_strcmp0("http", g_uri_get_scheme(uri)) == 0) {
      port = 80;
    } else if (g_strcmp0("https", g_uri_get_scheme(uri)) == 0) {
      port = 443;
    }
  }
  return g_strdup_printf("%s:%d", host, port);
}

GNetworkAddress *http_request_get_host_address(HttpRequest *request) {
  gchar *host_and_port = http_request_get_host_and_port(request);
  GNetworkAddress *address =
      G_NETWORK_ADDRESS(g_network_address_parse(host_and_port, 80, NULL));

  g_free(host_and_port);
  return address;
}

