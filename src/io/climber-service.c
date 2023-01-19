/* climber-service.c
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

#include "climber-service.h"

struct _ClimberService {
  GObject parent_instance;

  gint socks5_port;
  gint http_port;
};

G_DEFINE_FINAL_TYPE(ClimberService, climber_service, G_TYPE_OBJECT)

static void climber_service_class_init(ClimberServiceClass *klass) {}

static void climber_service_init(ClimberService *self) {}

ClimberService *climber_service_new(gint socks5_port, gint http_port) {
  ClimberService *service =
      CLIMBER_SERVICE(g_object_new(CLIMBER_TYPE_SERVICE, NULL));
  service->socks5_port = socks5_port;
  service->http_port = http_port;
  return service;
}

void climber_service_run(ClimberService *service) { g_print("running\n"); }

void climber_service_pause(ClimberService *service) { g_print("paused\n"); }

