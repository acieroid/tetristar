#include "drawing_area.h"

static void drawing_area_class_init(DrawingArea *klass);
static void drawing_area_init(DrawingArea *drawing_area);

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
}

void drawing_area_init(DrawingArea *drawing_area)
{
}

