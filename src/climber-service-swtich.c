/* climber-service-switch.c
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
#include "climber-service-switch.h"
#include "config.h"
#include "style.h"

struct _ClimberServiceSwitch {
  GtkBox parent_instance;

  GtkLabel *label;
  GtkSwitch *swtch;
};

G_DEFINE_FINAL_TYPE(ClimberServiceSwitch, climber_service_switch, GTK_TYPE_BOX)

static void
climber_service_switch_class_init(ClimberServiceSwitchClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class, CLIMBER_APPLICATION_PATH "/gtk/climber-service-switch.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberServiceSwitch,
                                       label);
  gtk_widget_class_bind_template_child(widget_class, ClimberServiceSwitch,
                                       swtch);
}

static void climber_service_switch_update_state(ClimberServiceSwitch *self,
                                                gboolean state) {
  if (state) {
    gtk_widget_set_name(GTK_WIDGET(self->label), "running");
    gtk_label_set_label(self->label, "Running");
  } else {
    gtk_widget_set_name(GTK_WIDGET(self->label), "paused");
    gtk_label_set_label(self->label, "Paused");
  }
}

static void climber_service_switch_state_set_handler(GtkSwitch *widget,
                                                     gboolean state,
                                                     gpointer user_data) {
  ClimberServiceSwitch *self = CLIMBER_SERVICE_SWITCH(user_data);

  climber_service_switch_update_state(self, state);
  g_print("%b\n", state);
}

static void climber_service_switch_init(ClimberServiceSwitch *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-service-switch.css");

  g_signal_connect(G_OBJECT(self->swtch), "state-set",
                   G_CALLBACK(climber_service_switch_state_set_handler), self);
}

ClimberServiceSwitch *climber_service_switch_new(void) {
  ClimberServiceSwitch *service_switch =
      CLIMBER_SERVICE_SWITCH(g_object_new(CLIMBER_TYPE_SERVICE_SWITCH, NULL));
  return service_switch;
}

void climber_service_switch_set_state(ClimberServiceSwitch *self,
                                      gboolean state) {
  climber_service_switch_update_state(self, state);
  gtk_switch_set_state(self->swtch, state);
}

