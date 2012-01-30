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
  matrix->content = g_malloc(width*sizeof(*matrix->content));
  matrix->changes = g_malloc(width*sizeof(*matrix->changes));
  for (x = 0; x < width; x++) {
    matrix->content[x] = g_malloc(height*sizeof(**matrix->content));
    matrix->changes[x] = g_malloc(height*sizeof(**matrix->changes));
    for (y = 0; y < height; y++) {
      matrix->content[x][y] = EMPTY;
      matrix->changes[x][y] = EMPTY;
    }
  }
  return matrix;
}
    
void tetris_matrix_free(TetrisMatrix *matrix)
{
  int x;
  for (x = 0; x < matrix->width; x++) {
    g_free(matrix->content[x]);
    g_free(matrix->changes[x]);
  }
  g_free(matrix->content);
  g_free(matrix->changes);
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
  if (x >= 0 && y >= 0 && x < matrix->width && y < matrix->height) {
    matrix->changes[x][y] = cell;
  }
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

TetrisCell tetris_matrix_get_uncommited_cell(TetrisMatrix *matrix,
                                             int x, int y)
{
  if (x >= 0 && y >= 0 && x < matrix->width && y < matrix->height)
    return matrix->changes[x][y];
  if (x >= 0 && x < matrix->width && y < 0)
    return 0;
  return 1;
}

GSList *tetris_matrix_diff(TetrisMatrix *matrix)
{
  int x, y;
  TetrisCellInfo *cell;
  GSList *changes = NULL;

  for (x = 0; x < matrix->width; x++) {
    for (y = 0; y < matrix->height; y++) {
      if (matrix->content[x][y] != matrix->changes[x][y]) {
        cell = tetris_cell_info_new(x, y, matrix->changes[x][y]);
        changes = g_slist_prepend(changes, (gpointer) cell);
      }
    }
  }

  return changes;
}

void tetris_matrix_commit(TetrisMatrix *matrix)
{
  int x, y;

  /* apply the changes */
  for (x = 0; x < matrix->width; x++)
    for (y = 0; y < matrix->height; y++)
      matrix->content[x][y] = matrix->changes[x][y];
}
