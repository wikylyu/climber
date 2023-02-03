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

const guchar MTOP_STATUS_SUCCESS=0x00;
const guchar MTOP_STATUS_INVALID_VERSION=0x01;
const guchar MTOP_STATUS_AUTH_FAILURE=0x02;
const guchar MTOP_STATUS_CONNECTION_FAILURE=0x03;

#define MTOP_TYPE_RESPONSE (mtop_response_get_type())

G_DECLARE_FINAL_TYPE(MtopResponse, mtop_response, MTOP, RESPONSE, GObject)

MtopResponse *mtop_response_new(guchar version,guchar status);

MtopResponse *mtop_response_new_from_input_stream(GInputStream *input_stream);

G_END_DECLS

