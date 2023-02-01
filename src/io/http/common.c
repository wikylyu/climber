/* common.c
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

gboolean http_parse_header(const gchar *header, gchar **k, gchar **v) {
  gchar **kv = g_strsplit(header, ":", 2);
  if (g_strv_length(kv) == 1) {
    *k = g_strdup(g_strstrip(kv[0]));
    *v = g_strdup("");
  } else if (g_strv_length(kv) == 2) {
    *k = g_strdup(g_strstrip(kv[0]));
    *v = g_strdup(g_strstrip(kv[1]));
  } else {
    g_strfreev(kv);
    return FALSE;
  }
  g_strfreev(kv);
  return TRUE;
}

gboolean http_request_header_key_equal(gconstpointer a, gconstpointer b) {
  if (g_ascii_strcasecmp(a, b) == 0) {
    return TRUE;
  }
  return FALSE;
}

