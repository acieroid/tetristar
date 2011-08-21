/**
 * @file matrix.h
 * @brief Implements cells and the tetris' matrix (ie. the game field)
 */
#ifndef LIBTETRIS_FIELD_H
#define LIBTETRIS_FIELD_H

#include <glib.h>

/**
 * A cell is simply a positive integer. The cell 0 is the empty cell.
 */
typedef unsigned int TetrisCell;

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
  GSList *changes;
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
 * Return a list of cell infos that represents the cells that have
 * been changed since the last commit
 * @sa tetris_matrix_commit
 */
GSList *tetris_matrix_diff(TetrisMatrix *matrix);

/**
 * Commit the changes on the matrix
 */
void tetris_matrix_commit(TetrisMatrix *matrix);

#endif /* LIBTETRIS_FIELD_H */
