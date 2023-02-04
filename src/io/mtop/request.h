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

#define MTOP_VERION_1 0x01
#define MTOP_METHOD_CONNECT 0x01

#define MTOP_ADDRESS_TYPE_IPV4 0x01
#define MTOP_ADDRESS_TYPE_IPV6 0x04
#define MTOP_ADDRESS_TYPE_DOMAIN 0x03

G_BEGIN_DECLS

#define MTOP_TYPE_REQUEST (mtop_request_get_type())

G_DECLARE_FINAL_TYPE(MtopRequest, mtop_request, MTOP, REQUEST, GObject)

MtopRequest *mtop_request_new(guchar version, guchar method,
                              const gchar *username, const gchar *password,
                              const gchar *addr, gushort port);
GBytes *mtop_request_build_bytes(MtopRequest *request);

G_END_DECLS
