/**
 * Copyright (c) 2013, Quentin Stievenart
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

GSList *tetris_matrix_get_cells(TetrisMatrix *matrix)
{
  int x, y;
  TetrisCellInfo *cell;
  GSList *cells = NULL;

  for (x = 0; x < matrix->width; x++) {
    for (y = 0; y < matrix->height; y++) {
      if (matrix->content[x][y] != EMPTY) {
        cell = tetris_cell_info_new(x, y, matrix->content[x][y]);
        cells = g_slist_prepend(cells, (gpointer) cell);
      }
    }
  }

  return cells;
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
