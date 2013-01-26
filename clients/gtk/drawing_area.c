#include "drawing_area.h"

static void drawing_area_class_init(DrawingAreaClass *klass);
static void drawing_area_init(DrawingArea *drawing_area);
static gboolean drawing_area_configure(GtkWidget *widget,
                                       GdkEvent *event,
                                       gpointer data);
static gboolean drawing_area_expose(GtkWidget *widget,
                                    GdkEventExpose *event,
                                    gpointer data);
static gboolean drawing_area_update(gpointer data);
static gboolean drawing_area_draw(DrawingArea *drawing_area);
static gboolean drawing_area_configure_next_piece(GtkWidget *widget,
                                                  GdkEvent *event,
                                                  gpointer data);
static gboolean drawing_area_expose_next_piece(GtkWidget *widget,
                                               GdkEventExpose *event,
                                               gpointer data);
static gboolean drawing_area_draw_next_piece(DrawingArea *drawing_area);
static gboolean drawing_area_configure_bonuses(GtkWidget *widget,
                                               GdkEvent *event,
                                               gpointer data);
static gboolean drawing_area_expose_bonuses(GtkWidget *widget,
                                            GdkEventExpose *event,
                                            gpointer data);
static gboolean drawing_area_draw_bonuses(DrawingArea *drawing_area);
static void drawing_area_cairo_draw_cell(DrawingArea *drawing_area,
                                         cairo_t *cairo, int x, int y,
                                         TetrisCell cell);
static void drawing_area_cairo_draw_shadow(DrawingArea *drawing_area,
                                           cairo_t *cairo, int x, int y);

#define N_CELLS 17
static const int DEFAULT_CELL = 1;
static const int IMAGE_SIZE = 20;
static const gchar *DATA_DIR = "../../data/";

#ifdef USE_PNG
# define EXT ".png"
#else
# define EXT ".svg"
#endif

static const gchar *IMAGES_NAMES[N_CELLS] = {
  "black" EXT, "magenta" EXT, "orange" EXT, "blue" EXT, "cyan" EXT,
  "green" EXT, "red" EXT, "yellow" EXT,
  "bonus-a" EXT, "bonus-c" EXT, "bonus-n" EXT, "bonus-r" EXT,
  "bonus-s" EXT, "bonus-b" EXT, "bonus-g" EXT, "bonus-q" EXT,
  "bonus-o" EXT
};
static const gchar *SHADOW_NAME = "shadow" EXT;
#ifdef USE_PNG
static cairo_surface_t *images[N_CELLS] = { NULL };
static cairo_surface_t *shadow_image = NULL;
#else
static RsvgHandle *images[N_CELLS] = { NULL };
static RsvgHandle *shadow_image = NULL;
#endif
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

/* TODO: handle the data somewhere more appropriate */
gchar *find_file(const gchar *filename)
{
  gchar *file = g_strdup_printf("%s/%s", DATA_DIR, filename);
  struct stat buf; /* unused */
  int err = stat(file, &buf); /* TODO: be more portable */
  if (err != 0) {
    g_error("File does not exists: %s", filename);
  }
  return file;
}

#ifdef USE_PNG
cairo_surface_t *load_image(gchar *file)
{
  cairo_status_t status;
  cairo_surface_t *surf = cairo_image_surface_create_from_png(file);
  status = cairo_surface_status(surf);
  if (status != CAIRO_STATUS_SUCCESS) {
    g_error("Cannot load image '%s': %s", file, cairo_status_to_string(status));
  }
  return surf;
}
#else
RsvgHandle *load_image(gchar *file)
{
  RsvgHandle *handle;
  GError *error = g_malloc(sizeof(*error));

  handle = rsvg_handle_new_from_file(file, &error);
  if (handle == NULL) {
    g_error("Cannot load image '%s': %s", file, error->message);
  }

  g_free(error);
  return handle;
}
#endif

void drawing_area_class_init(DrawingAreaClass *klass)
{
  int i;
  gchar *file;

  if (images != NULL) {
    for (i = 0; i < N_CELLS; i++) {
      file = find_file(IMAGES_NAMES[i]);
      images[i] = load_image(file);
      g_free(file);
    }
  }

  file = find_file(SHADOW_NAME);
  shadow_image = load_image(file);
  g_free(file);

  gdk_color_parse("white", &white);
}

