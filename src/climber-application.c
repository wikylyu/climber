/* climber-application.c
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

#include "climber-application.h"
#include "climber-preferences-dialog.h"
#include "climber-window.h"
#include "message.h"

struct _ClimberApplication {
  AdwApplication parent_instance;
  ClimberWindow *window;
};

G_DEFINE_TYPE(ClimberApplication, climber_application, ADW_TYPE_APPLICATION)

static void climber_application_finalize(GObject *object);

ClimberApplication *climber_application_new(const char *application_id,
                                            GApplicationFlags flags) {
  g_return_val_if_fail(application_id != NULL, NULL);

  return g_object_new(CLIMBER_TYPE_APPLICATION, "application-id",
                      application_id, "flags", flags, NULL);
}

static void climber_application_activate(GApplication *app) {
  ClimberApplication *self;

  g_assert(CLIMBER_IS_APPLICATION(app));

  self = CLIMBER_APPLICATION(app);

  if (self->window == NULL)
    self->window = g_object_new(CLIMBER_TYPE_WINDOW, "application", app, NULL);

  gtk_window_present(GTK_WINDOW(self->window));
}

static void climber_application_class_init(ClimberApplicationClass *klass) {
  GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

  app_class->activate = climber_application_activate;
  G_OBJECT_CLASS(app_class)->finalize = climber_application_finalize;
}

static void climber_application_finalize(GObject *object) {
  G_OBJECT_CLASS(climber_application_parent_class)->finalize(object);
}

static void climber_application_about_action(GSimpleAction *action,
                                             GVariant *parameter,
                                             gpointer user_data) {
  static const char *developers[] = {"Wiky Lyu", NULL};
  ClimberApplication *self = user_data;
  GtkWindow *window = NULL;

  g_assert(CLIMBER_IS_APPLICATION(self));

  window = gtk_application_get_active_window(GTK_APPLICATION(self));

  adw_show_about_window(
      window, "application-name", "Climber", "application-icon",
      "xyz.wikylyu.climber", "developer-name", "Wiky Lyu", "version", "0.1.0",
      "developers", developers, "copyright", "© 2023 Wiky Lyu", "license-type",
      GTK_LICENSE_GPL_3_0, "website", "https://github.com/GClimber/climber",
      "issue-url", "https://github.com/GClimber/climber/issues", NULL);
}

/*
 *  Quit application
 */
static void climber_application_quit_action(GSimpleAction *action,
                                            GVariant *parameter,
                                            gpointer user_data) {
  ClimberApplication *self = user_data;

  g_assert(CLIMBER_IS_APPLICATION(self));

  g_application_quit(G_APPLICATION(self));
}

static const GActionEntry app_actions[] = {
    {"quit", climber_application_quit_action},
    {"about", climber_application_about_action},
};

static void climber_application_init(ClimberApplication *self) {
  g_action_map_add_action_entries(G_ACTION_MAP(self), app_actions,
                                  G_N_ELEMENTS(app_actions), self);
  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "app.quit",
                                        (const char *[]){"<primary>q", NULL});
  gtk_application_set_accels_for_action(GTK_APPLICATION(self),
                                        "win.preferences",
                                        (const char *[]){"<primary>p", NULL});
  gtk_application_set_accels_for_action(GTK_APPLICATION(self), "win.new-server",
                                        (const char *[]){"<primary>n", NULL});
  gtk_application_set_accels_for_action(
      GTK_APPLICATION(self), "win.new-subscription",
      (const char *[]){"<primary><shift>n", NULL});
}

