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
#include "response.h"

struct _MtopResponse {
  GObject parent_instance;
  guchar version;
  guchar status;
};

G_DEFINE_FINAL_TYPE(MtopResponse, mtop_response, G_TYPE_OBJECT)

static void mtop_response_finalize(GObject *object) {
  /* MtopResponse *self = MTOP_RESPONSE(object); */
  G_OBJECT_CLASS(mtop_response_parent_class)->finalize(object);
}

static void mtop_response_class_init(MtopResponseClass *klass) {
  G_OBJECT_CLASS(klass)->finalize = mtop_response_finalize;
}

static void mtop_response_init(MtopResponse *self) {}

MtopResponse *mtop_response_new(guchar version, guchar status) {
  MtopResponse *response =
      MTOP_RESPONSE(g_object_new(MTOP_TYPE_RESPONSE, NULL));
  response->version = version;
  response->status = status;
  return response;
}

MtopResponse *mtop_response_new_from_input_stream(GInputStream *input_stream) {
  gchar buf[2];
  gsize bytes_read = 0;
  if (!g_input_stream_read_all(input_stream, buf, sizeof(buf) / sizeof(gchar),
                               &bytes_read, NULL, NULL)) {
    return NULL;
  } else if (bytes_read != 2) {
    return NULL;
  }
  return mtop_response_new(buf[0], buf[1]);
}
