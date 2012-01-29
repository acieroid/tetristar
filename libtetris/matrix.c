#include "matrix.h"

TetrisCellInfo *tetris_cell_info_new(int x, int y, TetrisCell cell)
{
  TetrisCellInfo *info = g_malloc(sizeof(*info));
  info->x = x;
  info->y = y;
  info->cell = cell;
  return info;
}

void tetris_cell_info_free(TetrisCellInfo *info)
{
  g_free(info);
}

TetrisMatrix *tetris_matrix_new(int width, int height)
{
  int x, y;
  TetrisMatrix *matrix = g_malloc(sizeof(*matrix));
  matrix->width = width;
  matrix->height = height;
  matrix->changes = NULL;
  matrix->content = g_malloc(width*sizeof(*matrix->content));
  for (x = 0; x < width; x++) {
    matrix->content[x] = g_malloc(height*sizeof(**matrix->content));
    for (y = 0; y < height; y++) {
      matrix->content[x][y] = 0;
    }
  }
  return matrix;
}
    
void tetris_matrix_free(TetrisMatrix *matrix)
{
  int x;
  for (x = 0; x < matrix->width; x++)
    g_free(matrix->content[x]);
  g_free(matrix->content);
  g_slist_free_full(matrix->changes,
                    (GDestroyNotify) tetris_cell_info_free);
  g_free(matrix);
}

int tetris_matrix_get_width(TetrisMatrix *matrix)
{
  return matrix->width;
}

int tetris_matrix_get_height(TetrisMatrix *matrix)
{
  return matrix->height;
}

void tetris_matrix_set_cell(TetrisMatrix *matrix,
                            int x, int y, TetrisCell cell)
{
  if (x >= 0 && y >= 0 && x < matrix->width && y < matrix->height)
    matrix->changes =
      g_slist_prepend(matrix->changes,
                      (gpointer) tetris_cell_info_new(x, y, cell));
  else {
    g_warning("tetris_matrix_set_cell called on cell %d, %d on a matrix of size %dx%d",
              x, y, matrix->width, matrix->height);
    g_return_if_reached();
  }
}

TetrisCell tetris_matrix_get_cell(TetrisMatrix *matrix,
                                  int x, int y)
{
  if (x >= 0 && y >= 0 && x < matrix->width && y < matrix->height)
    /* in the matrix */
    return matrix->content[x][y];
  if (x >= 0 && x < matrix->width && y < 0)
    /* on the top of the matrix, where the pieces starts to fall */
    return 0;
  /* borders are inaccessibles */
  return 1;
}

static int compare_cell_with_pos(TetrisCellInfo *cell, int position[2])
{
  if (cell->x == position[0] && cell->y == position[1]) {
    return 0;
  }
  return -1;
}

TetrisCell tetris_matrix_get_uncommited_cell(TetrisMatrix *matrix,
                                             int x, int y)
{
  int pos[2] = { x, y };
  GSList *l;

  /* Look if the cell is in the changes list */
  l = g_slist_find_custom(matrix->changes, pos,
                          (GCompareFunc) compare_cell_with_pos);
  if (l)
    return ((TetrisCellInfo *)l->data)->cell;
  else
    return tetris_matrix_get_cell(matrix, x, y);
}

GSList *tetris_matrix_diff(TetrisMatrix *matrix)
{
  return matrix->changes;
}

void tetris_matrix_commit(TetrisMatrix *matrix)
{
  GSList *elem;
  TetrisCellInfo *info;

  /* reverse the changes to apply them in the right order */
  matrix->changes = g_slist_reverse(matrix->changes);

  /* apply the changes */
  for (elem = matrix->changes; elem != NULL; elem = elem->next) {
    info = (TetrisCellInfo *) elem->data;
    matrix->content[info->x][info->y] = info->cell;
  }

  /* and free the changes list */
  g_slist_free_full(matrix->changes,
                    (GDestroyNotify) tetris_cell_info_free);
  matrix->changes = NULL;
}
