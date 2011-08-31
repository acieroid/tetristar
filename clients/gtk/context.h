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

  void (* context) (Context *chat);
} ContextClass;

GType context_get_type(void);
GtkWidget *context_new(void);

void context_add_player(Context *context, TetrisPlayer *player);
void context_remove_player(Context *context, TetrisPlayer *player);

G_END_DECLS

#endif /* CONTEXT_H */

