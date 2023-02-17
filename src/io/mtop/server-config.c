/* server-config.c
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
#include "server-config.h"

struct _MtopServerConfig {
  GObject parent_instance;
  gchar *type;
  gchar *username;
  gchar *password;
  gchar *host;
  gushort port;
  gchar *proto;
  gchar *ca_path;
};

G_DEFINE_FINAL_TYPE(MtopServerConfig, mtop_server_config, G_TYPE_OBJECT)

static void mtop_server_config_finalize(GObject *object) {
  MtopServerConfig *self = MTOP_SERVER_CONFIG(object);
  g_free(self->username);
  g_free(self->password);
  g_free(self->host);
  g_free(self->ca_path);
  g_free(self->type);
  g_free(self->proto);
  G_OBJECT_CLASS(mtop_server_config_parent_class)->finalize(object);
}

static void mtop_server_config_class_init(MtopServerConfigClass *klass) {
  G_OBJECT_CLASS(klass)->finalize = mtop_server_config_finalize;
}

static void mtop_server_config_init(MtopServerConfig *self) {}

MtopServerConfig *mtop_server_config_new(const gchar *host, gushort port,
                                         const gchar *username,
                                         const gchar *password,
                                         const gchar *type, const gchar *proto,
                                         const gchar *ca_path) {
  MtopServerConfig *self =
      MTOP_SERVER_CONFIG(g_object_new(MTOP_TYPE_SERVER_CONFIG, NULL));
  self->host = g_strdup(host);
  self->port = port;
  self->username = g_strdup(username);
  self->password = g_strdup(password);
  self->ca_path = g_strdup(ca_path);
  self->type = g_strdup(type);
  self->proto = g_strdup(proto);
  return self;
}

MtopServerConfig *mtop_server_config_new_from_json_node(JsonNode *node) {
  JsonObject *obj = NULL;
  const gchar *host = NULL;
  gushort port;
  const gchar *username = NULL;
  const gchar *password = NULL;
  const gchar *ca_path = NULL;
  const gchar *type = NULL;
  const gchar *proto = NULL;
  if (!JSON_NODE_HOLDS_OBJECT(node)) {
    return NULL;
  }
  obj = json_node_get_object(node);
  host = json_object_get_string_member(obj, "host");
  port = (gushort)json_object_get_int_member(obj, "port");
  username = json_object_get_string_member(obj, "username");
  password = json_object_get_string_member(obj, "password");
  ca_path = json_object_get_string_member(obj, "ca_path");
  type = json_object_get_string_member(obj, "type");
  proto = json_object_get_string_member(obj, "proto");
  if (host == NULL || port == 0 || username == NULL || password == NULL) {
    return NULL;
  }
  return mtop_server_config_new(host, port, username, password, type, proto,
                                ca_path);
}

JsonNode *mtop_server_config_get_json_node(MtopServerConfig *self) {
  JsonNode *node = json_node_new(JSON_TYPE_OBJECT);
  JsonObject *obj = json_node_get_object(node);

  json_object_set_string_member(obj, "host", self->host);
  json_object_set_int_member(obj, "port", self->port);
  json_object_set_string_member(obj, "username", self->username);
  json_object_set_string_member(obj, "password", self->password);
  json_object_set_string_member(obj, "type", self->type);
  json_object_set_string_member(obj, "proto", self->proto);
  if (self->ca_path != NULL) {
    json_object_set_string_member(obj, "ca_path", self->ca_path);
  }

  return node;
}

const gchar *mtop_server_config_get_host(MtopServerConfig *config) {
  return config->host;
}
gushort mtop_server_config_get_port(MtopServerConfig *config) {
  return config->port;
}
const gchar *mtop_server_config_get_username(MtopServerConfig *config) {
  return config->username;
}
const gchar *mtop_server_config_get_password(MtopServerConfig *config) {
  return config->password;
}
const gchar *mtop_server_config_get_ca_path(MtopServerConfig *config) {
  return config->ca_path;
}

const gchar *mtop_server_config_get_mtype(MtopServerConfig *config) {
  return config->type;
}

const gchar *mtop_server_config_get_proto(MtopServerConfig *config) {
  return config->proto;
}

GUri *mtop_server_config_get_uri(MtopServerConfig *config) {
  gchar *userinfo =
      g_strdup_printf("%s:%s", config->username, config->password);
  gchar *path = g_strdup_printf("/%s", config->type);
  gchar *query = g_strdup_printf("proto=%s", config->proto);
  GUri *uri = g_uri_build(G_URI_FLAGS_NONE, "mtop", userinfo, config->host,
                          config->port, path, query, NULL);
  g_free(userinfo);
  g_free(path);
  g_free(query);
  return uri;
}

