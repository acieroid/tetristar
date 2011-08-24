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
  else
    g_return_if_reached();
}

TetrisCell tetris_matrix_get_cell(TetrisMatrix *matrix,
                                  int x, int y)
{
  if (x >= 0 && y >= 0 && x < matrix->width && y < matrix->height)
    return matrix->content[x][y];
  /* Outside of the matrix: cell is already occupied */
  return 1;
}

GSList *tetris_matrix_diff(TetrisMatrix *matrix)
{
  return matrix->changes;
}

void tetris_matrix_commit(TetrisMatrix *matrix)
{
  GSList *elem;
  TetrisCellInfo *info;

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
