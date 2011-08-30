#include "context.h"

enum {
  KEY_PRESS,
  LAST_SIGNAL
};

static void context_class_init(ContextClass *klass);
static void context_init(Context *context);
static gint context_compare(gpointer drawing_area, gpointer player);
static void context_realize (Context *context, gpointer data);
static gboolean context_expose (Context *context,
                                GdkEventExpose *event,
                                gpointer data);
static gboolean context_idle(gpointer data);
static gboolean context_draw(GtkWidget *drawing_area);
static void context_cairo_draw_cell(cairo_t *cairo, int x, int y,
                                    TetrisCell cell);

static guint context_signals[LAST_SIGNAL] = { 0 };
static GStaticMutex draw_mutex = G_STATIC_MUTEX_INIT;

#define N_COLORS 8
#define DEFAULT_CELL 1
#define CELL_SIZE 10
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
  gtk_widget_set_can_focus(GTK_WIDGET(context), TRUE);
  context->drawing_areas = NULL;
}

gint context_compare(gpointer drawing_area, gpointer player)
{
  TetrisPlayer *player_a, *player_b;

  player_a = g_object_get_data(G_OBJECT(drawing_area), "player");
  player_b = (TetrisPlayer *) player;
  if (player_a == player_b)
    return 0;
  return 1;
}

void context_realize(Context *context, gpointer data)
{
}

gboolean context_expose(Context *context,
                        GdkEventExpose *event,
                        gpointer data)
{
  GtkWidget *drawing_area = (GtkWidget *) data;
  if (gtk_widget_get_realized(drawing_area))
    return context_draw(drawing_area);
  return FALSE;
}

gboolean context_idle(gpointer data)
{
  GtkWidget *drawing_area = (GtkWidget *) data;
  if (gtk_widget_get_realized(drawing_area))
    return context_draw(drawing_area);
  return FALSE;
}

gboolean context_draw(GtkWidget *drawing_area)
{
  TetrisMatrix *matrix;
  TetrisCellInfo *info;
  TetrisPlayer *player;
  GSList *elem;
  cairo_t *cairo;
  int x, y;

  g_static_mutex_lock(&draw_mutex);

  player = g_object_get_data(G_OBJECT(drawing_area), "player");
  cairo = g_object_get_data(G_OBJECT(drawing_area), "cairo");
  if (cairo == NULL)
    cairo = gdk_cairo_create(drawing_area->window);
  matrix = tetris_player_get_matrix(player);

  /* default color */
  gdk_cairo_set_source_color(cairo, &colors[DEFAULT_CELL]);

  for (x = 0; x < tetris_matrix_get_width(matrix); x++)
    for (y = 0; y < tetris_matrix_get_height(matrix); y++)
      context_cairo_draw_cell(cairo, x, y,
                              tetris_matrix_get_cell(matrix, x, y));

  x = tetris_player_get_piece_position(player)[0];
  y = tetris_player_get_piece_position(player)[1];
  for (elem = tetris_player_get_piece(player); elem != NULL;
       elem = elem->next) {
    info = elem->data;
    context_cairo_draw_cell(cairo, x + info->x, y + info->y, info->cell);
  }

  g_static_mutex_unlock(&draw_mutex);
  return TRUE;
}

void context_cairo_draw_cell(cairo_t *cairo, int x, int y, TetrisCell cell)
{
  cairo_save(cairo);
  if (cell < N_COLORS)
    gdk_cairo_set_source_color(cairo, &colors[cell]);
  cairo_translate(cairo, x*CELL_SIZE, y*CELL_SIZE);
  cairo_rectangle(cairo, 0, 0, CELL_SIZE, CELL_SIZE);
  cairo_fill(cairo);
  cairo_restore(cairo);
}

GtkWidget *context_new(void)
{
  return GTK_WIDGET(g_object_new(CONTEXT_TYPE, NULL));
}

void context_add_player(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;

  drawing_area = gtk_drawing_area_new();
  context->drawing_areas = g_slist_prepend(context->drawing_areas,
                                           (gpointer) drawing_area);

  g_signal_connect_after(G_OBJECT(drawing_area), "realize",
                         G_CALLBACK(context_realize), drawing_area);
  g_signal_connect(G_OBJECT(drawing_area), "expose_event",
                   G_CALLBACK(context_expose), drawing_area);
  gtk_idle_add(context_idle, drawing_area);

  g_object_set_data(G_OBJECT(drawing_area), "player", player);
  g_object_set_data(G_OBJECT(drawing_area), "cairo", NULL);

  gtk_container_add(GTK_CONTAINER(context), drawing_area);

  gtk_widget_show(drawing_area);
}

void context_remove_player(Context *context, TetrisPlayer *player)
{
  GtkWidget *drawing_area;
  /* take care of not removing a player while we're drawing */
  g_static_mutex_lock(&draw_mutex);

  GSList *elem = g_slist_find_custom(context->drawing_areas,
                                     (gpointer) player,
                                     (GCompareFunc) context_compare);
  if (elem == NULL)
    g_return_if_reached();
  drawing_area = (GtkWidget *) elem->data;
  context->drawing_areas = g_slist_remove_link(context->drawing_areas,
                                               elem);
  gtk_container_remove(GTK_CONTAINER(context), drawing_area);
  gtk_idle_remove_by_data(drawing_area);
  /*gtk_widget_set_realized(drawin_area, FALSE)*/
  g_free(drawing_area);

  g_static_mutex_unlock(&draw_mutex);
}
