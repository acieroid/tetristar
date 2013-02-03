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

/**
 * @file matrix.h
 * @brief Implements cells and the tetris' matrix (ie. the game field)
 */
#ifndef LIBTETRIS_FIELD_H
#define LIBTETRIS_FIELD_H

#include <glib.h>

/**
 * A cell is simply a positive integer.
 */
typedef unsigned int TetrisCell;

/**
 * The empty cell
 */
static const TetrisCell EMPTY = 0;

/**
 * The cell info structure is used to have the information on a cell
 * position and its type.
 */
typedef struct TetrisCellInfo {
  int x, y;
  TetrisCell cell;
} TetrisCellInfo;

/**
 * Return a newly allocated TetrisCellInfo
 */
TetrisCellInfo *tetris_cell_info_new(int x, int y, TetrisCell cell);

/**
 * Free a TetrisCellInfo
 */
void tetris_cell_info_free(TetrisCellInfo *info);

/**
 * The matrix is the game field. Usually it is a 10x22 grid.
 */
typedef struct TetrisMatrix {
  TetrisCell **content;
  TetrisCell **changes;
  int width;
  int height;
} TetrisMatrix;

/**
 * Return a new TetrisMatrix of the given dimensions
 */
TetrisMatrix *tetris_matrix_new(int width, int height);

/**
 * Free a TetrisMatrix
 */
void tetris_matrix_free(TetrisMatrix *matrix);

/**
 * Return the width of the matrix
 */
int tetris_matrix_get_width(TetrisMatrix *matrix);

/**
 * Return the height of the matrix
 */
int tetris_matrix_get_height(TetrisMatrix *matrix);

/**
 * Change a cell on the matrix. Note that the change aren't directly
 * applied, they should be commited first.
 * @sa tetris_matrix_commit
 */
void tetris_matrix_set_cell(TetrisMatrix *matrix,
                            int x, int y, TetrisCell cell);

/**
 * Get a cell from the matrix
 */
TetrisCell tetris_matrix_get_cell(TetrisMatrix *matrix,
                                  int x, int y);

/**
 * Get a cell from the matrix, looking first at cell which aren't
 * commited yet
 * @sa tetris_matrix_get_cell
 */
TetrisCell tetris_matrix_get_uncommited_cell(TetrisMatrix *matrix,
                                             int x, int y);

/**
 * Return a list of all the filled (and commited) cells in this matrix
 * @sa tetris_matrix_diff
 */
GSList *tetris_matrix_get_cells(TetrisMatrix *matrix);

/**
 * Return a list of cell infos that represents the cells that have
 * been changed since the last commit. The list should be freed after
 * use.
 * @sa tetris_matrix_commit
 */
GSList *tetris_matrix_diff(TetrisMatrix *matrix);

/**
 * Commit the changes on the matrix
 */
void tetris_matrix_commit(TetrisMatrix *matrix);

#endif /* LIBTETRIS_FIELD_H */
