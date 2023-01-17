/* climber-window.c
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

#include "config.h"

#include "climber-window.h"

struct _ClimberWindow {
  AdwApplicationWindow parent_instance;

  /* Template widgets */
  GtkHeaderBar *header_bar;
  GtkLabel *label;
};

G_DEFINE_FINAL_TYPE(ClimberWindow, climber_window, ADW_TYPE_APPLICATION_WINDOW)

static void climber_window_class_init(ClimberWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class, CLIMBER_APPLICATION_PATH "/gtk/climber-window.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberWindow, header_bar);
  gtk_widget_class_bind_template_child(widget_class, ClimberWindow, label);
}

static void climber_window_new_server_action(GSimpleAction *action,
                                             GVariant *parameter,
                                             gpointer user_data) {
  ClimberWindow *self = user_data;

  g_assert(CLIMBER_IS_WINDOW(self));
}

static void climber_window_new_subscription_action(GSimpleAction *action,
                                                   GVariant *parameter,
                                                   gpointer user_data) {
  ClimberWindow *self = user_data;

  g_assert(CLIMBER_IS_WINDOW(self));
}

static const GActionEntry win_actions[] = {
    {"new-server", climber_window_new_server_action},
    {"new-subscription", climber_window_new_subscription_action},
};

static void climber_window_init(ClimberWindow *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  g_action_map_add_action_entries(G_ACTION_MAP(self), win_actions,
                                  G_N_ELEMENTS(win_actions), self);
}

