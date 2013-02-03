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

/**
 * @file context.h
 * @brief Implement the context widget
 */
#ifndef CONTEXT_H
#define CONTEXT_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <libtetris.h>
#include "drawing_area.h"

G_BEGIN_DECLS

#define CONTEXT_TYPE            (context_get_type())
#define CONTEXT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CONTEXT_TYPE, Context))
#define CONTEXT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST(klass), CONTEXT_TYPE, Context))
#define IS_CONTEXT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CONTEXT_TYPE))
#define IS_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE(klass), CONTEXT_TYPE)

typedef struct Context {
  GtkHBox widget;

  GSList *drawing_areas;
} Context;

typedef struct ContextClass {
  GtkHBoxClass parent_class;

  void (* context) (Context *context);
} ContextClass;

GType context_get_type(void);
GtkWidget *context_new(void);

void context_add_player(Context *context, TetrisPlayer *player);
void context_remove_player(Context *context, TetrisPlayer *player);
void context_remove_all_players(Context *context);

/**
 * Mark a player's field as "changed", which will have as effect that
 * the field will be refreshed on the display
 */
void context_field_changed(Context *context, TetrisPlayer *player, GSList *changes);

/**
 * Mark a player's next piece as changed.
 */
void context_next_piece_changed(Context *context, TetrisPlayer *player);

/**
 * Mark a player's bonuses as changed
 */
void context_bonuses_changed(Context *context, TetrisPlayer *player);

/**
 * Mark a player's kept piece as changed
 */
void context_kept_piece_changed(Context *context, TetrisPlayer *player);

/**
 * Set the shadow of a player's piece
 */
void context_set_shadow(Context *context, TetrisPlayer *player, GSList *shadow);

/**
 * Grab the focus
 */
void context_grab_focus(Context *context);

G_END_DECLS

#endif /* CONTEXT_H */

