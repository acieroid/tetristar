/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "context.h"

static void context_class_init(ContextClass *klass);
static void context_init(Context *context);
static gint context_compare(gpointer drawing_area, gpointer player);

GType context_get_type(void)
{
  static GType context_type = 0;

  if (!context_type) {
    const GTypeInfo context_info = {
      sizeof(ContextClass), NULL, NULL,
      (GClassInitFunc) context_class_init, NULL, NULL,
      sizeof(Context), 0,
      (GInstanceInitFunc) context_init,
    };

    context_type = g_type_register_static(GTK_TYPE_HBOX, "Context",
                                          &context_info, 0);
  }

  return context_type;
}

void context_class_init(ContextClass *klass)
{
}

void context_init(Context *context)
{
  gtk_widget_set_can_focus(GTK_WIDGET(context), TRUE);
  context->drawing_areas = NULL;
}

gint context_compare(gpointer drawing_area, gpointer player)
{
  TetrisPlayer *player_a, *player_b;

  if (drawing_area == NULL) {
    return 0;
  }

  player_a = drawing_area_get_player((DrawingArea *) drawing_area);
  player_b = (TetrisPlayer *) player;
  if (player_a == player_b)
    return 0;
  return 1;
}

GtkWidget *context_new(void)
{
  return GTK_WIDGET(g_object_new(CONTEXT_TYPE, NULL));
}

void context_add_player(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;

  drawing_area = drawing_area_new(player);
  context->drawing_areas = g_slist_prepend(context->drawing_areas,
                                           (gpointer) drawing_area);
  gtk_container_add(GTK_CONTAINER(context), drawing_area);
  gtk_widget_show(drawing_area);
}

void context_remove_player(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;
  GSList *elem;

  elem = g_slist_find_custom(context->drawing_areas,
                             (gpointer) player,
                             (GCompareFunc) context_compare);
  if (elem == NULL)
    g_return_if_reached();
  drawing_area = (GtkWidget *) elem->data;
  context->drawing_areas = g_slist_remove_link(context->drawing_areas,
                                               elem);
  drawing_area_free(DRAWING_AREA(drawing_area));
}

void context_remove_all_players(Context *context)
{
  g_slist_free_full(context->drawing_areas,
                    (GDestroyNotify) drawing_area_free);
  context->drawing_areas = NULL;
}

void context_field_changed(Context *context, TetrisPlayer *player, GSList *changes)
{
  GtkWidget *drawing_area;
  GSList *elem;

  elem = g_slist_find_custom(context->drawing_areas,
                             (gpointer) player,
                             (GCompareFunc) context_compare);
  if (elem == NULL)
    g_return_if_reached();

  drawing_area = elem->data;
  drawing_area_set_changed(DRAWING_AREA(drawing_area), changes);
}

void context_next_piece_changed(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;
  GSList *elem;

  elem = g_slist_find_custom(context->drawing_areas,
                             (gpointer) player,
                             (GCompareFunc) context_compare);

  if (elem == NULL)
    g_return_if_reached();

  drawing_area = elem->data;
  drawing_area_set_next_piece_changed(DRAWING_AREA(drawing_area));
}

void context_bonuses_changed(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;
  GSList *elem;

  elem = g_slist_find_custom(context->drawing_areas,
                             (gpointer) player,
                             (GCompareFunc) context_compare);

  if (elem == NULL) {
    g_return_if_reached();
  }

  drawing_area = elem->data;
  drawing_area_set_bonuses_changed(DRAWING_AREA(drawing_area));
}

void context_kept_piece_changed(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;
  GSList *elem;

  elem = g_slist_find_custom(context->drawing_areas,
                             (gpointer) player,
                             (GCompareFunc) context_compare);

  if (elem == NULL) {
    g_return_if_reached();
  }

  drawing_area = elem->data;
  drawing_area_set_kept_piece_changed(DRAWING_AREA(drawing_area));
}

void context_set_shadow(Context *context, TetrisPlayer *player, GSList *shadow)
{
  GtkWidget *drawing_area;
  GSList *elem;

  elem = g_slist_find_custom(context->drawing_areas,
                             (gpointer) player,
                             (GCompareFunc) context_compare);

  if (elem == NULL)
    g_return_if_reached();

  drawing_area = elem->data;
  drawing_area_set_shadow(DRAWING_AREA(drawing_area), shadow);
}

void context_grab_focus(Context *context)
{
  gtk_widget_grab_focus(GTK_WIDGET(context));
}
