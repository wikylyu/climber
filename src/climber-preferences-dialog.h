/* climber-preferences-dialog.h
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

#define CLIMBER_TYPE_PREFERENCES_DIALOG (climber_preferences_dialog_get_type())

G_DECLARE_FINAL_TYPE(ClimberPreferencesDialog, climber_preferences_dialog,
                     CLIMBER, PREFERENCES_DIALOG, GtkDialog)

ClimberPreferencesDialog *climber_preferences_dialog_new(GtkApplication *app);

/* Open Preferences Dialog and only one dialog instance will be opened */
void show_climber_preferences_dialog(GtkApplication *app);

G_END_DECLS

