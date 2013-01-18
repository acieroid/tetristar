/**
 * @file drawing_area.h
 * @brief Implement a drawing area widget, which wraps the
 * GtkDrawingArea with some more infos
 */
#ifndef DRAWING_AREA_H
#define DRAWING_AREA_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <librsvg/rsvg.h>
#include <libtetris.h>

G_BEGIN_DECLS

#define DRAWING_AREA_TYPE            (drawing_area_get_type())
#define DRAWING_AREA(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), DRAWING_AREA_TYPE, DrawingArea))
#define DRAWING_AREA_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST(klass), DRAWING_AREA_TYPE, DrawingArea))
#define IS_DRAWING_AREA(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), DRAWING_AREA_TYPE))
#define IS_DRAWING_AREA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE(klass), DRAWING_AREA_TYPE)

typedef struct DrawingArea {
  GtkHBox widget;

  GtkWidget *left_vbox, *right_vbox;
  GtkWidget *info_hbox;
  GtkWidget *number_label, *name_label;
  GtkWidget *next_piece_label;
  GtkWidget *field, *next_piece;

  TetrisPlayer *player;
  GSList *shadow;
  guint timeout_tag;
  gboolean changed, changed_next_piece;
  int cell_size;
} DrawingArea;

typedef struct DrawingAreaClass {
  GtkHBoxClass parent_class;

  void (* drawing_area) (DrawingArea *drawing_area);
} DrawingAreaClass;

GType drawing_area_get_type(void);
GtkWidget *drawing_area_new(TetrisPlayer *player);
void drawing_area_free(DrawingArea *drawing_area);

/**
 * Set the player for this drawing area
 */
void drawing_area_set_player(DrawingArea *drawing_area, TetrisPlayer *player);

/**
 * Return the player associated with this drawing area
 */
TetrisPlayer *drawing_area_get_player(DrawingArea *drawing_area);

/**
 * Mark the player's field as "changed", which will have as effect that
 * the field will be refreshed on the display
 */
void drawing_area_set_changed(DrawingArea *drawing_area);

/**
 * Mark the player's next piece as "changed".
 */
void drawing_area_set_next_piece_changed(DrawingArea *drawing_area);

/**
 * Set the shadow of this drawing area
 */
void drawing_area_set_shadow(DrawingArea *drawing_area, GSList *shadow);

G_END_DECLS

#endif /* DRAWING_AREA_H */