void drawing_area_init(DrawingArea *drawing_area)
{
  drawing_area->field = gtk_drawing_area_new();
  drawing_area->next_piece = gtk_drawing_area_new();
  drawing_area->bonuses = gtk_drawing_area_new();
  drawing_area->left_vbox = gtk_vbox_new(FALSE, 0);
  drawing_area->right_vbox = gtk_vbox_new(FALSE, 0);
  drawing_area->info_hbox = gtk_hbox_new(FALSE, 0);
  drawing_area->number_label = gtk_label_new("");
  drawing_area->name_label = gtk_label_new("");
  drawing_area->next_piece_label = gtk_label_new("Next piece:");
  drawing_area->bonuses_label = gtk_label_new("Bonuses:");
  drawing_area->changed = TRUE;
  drawing_area->changed_next_piece = TRUE;
  drawing_area->changed_bonuses = TRUE;
  drawing_area->cell_size = 0;
  drawing_area->shadow = NULL;

  g_signal_connect(G_OBJECT(drawing_area->field), "configure-event",
                   G_CALLBACK(drawing_area_configure), drawing_area);
  g_signal_connect(G_OBJECT(drawing_area->field), "expose-event",
                   G_CALLBACK(drawing_area_expose), drawing_area);

  g_signal_connect(G_OBJECT(drawing_area->next_piece), "configure-event",
                   G_CALLBACK(drawing_area_configure_next_piece), drawing_area);
  g_signal_connect(G_OBJECT(drawing_area->next_piece), "expose-event",
                   G_CALLBACK(drawing_area_expose_next_piece), drawing_area);

  g_signal_connect(G_OBJECT(drawing_area->bonuses), "configure-event",
                   G_CALLBACK(drawing_area_configure_bonuses), drawing_area);
  g_signal_connect(G_OBJECT(drawing_area->next_piece), "expose-event",
                   G_CALLBACK(drawing_area_expose_bonuses), drawing_area);

  /* we try to refresh each 100ms */
  drawing_area->timeout_tag = g_timeout_add(100, drawing_area_update, drawing_area);

  gtk_box_set_homogeneous(GTK_BOX(drawing_area), TRUE);

  gtk_box_pack_start(GTK_BOX(drawing_area->info_hbox),
                     drawing_area->number_label,
                     FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(drawing_area->info_hbox),
                     drawing_area->name_label,
                     TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(drawing_area->left_vbox),
                     drawing_area->info_hbox,
                     FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(drawing_area->left_vbox),
                     drawing_area->field,
                     TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(drawing_area->right_vbox),
                     drawing_area->next_piece_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(drawing_area->right_vbox),
                     drawing_area->next_piece,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(drawing_area->right_vbox),
                     drawing_area->bonuses_label,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(drawing_area->right_vbox),
                     drawing_area->bonuses,
                     TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(drawing_area),
                     drawing_area->left_vbox,
                     TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(drawing_area),
                     drawing_area->right_vbox,
                     TRUE, TRUE, 0);

  gtk_widget_show_all(GTK_WIDGET(drawing_area));
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

void drawing_area_set_next_piece_changed(DrawingArea *drawing_area)
{
  drawing_area->changed_next_piece = TRUE;
}

#define min(a, b) ((a) < (b) ? (a) : (b))
gboolean drawing_area_configure(GtkWidget *widget,
                                GdkEvent *event,
                                gpointer data)
{
  GtkAllocation alloc;
  DrawingArea *drawing_area = (DrawingArea *) data;

  gtk_widget_get_allocation(widget, &alloc);
  drawing_area->cell_size = min(alloc.height/22, alloc.width/10);

  return drawing_area_draw(drawing_area);
}

gboolean drawing_area_expose(GtkWidget *widget,
                             GdkEventExpose *event,
                             gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;
  return drawing_area_draw(drawing_area);
}

gboolean drawing_area_update(gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;
  gboolean res = TRUE;

  if (drawing_area->changed) {
    drawing_area->changed = FALSE;
    res = res && drawing_area_draw(drawing_area);
  }

  if (drawing_area->changed_next_piece) {
    drawing_area->changed_next_piece = FALSE;
    res = res && drawing_area_draw_next_piece(drawing_area);
  }

  if (drawing_area->changed_bonuses) {
    drawing_area->changed_bonuses = FALSE;
    res = res && drawing_area_draw_bonuses(drawing_area);
  }

  return res;
}

gboolean drawing_area_draw(DrawingArea *drawing_area)
{
  TetrisMatrix *matrix;
  TetrisCellInfo *info;
  TetrisPlayer *player;
  GSList *elem;
  cairo_t *cairo;
  int x, y;
  cairo_text_extents_t extents;
  cairo_status_t status;
  cairo_matrix_t zoom;
  char *notplaying = "Not playing";

  if (!gtk_widget_get_realized(drawing_area->field))
    return FALSE;

  player = drawing_area->player;
  /* It seems to be necesarry to create the cairo context every time we want to draw */
  cairo = gdk_cairo_create(drawing_area->field->window);
  status = cairo_status(cairo);
  if (status != CAIRO_STATUS_SUCCESS) {
    g_error("Cannot create cairo context: %s", cairo_status_to_string(status));
    g_return_val_if_reached(FALSE);
  }

  /* scale the drawing */
  cairo_matrix_init_scale(&zoom,
                          ((double) drawing_area->cell_size)/IMAGE_SIZE,
                          ((double) drawing_area->cell_size)/IMAGE_SIZE);
  cairo_transform(cairo, &zoom);

  matrix = tetris_player_get_matrix(player);
  for (x = 0; x < tetris_matrix_get_width(matrix); x++) {
    for (y = 0; y < tetris_matrix_get_height(matrix); y++) {
      drawing_area_cairo_draw_cell(drawing_area, cairo, x, y,
                                   tetris_matrix_get_cell(matrix, x, y));
    }
  }

  x = tetris_player_get_piece_position(player)[0];
  y = tetris_player_get_piece_position(player)[1];
  for (elem = tetris_player_get_piece(player); elem != NULL;
       elem = elem->next) {
    info = elem->data;
    drawing_area_cairo_draw_cell(drawing_area, cairo,
                                 x + info->x, y + info->y, info->cell);
  }

  for (elem = drawing_area->shadow; elem != NULL; elem = elem->next) {
    info = elem->data;
    drawing_area_cairo_draw_shadow(drawing_area, cairo,
                                   info->x, info->y);
  }

  if (!tetris_player_is_playing(player)) {
    cairo_save(cairo);
    gdk_cairo_set_source_color(cairo, &white);
    cairo_set_font_size(cairo, 15);
    cairo_text_extents(cairo, notplaying, &extents);
    cairo_translate(cairo,
                    tetris_matrix_get_width(matrix)/2 * IMAGE_SIZE -
                    extents.width/2,
                    tetris_matrix_get_height(matrix)/2 * IMAGE_SIZE);
    cairo_show_text(cairo, "Not playing");
    cairo_restore(cairo);
  }

  cairo_destroy(cairo);
  return TRUE;
}

void drawing_area_cairo_draw_cell(DrawingArea *drawing_area, cairo_t *cairo, int x, int y, TetrisCell cell)
{
  cairo_save(cairo);
  cairo_translate(cairo, x*IMAGE_SIZE, y*IMAGE_SIZE);
#ifdef USE_PNG
  cairo_rectangle(cairo, 0, 0, IMAGE_SIZE, IMAGE_SIZE);

  if (cell < N_CELLS) {
    cairo_set_source_surface(cairo, images[cell],
                             x*IMAGE_SIZE, y*IMAGE_SIZE);
  }
  cairo_fill(cairo);
#else
  rsvg_handle_render_cairo(images[cell], cairo);
#endif

  cairo_restore(cairo);
}

void drawing_area_cairo_draw_shadow(DrawingArea *drawing_area, cairo_t *cairo, int x, int y)
{
  cairo_save(cairo);
  cairo_translate(cairo, x*IMAGE_SIZE, y*IMAGE_SIZE);
#ifdef USE_PNG
  cairo_rectangle(cairo, 0, 0, IMAGE_SIZE, IMAGE_SIZE);

  if (cell < N_CELLS) {
    cairo_set_source_surface(cairo, shadow_image,
                             x*IMAGE_SIZE, y*IMAGE_SIZE);
  }
  cairo_fill(cairo);
#else
  rsvg_handle_render_cairo(shadow_image, cairo);
#endif

  cairo_restore(cairo);
}


void drawing_area_free(DrawingArea *drawing_area)
{
  g_source_remove(drawing_area->timeout_tag);
  g_slist_free_full(drawing_area->shadow,
                    (GDestroyNotify) tetris_cell_info_free);
  tetris_player_remove(drawing_area->player);
  gtk_widget_destroy(GTK_WIDGET(drawing_area));
}

gboolean drawing_area_configure_next_piece(GtkWidget *widget,
                                           GdkEvent *event,
                                           gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;
  return drawing_area_draw_next_piece(drawing_area);
}

gboolean drawing_area_expose_next_piece(GtkWidget *widget,
                                        GdkEventExpose *event,
                                        gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;
  return drawing_area_draw_next_piece(drawing_area);
}

/* TODO: this shouldn't be hardcoded. It depends on the pieces defined
   in the server's piece.lua */
#define NEXT_PIECE_WIDTH 5
#define NEXT_PIECE_HEIGHT 5
#define NEXT_PIECE_SHIFT_X 2
#define NEXT_PIECE_SHIFT_Y 1
gboolean drawing_area_draw_next_piece(DrawingArea *drawing_area)
{
  TetrisPlayer *player;
  TetrisCellInfo *info;
  cairo_t *cairo;
  cairo_status_t status;
  cairo_matrix_t matrix;
  int x, y;
  GSList *elem;
  GtkAllocation alloc;

  if (!gtk_widget_get_realized(drawing_area->field) ||
      drawing_area->cell_size == 0)
    return FALSE;

  player = drawing_area->player;
  cairo = gdk_cairo_create(drawing_area->next_piece->window);
  status = cairo_status(cairo);
  if (status != CAIRO_STATUS_SUCCESS) {
    g_error("Cannot create cairo context: %s", cairo_status_to_string(status));
    g_return_val_if_reached(FALSE);
  }

  gtk_widget_get_allocation(drawing_area->bonuses, &alloc);
  cairo_matrix_init_translate(&matrix,
                              alloc.width/2 -
                              (NEXT_PIECE_WIDTH*drawing_area->cell_size)/2,
                              0);
  cairo_matrix_scale(&matrix,
                          ((double) drawing_area->cell_size)/IMAGE_SIZE,
                          ((double) drawing_area->cell_size)/IMAGE_SIZE);
  cairo_transform(cairo, &matrix);

  for (x = 0; x < NEXT_PIECE_WIDTH; x++) {
    for (y = 0; y < NEXT_PIECE_HEIGHT; y++) {
      drawing_area_cairo_draw_cell(drawing_area, cairo, x, y, 0);
    }
  }

  for (elem = tetris_player_get_next_piece(player); elem != NULL;
       elem = elem->next) {
    info = elem->data;
    drawing_area_cairo_draw_cell(drawing_area, cairo,
                                 info->x+NEXT_PIECE_SHIFT_X,
                                 info->y+NEXT_PIECE_SHIFT_Y, info->cell);
  }

  cairo_destroy(cairo);
  return TRUE;
}

gboolean drawing_area_configure_bonuses(GtkWidget *widget,
                                        GdkEvent *event,
                                        gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;
  return drawing_area_draw_bonuses(drawing_area);
}

gboolean drawing_area_expose_bonuses(GtkWidget *widget,
                                     GdkEventExpose *event,
                                     gpointer data)
{
  DrawingArea *drawing_area = (DrawingArea *) data;
  return drawing_area_draw_bonuses(drawing_area);
}

#define BONUS_LINES 2
gboolean drawing_area_draw_bonuses(DrawingArea *drawing_area)
{
  TetrisPlayer *player;
  TetrisCell bonus;
  cairo_t *cairo;
  cairo_status_t status;
  cairo_matrix_t zoom;
  GSList *elem;
  GtkAllocation alloc;
  int x, y, width;

  if (!gtk_widget_get_realized(drawing_area->bonuses) ||
      drawing_area->cell_size == 0) {
    return FALSE;
  }

  gtk_widget_get_allocation(drawing_area->bonuses, &alloc);
  width = alloc.width/drawing_area->cell_size;

  player = drawing_area->player;
  cairo = gdk_cairo_create(drawing_area->bonuses->window);
  status = cairo_status(cairo);
  if (status != CAIRO_STATUS_SUCCESS) {
    g_error("Cannot create cairo context: %s", cairo_status_to_string(status));
    g_return_val_if_reached(FALSE);
  }

  cairo_matrix_init_scale(&zoom,
                          ((double) drawing_area->cell_size)/IMAGE_SIZE,
                          ((double) drawing_area->cell_size)/IMAGE_SIZE);
  cairo_transform(cairo, &zoom);

  /* TODO: limit the number of bonuses displayed? */
  elem = tetris_player_get_bonuses(player);
  for (y = 0; y < BONUS_LINES; y++) {
    for (x = 0; x < width; x++) {
      if (elem != NULL) {
        bonus = GPOINTER_TO_UINT(elem->data);
        elem = elem->next;
      } else {
        bonus = (TetrisCell) 0;
      }

      drawing_area_cairo_draw_cell(drawing_area, cairo, x, y, bonus);
    }
  }

  cairo_destroy(cairo);
  return TRUE;
}

void drawing_area_set_bonuses_changed(DrawingArea *drawing_area)
{
  drawing_area->changed_bonuses = TRUE;
}

void drawing_area_set_shadow(DrawingArea *drawing_area, GSList *shadow)
{
  if (drawing_area->shadow != NULL) {
    g_slist_free_full(drawing_area->shadow,
                      (GDestroyNotify) tetris_cell_info_free);
  }

  drawing_area->shadow = shadow;
}
