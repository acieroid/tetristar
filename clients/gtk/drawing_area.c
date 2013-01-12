#include "drawing_area.h"

static void drawing_area_class_init(DrawingAreaClass *klass);
static void drawing_area_init(DrawingArea *drawing_area);
static void drawing_area_realize(DrawingArea *drawing_area, gpointer data);
static gboolean drawing_area_configure(DrawingArea *drawing_area,
                                       GdkEvent *event,
                                       gpointer data);
static gboolean drawing_area_expose(DrawingArea *drawing_area,
                                    GdkEventExpose *event,
                                    gpointer data);
static gboolean drawing_area_update(gpointer data);
static gboolean drawing_area_draw(DrawingArea *drawing_area);
static void drawing_area_cairo_draw_cell(cairo_t *cairo, int x, int y, TetrisCell cell);

#define N_COLORS 8
static int default_cell = 1;
static int cell_size = 10;
static const char *colors_names[N_COLORS] = {
  "black", "magenta", "orange", "blue", "cyan", "green", "red", "yellow"
};
static GdkColor colors[N_COLORS] = { {} };
static GdkColor white = {};

GType drawing_area_get_type(void)
{
  static GType drawing_area_type = 0;

  if (!drawing_area_type) {
    const GTypeInfo drawing_area_info = {
      sizeof(DrawingAreaClass), NULL, NULL,
      (GClassInitFunc) drawing_area_class_init, NULL, NULL,
      sizeof(DrawingArea), 0,
      (GInstanceInitFunc) drawing_area_init,
    };

    drawing_area_type = g_type_register_static(GTK_TYPE_HBOX, "DrawingArea",
                                               &drawing_area_info, 0);
  }

  return drawing_area_type;
}

void drawing_area_class_init(DrawingAreaClass *klass)
{
  int i;
  if (colors != NULL)
    for (i = 0; i < N_COLORS; i++)
      gdk_color_parse(colors_names[i], &colors[i]);
  gdk_color_parse("white", &white);
}

void drawing_area_init(DrawingArea *drawing_area)
{
  drawing_area->field = gtk_drawing_area_new();
  drawing_area->next_piece = gtk_drawing_area_new();
  drawing_area->left_vbox = gtk_vbox_new(FALSE, 0);
  drawing_area->right_vbox = gtk_vbox_new(FALSE, 0);
  drawing_area->info_hbox = gtk_hbox_new(FALSE, 0);
  drawing_area->number_label = gtk_label_new("");
  drawing_area->name_label = gtk_label_new("");
  drawing_area->next_piece_label = gtk_label_new("Next piece:");

  g_signal_connect_after(G_OBJECT(drawing_area->field), "realize",
                         G_CALLBACK(drawing_area_realize), drawing_area->field);
  g_signal_connect(G_OBJECT(drawing_area->field), "configure-event",
                   G_CALLBACK(drawing_area_configure), drawing_area->field);
  g_signal_connect(G_OBJECT(drawing_area->field), "expose-event",
                   G_CALLBACK(drawing_area_expose), drawing_area->field);

  g_signal_connect_after(G_OBJECT(drawing_area->next_piece), "realize",
                         G_CALLBACK(drawing_area_realize), drawing_area->next_piece);
  g_signal_connect(G_OBJECT(drawing_area->next_piece), "configure-event",
                   G_CALLBACK(drawing_area_configure), drawing_area->next_piece);
  g_signal_connect(G_OBJECT(drawing_area->next_piece), "expose-event",
                   G_CALLBACK(drawing_area_expose), drawing_area->next_piece);

  /* we try to refresh each 100ms */
  drawing_area->timeout_tag = g_timeout_add(100, drawing_area_update, drawing_area);

  gtk_container_add(GTK_CONTAINER(drawing_area->info_hbox),
                    drawing_area->number_label);
  gtk_container_add(GTK_CONTAINER(drawing_area->info_hbox),
                    drawing_area->name_label);

  gtk_container_add(GTK_CONTAINER(drawing_area->left_vbox),
                    drawing_area->info_hbox);
  gtk_container_add(GTK_CONTAINER(drawing_area->left_vbox),
                    drawing_area->field);

  gtk_container_add(GTK_CONTAINER(drawing_area->right_vbox),
                    drawing_area->next_piece_label);
  gtk_container_add(GTK_CONTAINER(drawing_area->right_vbox),
                    drawing_area->next_piece);

  gtk_container_add(GTK_CONTAINER(drawing_area),
                    drawing_area->left_vbox);
  gtk_container_add(GTK_CONTAINER(drawing_area),
                    drawing_area->right_vbox);

  gtk_widget_show_all(drawing_area->left_vbox);
  gtk_widget_show_all(drawing_area->right_vbox);
  drawing_area->cairo = NULL;
  drawing_area->changed = TRUE;
  drawing_area->cell_size = 0;
}

