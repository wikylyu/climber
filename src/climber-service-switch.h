/* climber-service-switch.h
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

#define CLIMBER_TYPE_SERVICE_SWITCH (climber_service_switch_get_type())

G_DECLARE_FINAL_TYPE(ClimberServiceSwitch, climber_service_switch, CLIMBER,
                     SERVICE_SWITCH, GtkBox)

ClimberServiceSwitch *climber_service_switch_new(void);

void climber_service_switch_set_state(ClimberServiceSwitch *service_switch,
                                      gboolean state);

G_END_DECLS

