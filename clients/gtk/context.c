#include "context.h"

enum {
  KEY_PRESS,
  LAST_SIGNAL
};

static void context_class_init(ContextClass *klass);
static void context_init(Context *context);
static void context_realize (Context *context, gpointer data);
static gboolean context_expose (Context *context,
                                GdkEventExpose *event,
                                gpointer data);
static gboolean context_idle(gpointer data);
static gboolean context_draw(TetrisPlayer *player);

static guint context_signals[LAST_SIGNAL] = { 0 };

#define N_COLORS 8
#define DEFAULT_CELL 1
#define CELL_SIZE 20
static const char *colors_names[N_COLORS] = {
  "black", "magenta", "orange", "blue", "cyan", "green", "red", "yellow"
};
static GdkColor colors[N_COLORS] = { {} };

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
  int i;
  if (colors != NULL)
    for (i = 0; i < N_COLORS; i++)
      gdk_color_parse(colors_names[i], &colors[i]);
}

void context_init(Context *context)
{
  context->players = NULL;
}

void context_realize(Context *context, gpointer data)
{
}

gboolean context_expose(Context *context,
                        GdkEventExpose *event,
                        gpointer data)
{
  TetrisPlayer *player = (TetrisPlayer *) data;
  return context_draw(player);
}

gboolean context_idle(gpointer data)
{
  TetrisPlayer *player = (TetrisPlayer *) data;
  return context_draw(player);
}

gboolean context_draw(TetrisPlayer *player)
{
  TetrisMatrix *matrix;
  TetrisCell cell;
  GtkWidget *drawing_area;
  cairo_t *cairo;
  int x, y;

  drawing_area = g_object_get_data(G_OBJECT(player), "drawing-area");
  cairo = g_object_get_data(G_OBJECT(player), "cairo");
  if (cairo == NULL)
    cairo = gdk_cairo_create(drawing_area->window);
  matrix = tetris_player_get_matrix(player);

  /* default color */
  gdk_cairo_set_source_color(cairo, &colors[DEFAULT_CELL]);

  for (x = 0; x < tetris_matrix_get_width(matrix); x++) {
    for (y = 0; y < tetris_matrix_get_height(matrix); y++) {
      cell = tetris_matrix_get_cell(matrix, x, y);
      cairo_save(cairo);
      if (cell < N_COLORS)
        gdk_cairo_set_source_color(cairo, &colors[cell]);
      cairo_translate(cairo, x*CELL_SIZE, y*CELL_SIZE);
      cairo_rectangle(cairo, 0, 0, CELL_SIZE, CELL_SIZE);
      cairo_restore(cairo);
    }
  }
  return TRUE;
}

GtkWidget *context_new(void)
{
  return GTK_WIDGET(g_object_new(CONTEXT_TYPE, NULL));
}

void context_add_player(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;
  context->players = g_slist_prepend(context->players, (gpointer) player);

  drawing_area = gtk_drawing_area_new();
#if 0
  g_signal_connect_after(G_OBJECT(drawing_area), "realize",
                         G_CALLBACK(context_realize), player);
  g_signal_connect(G_OBJECT(drawing_area), "expose_event",
                   G_CALLBACK(context_expose), player);
  gtk_idle_add(context_idle, player);
#endif


  g_object_set_data(G_OBJECT(player), "drawing-area", drawing_area);
  g_object_set_data(G_OBJECT(player), "cairo", NULL);

  gtk_widget_show(drawing_area);
}

void context_remove_player(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;

  context->players = g_slist_remove(context->players, (gpointer) player);
  drawing_area = g_object_get_data(G_OBJECT(player), "drawing-area");
  g_free(drawing_area);
  gtk_idle_remove_by_data(player);
}
