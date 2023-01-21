/* climber-service.h
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

#define CLIMBER_TYPE_SERVICE (climber_service_get_type())

G_DECLARE_FINAL_TYPE(ClimberService, climber_service, CLIMBER, SERVICE, GObject)

ClimberService *climber_service_new(gint socks5_port, gint http_port);

/*
 * Start service and listen for connection
 */
void climber_service_run(ClimberService *service);

/*
 * Pause service
 */
void climber_service_pause(ClimberService *service);

/*
 * Restart service only when port changed
 */
void climber_service_restart(ClimberService *service, gint socks5_port,
                             gint http_port);

/*
 * Start or restart service to make them effective.
 */
void climber_service_set_socks5_port(ClimberService *service, gint socks5_port);
void climber_service_set_http_port(ClimberService *service, gint http_port);

G_END_DECLS
