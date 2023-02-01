/* response.c
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
#include "common.h"
#include "response.h"

struct _HttpResponse {
  GObject parent_instance;

  gchar *version;
  gchar *code;
  gchar *message;
  GHashTable *headers;
  GByteArray *body;
  guint64 content_length;
};

G_DEFINE_FINAL_TYPE(HttpResponse, http_response, G_TYPE_OBJECT)

static void http_response_finalize(GObject *object) {
  HttpResponse *self = HTTP_RESPONSE(object);
  g_free(self->version);
  g_free(self->code);
  g_free(self->message);

  g_hash_table_unref(self->headers);
  g_byte_array_free(self->body, TRUE);
  G_OBJECT_CLASS(http_response_parent_class)->finalize(object);
}

static void http_response_class_init(HttpResponseClass *klass) {
  G_OBJECT_CLASS(klass)->finalize = http_response_finalize;
}

static void http_response_init(HttpResponse *self) {}

static gboolean http_parse_statusline(const gchar *statusline, gchar **version,
                                      gchar **code, gchar **message) {
  gchar **seps = g_strsplit(statusline, " ", -1);
  if (g_strv_length(seps) != 3) {
    g_strfreev(seps);
    return FALSE;
  }

  *version = g_strdup(seps[0]);
  *code = g_strdup(seps[1]);
  *message = g_strdup(seps[2]);
  g_strfreev(seps);
  return TRUE;
}

HttpResponse *http_response_read_from_input_stream(GInputStream *input_stream) {
  HttpResponse *response = NULL;
  gchar *code = NULL;
  gchar *version = NULL;
  gchar *message = NULL;
  GHashTable *headers = NULL;
  GByteArray *body = NULL;
  guint64 content_length = 0;
  gchar *buf;
  GError *error = NULL;
  gsize n;
  GDataInputStream *data_stream = g_data_input_stream_new(input_stream);
  g_filter_input_stream_set_close_base_stream(
      G_FILTER_INPUT_STREAM(data_stream), FALSE);
  g_data_input_stream_set_newline_type(data_stream,
                                       G_DATA_STREAM_NEWLINE_TYPE_CR_LF);

  buf = g_data_input_stream_read_line(data_stream, NULL, NULL, NULL);
  if (buf == NULL) {
    goto RETURN;
  }
  if (!http_parse_statusline(g_strstrip(buf), &version, &code, &message)) {
    goto RETURN;
  }
  g_free(buf);
  headers = g_hash_table_new_full(g_str_hash, http_request_header_key_equal,
                                  g_free, g_free);
  while (TRUE) {
    gchar **kv = NULL;
    gchar *k = NULL;
    gchar *v = NULL;
    buf = g_data_input_stream_read_line(data_stream, NULL, NULL, &error);
    if (error != NULL) {
      g_free(code);
      g_free(version);
      g_free(message);
      g_hash_table_unref(headers);
      goto RETURN;
    } else if (buf == NULL) {
      break;
    } else if (strlen(buf) == 0) {
      g_free(buf);
      break;
    }
    if (!http_parse_header(g_strstrip(buf), &k, &v)) {
      g_free(buf);
      continue;
    }
    g_free(buf);
    g_hash_table_insert(headers, k, v);
    if (g_ascii_strncasecmp(k, "content-length", -1) == 0) {
      content_length = g_ascii_strtoull(v, NULL, 0);
    }
    g_strfreev(kv);
  }

  body = g_byte_array_new();
  n = g_buffered_input_stream_get_available(
      G_BUFFERED_INPUT_STREAM(data_stream));
  if (n > 0) {
    gsize bytes_read;
    guint8 *body_buffer = g_malloc(sizeof(guint8) * n);
    if (!g_input_stream_read_all(G_INPUT_STREAM(data_stream), body_buffer, n,
                                 &bytes_read, NULL, NULL) ||
        bytes_read != n) {
      g_free(body_buffer);
      g_free(code);
      g_free(version);
      g_free(message);
      g_hash_table_unref(headers);
      g_byte_array_free(body, TRUE);
      goto RETURN;
    }
    g_byte_array_append(body, body_buffer, bytes_read);
    g_free(body_buffer);
  }
  /* if (content_length > 0) { */
  /*   gsize bytes_read; */
  /*   guint8 *body_buffer = g_malloc(sizeof(guint8) * content_length); */
  /*   if (!g_input_stream_read_all(G_INPUT_STREAM(data_stream), body_buffer, */
  /*                                content_length, &bytes_read, NULL, NULL) ||
   */
  /*       bytes_read != content_length) { */
  /*     g_free(body_buffer); */
  /*     g_free(code); */
  /*     g_free(version); */
  /*     g_free(message); */
  /*     g_hash_table_unref(headers); */
  /*     g_byte_array_free(body, TRUE); */
  /*     goto RETURN; */
  /*   } */
  /*   g_byte_array_append(body, body_buffer, bytes_read); */
  /*   g_free(body_buffer); */
  /* } */
  response = HTTP_RESPONSE(g_object_new(HTTP_TYPE_RESPONSE, NULL));
  response->version = version;
  response->code = code;
  response->message = message;
  response->headers = headers;
  response->body = body;
  response->content_length = content_length;
RETURN:
  if (error != NULL) {
    g_error_free(error);
  }
  g_object_unref(data_stream);
  return response;
}

GBytes *http_response_build_bytes(HttpResponse *response, gsize *body_size) {
  GByteArray *array = NULL;
  GString *string = g_string_new(NULL);
  GHashTableIter iter;
  gpointer key, value;

  g_string_append(string, response->version);
  g_string_append(string, " ");
  g_string_append(string, response->code);
  g_string_append(string, " ");
  g_string_append(string, response->message);
  g_string_append(string, "\r\n");

  g_hash_table_iter_init(&iter, response->headers);
  while (g_hash_table_iter_next(&iter, &key, &value)) {
    g_string_append(string, (const gchar *)key);
    g_string_append(string, " : ");
    g_string_append(string, (const gchar *)value);
    g_string_append(string, "\r\n");
  }
  g_string_append(string, "\r\n");

  array = g_bytes_unref_to_array(g_string_free_to_bytes(string));
  if (response->body->len > 0) {
    g_byte_array_append(array, response->body->data, response->body->len);
    *body_size = response->body->len;
  }

  return g_byte_array_free_to_bytes(array);
}

guint64 http_response_get_content_length(HttpResponse *response) {
  return response->content_length;
}

