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
 * @file chat.h
 * @brief Implement the irc-like chat widget
 */
#ifndef CHAT_H
#define CHAT_H

#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

#define CHAT_TYPE            (chat_get_type())
#define CHAT(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), CHAT_TYPE, Chat))
#define CHAT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST(klass), CHAT_TYPE, Chat))
#define IS_CHAT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), CHAT_TYPE))
#define IS_CHAT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE(klass), CHAT_TYPE)

typedef struct Chat {
  /* Widget */
  GtkTable table;
  GtkWidget *scroll;
  GtkWidget *text_view;
  GtkWidget *entry;

  /* History management */
  GList *history, *history_current;
  gboolean in_history;
  
} Chat;

typedef struct ChatClass {
  GtkTableClass parent_class;

  void (* chat) (Chat *chat);
} ChatClass;

GType chat_get_type(void);
GtkWidget *chat_new(void);

/**
 * Set the focus to the chat's entry. To be called when the chat is
 * added to a container
 */
void chat_set_focus(Chat *chat);

/**
 * Add some text to the chat and apply it a certain tag (good, bad, info, error)
 */
void chat_add_colored_text(Chat *chat, const gchar *tagname, const gchar *format, ...);

/**
 * Clear all the data from the chat
 */
void chat_clear(Chat *chat);

G_END_DECLS

#endif /* CHAT_H */
