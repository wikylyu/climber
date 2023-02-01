/* response.h
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

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HTTP_TYPE_RESPONSE (http_response_get_type())

G_DECLARE_FINAL_TYPE(HttpResponse, http_response, HTTP, RESPONSE, GObject)

HttpResponse *http_response_read_from_input_stream(GInputStream *input_stream);

GBytes *http_response_build_bytes(HttpResponse *response, gsize *body_size);

guint64 http_response_get_content_length(HttpResponse *response);

G_END_DECLS

