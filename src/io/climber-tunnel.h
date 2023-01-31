/* climber-tunnel.h
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

#define CLIMBER_TYPE_TUNNEL (climber_tunnel_get_type())

G_DECLARE_FINAL_TYPE(ClimberTunnel, climber_tunnel, CLIMBER, TUNNEL, GObject)

ClimberTunnel *climber_tunnel_new(GSocketConnection *client_conn,
                                  GSocketConnection *remote_conn);

GSocketConnection *climber_tunnel_get_client_conn(ClimberTunnel *tunnel);
GSocketConnection *climber_tunnel_get_remote_conn(ClimberTunnel *tunnel);

void climber_tunnel_close(ClimberTunnel *tunnel);

void climber_tunnel_run(ClimberTunnel *tunnel);

G_END_DECLS

