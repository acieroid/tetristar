/**
 * @file player.h
 * Implement the TetrisPlayer structure
 */
#ifndef LIBTETRIS_PLAYER_H
#define LIBTETRIS_PLAYER_H

#include <glib.h>
#include <string.h>

#include "matrix.h"

/**
 * Represents a player
 */
typedef struct TetrisPlayer {
  int id;
  gboolean admin;
  gboolean playing;
  gchar *nick;
  TetrisMatrix *matrix;
  GSList *piece, *next_piece;
  int piece_position[2];
  GSList *bonuses;
  int points;
} TetrisPlayer;

/**
 * Allocate a new player
 */
TetrisPlayer *tetris_player_new(int id);

/**
 * Free a player allocated by tetris_player_new
 */
void tetris_player_free(TetrisPlayer *player);

/**
 * Get the nick of a player
 */
gchar *tetris_player_get_nick(TetrisPlayer *player);

/**
 * Set the nick of a player
 */
void tetris_player_set_nick(TetrisPlayer *player, gchar *nick);

/**
 * Get the id of a player. The player's id cannot be changed.
 */
int tetris_player_get_id(TetrisPlayer *player);

/**
 * Get the TetrisMatrix of the player
 */
TetrisMatrix *tetris_player_get_matrix(TetrisPlayer *player);

/**
 * Get the player's current piece
 */
GSList *tetris_player_get_piece(TetrisPlayer *player);

/**
 * Set the player's current piece
 */
void tetris_player_set_piece(TetrisPlayer *player, GSList *piece);

/**
 * Get the player's current piece position
 */
int *tetris_player_get_piece_position(TetrisPlayer *player);

/**
 * Get the player's next piece
 */
GSList *tetris_player_get_next_piece(TetrisPlayer *player);

/**
 * Set the player's next piece
 */
void tetris_player_set_next_piece(TetrisPlayer *player, GSList *piece);

/**
 * Set the player's current piece position
 */
void tetris_player_set_piece_position(TetrisPlayer *player,
                                      int position[2]);

/**
 * Return TRUE if the player is an admin
 */
gboolean tetris_player_is_admin(TetrisPlayer *player);

/**
 * Set the player's admin status
 */
void tetris_player_set_admin(TetrisPlayer *player, gboolean status);

/**
 * Return TRUE if the playir is playing
 */
gboolean tetris_player_is_playing(TetrisPlayer *player);

/**
 * Set the player's playing status
 */
void tetris_player_set_playing(TetrisPlayer *player, gboolean status);

/**
 * Add a bonus to a player
 */
void tetris_player_add_bonus(TetrisPlayer *player, TetrisCell bonus);

/**
 * Remove a bonus from a player
 */
void tetris_player_remove_bonus(TetrisPlayer *player, TetrisCell bonus);

/**
 * Check if a player has a certain bonus
 */
gboolean tetris_player_has_bonus(TetrisPlayer *player, TetrisCell bonus);

/**
 * Get the list of bonuses of a player. The elements of the list
 * should be converted back to uints with GPOINTER_TO_UINT
 */
GSList *tetris_player_get_bonuses(TetrisPlayer *player);

/**
 * Add points to the player
 */
void tetris_player_add_points(TetrisPlayer *player, int points);

/**
 * Reset the points of the player
 */
void tetris_player_reset_points(TetrisPlayer *player);

/**
 * Return the ponits of the player
 */
int tetris_player_get_points(TetrisPlayer *player);

/**
 * Add a new player
 */
void tetris_player_add(TetrisPlayer *player);

/**
 * Remove a player
 */
void tetris_player_remove(TetrisPlayer *player);

/**
 * Remove all the players
 */
void tetris_player_remove_all();

/**
 * Find a player by its nick. If the player is not found, return NULL
 */
TetrisPlayer *tetris_player_find(int id);

/**
 * Return the list of all the players
 */
GSList *tetris_player_all();

/**
 * Check if a nick is valid
 */
gboolean tetris_nick_is_valid(gchar *nick);

/**
 * Check if a nick is available (and valid)
 */
gboolean tetris_nick_is_available(gchar *nick);

#endif /* LIBTETRIS_PLAYER_H */
