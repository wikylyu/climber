/* client.h
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

#include "server-config.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

/*
 * Connect to mtop server
 * ca optional, if set, it's the path for custom ca file
 */
GIOStream *mtop_connect(const gchar *username, const gchar *password,
                        const gchar *server_host, gushort server_port,
                        const gchar *ca, const gchar *target_host,
                        gushort target_port);

GIOStream *mtop_connect_with_server_config(MtopServerConfig *config,
                                           const gchar *target_host,
                                           gushort target_port);

G_END_DECLS

