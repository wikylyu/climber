/* request.h
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

#define HTTP_TYPE_REQUEST (http_request_get_type())

G_DECLARE_FINAL_TYPE(HttpRequest, http_request, HTTP, REQUEST, GObject)

HttpRequest *http_request_new_from_input_stream(GInputStream *input_stream);

GUri *http_request_get_uri(HttpRequest *request);

GBytes *http_request_build_bytes(HttpRequest *request);

gchar *http_request_get_host_and_port(HttpRequest *request);
GNetworkAddress *http_request_get_host_address(HttpRequest *request);
const gchar *http_request_get_method(HttpRequest *request);

G_END_DECLS