GtkWidget *drawing_area_new(TetrisPlayer *player)
{
  DrawingArea *drawing_area = DRAWING_AREA(g_object_new(DRAWING_AREA_TYPE, NULL));
  drawing_area_set_player(drawing_area, player);
  return GTK_WIDGET(drawing_area);
}

void drawing_area_set_player(DrawingArea *drawing_area, TetrisPlayer *player)
{
  gchar *number_str;

  drawing_area->player = player;

  number_str = g_strdup_printf("%d", drawing_area->player->id);
  gtk_label_set_markup(GTK_LABEL(drawing_area->number_label), number_str);
  g_free(number_str);

  gtk_label_set_markup(GTK_LABEL(drawing_area->name_label),
                       drawing_area->player->nick);
}

TetrisPlayer *drawing_area_get_player(DrawingArea *drawing_area)
{
  return drawing_area->player;
}

void drawing_area_set_changed(DrawingArea *drawing_area)
{
  drawing_area->changed = TRUE;
}

void drawing_area_realize(DrawingArea *drawing_area, gpointer data)
{
}

gboolean drawing_area_configure(DrawingArea *drawing_area,
                                GdkEvent *event,
                                gpointer data)
{
  GtkAllocation alloc;
  GtkWidget *draw = (GtkWidget *) data;

  gtk_widget_get_allocation(draw, &alloc);
  drawing_area->cell_size = alloc.height/22;
  return drawing_area_draw(drawing_area);
}

gboolean drawing_area_expose(DrawingArea *drawing_area,
                             GdkEventExpose *event,
                             gpointer data)
{
  return drawing_area_draw(drawing_area);
}

gboolean drawing_area_update(gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;

  if (drawing_area->changed) {
    drawing_area->changed = FALSE;
    return drawing_area_draw(drawing_area);
  }
  return TRUE;
}

gboolean drawing_area_draw(DrawingArea *drawing_area)
{
  TetrisMatrix *matrix;
  TetrisCellInfo *info;
  TetrisPlayer *player;
  GSList *elem;
  cairo_t *cairo;
  int x, y;
  char *notplaying = "Not playing";
  cairo_text_extents_t extents;

  if (!gtk_widget_get_realized(drawing_area->field))
    return FALSE;

  player = drawing_area->player;
  cairo = drawing_area->cairo;
  if (cairo == NULL)
    cairo = gdk_cairo_create(drawing_area->field->window);
  matrix = tetris_player_get_matrix(player);

  /* default color */
  gdk_cairo_set_source_color(cairo, &colors[default_cell]);

  for (x = 0; x < tetris_matrix_get_width(matrix); x++)
    for (y = 0; y < tetris_matrix_get_height(matrix); y++)
      drawing_area_cairo_draw_cell(cairo, x, y,
                              tetris_matrix_get_cell(matrix, x, y));

  x = tetris_player_get_piece_position(player)[0];
  y = tetris_player_get_piece_position(player)[1];
  for (elem = tetris_player_get_piece(player); elem != NULL;
       elem = elem->next) {
    info = elem->data;
    drawing_area_cairo_draw_cell(cairo, x + info->x, y + info->y, info->cell);
  }

  if (!tetris_player_is_playing(player)) {
    cairo_save(cairo);
    gdk_cairo_set_source_color(cairo, &white);
    cairo_set_font_size(cairo, 15);
    cairo_text_extents(cairo, notplaying, &extents);
    cairo_translate(cairo,
                    tetris_matrix_get_width(matrix)/2 * cell_size -
                    extents.width/2,
                    tetris_matrix_get_height(matrix)/2 * cell_size);
    cairo_show_text(cairo, "Not playing");
  }

  return TRUE;
}

void drawing_area_cairo_draw_cell(cairo_t *cairo, int x, int y, TetrisCell cell)
{
  cairo_save(cairo);
  if (cell < N_COLORS)
    gdk_cairo_set_source_color(cairo, &colors[cell]);
  cairo_translate(cairo, x*cell_size, y*cell_size);
  cairo_rectangle(cairo, 0, 0, cell_size, cell_size);
  cairo_fill(cairo);
  cairo_restore(cairo);
}

void drawing_area_free(DrawingArea *drawing_area)
{
  g_source_remove(drawing_area->timeout_tag);
  tetris_player_remove(drawing_area->player);
  cairo_destroy(drawing_area->cairo);
  gtk_widget_destroy(GTK_WIDGET(drawing_area));
}
