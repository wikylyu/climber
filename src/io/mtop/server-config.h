/* server-config.h
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
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define MTOP_TYPE_SERVER_CONFIG (mtop_server_config_get_type())

G_DECLARE_FINAL_TYPE(MtopServerConfig, mtop_server_config, MTOP, SERVER_CONFIG,
                     GObject)

MtopServerConfig *mtop_server_config_new(const gchar *host, gushort port,
                                         const gchar *username,
                                         const gchar *password,
                                         const gchar *ca_path);

/*
 * Create a MTopServerConfig from JSON node
 * return NULL if fail
 */
MtopServerConfig *mtop_server_config_new_from_json_node(JsonNode *node);

/*
 * The caller is responsible to free JsonNode
 */
JsonNode *mtop_server_config_get_json_node(MtopServerConfig *config);


const gchar *mtop_server_config_get_host(MtopServerConfig *config);
gushort mtop_server_config_get_port(MtopServerConfig *config);
const gchar *mtop_server_config_get_username(MtopServerConfig *config);
const gchar *mtop_server_config_get_password(MtopServerConfig *config);
const gchar *mtop_server_config_get_ca_path(MtopServerConfig *config);

G_END_DECLS

