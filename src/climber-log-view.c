/* climber-log-view.c
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

#include "climber-log-view.h"
#include "style.h"

struct _ClimberLogView {
  GtkBox parent_instance;
  GtkTextView *textview;
};

G_DEFINE_FINAL_TYPE(ClimberLogView, climber_log_view, GTK_TYPE_BOX)

static void climber_log_view_class_init(ClimberLogViewClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class, CLIMBER_APPLICATION_PATH "/gtk/climber-log-view.ui");
  gtk_widget_class_bind_template_child(widget_class, ClimberLogView, textview);
}

static void climber_log_view_init(ClimberLogView *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  gtk_widget_apply_css_all(GTK_WIDGET(self), CLIMBER_APPLICATION_PATH
                           "/gtk/climber-log-view.css");
}

void climber_log_view_insert_markup(ClimberLogView *self,
                                    const gchar *message) {
  gchar strbuf[2048];
  GtkTextIter iter;
  GtkTextMark *mark;
  GDateTime *datetime;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(self->textview);

  gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);

  datetime = g_date_time_new_now_local();
  g_snprintf(strbuf, sizeof(strbuf) / sizeof(gchar), "[%02d:%02d:%02d] %s\n",
             g_date_time_get_hour(datetime), g_date_time_get_minute(datetime),
             g_date_time_get_second(datetime), message);
  g_date_time_unref(datetime);
  gtk_text_buffer_insert_markup(buffer, &iter, strbuf, -1);
  mark = gtk_text_buffer_create_mark(buffer, "end", &iter, FALSE);
  gtk_text_view_scroll_mark_onscreen(self->textview, mark);
}
