#include "context.h"

enum {
  KEY_PRESS,
  LAST_SIGNAL
};

static void context_class_init(ContextClass *klass);
static void context_init(Context *context);

static guint context_signals[LAST_SIGNAL] = { 0 };

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

    context_type = g_type_register_static(GTK_TYPE_WIDGET, "Context",
                                          &context_info, 0);
  }

  return context_type;
}

void context_class_init(ContextClass *klass)
{
  /* TODO: signals */
}

void context_init(Context *context)
{
}

GtkWidget *context_new(void)
{
  return GTK_WIDGET(g_object_new(CONTEXT_TYPE, NULL));
}

