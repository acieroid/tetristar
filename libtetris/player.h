/**
 * @file player.h
 * Implement the TetrisPlayer structure
 */
#ifndef LIBTETRIS_PLAYER_H
#define LIBTETRIS_PLAYER_H

#include <assert.h>

#include <glib.h>

/**
 * Represents a player
 */
typedef struct TetrisPlayer {
  int id;
  gboolean admin;
  gchar *nick;
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
 * Return TRUE if the player is an admin
 */
gboolean tetris_player_is_admin(TetrisPlayer *player);

/**
 * Set the player as an admin
 */
void tetris_player_set_admin(TetrisPlayer *player);

/**
 * Add a new player
 */
void tetris_player_add(TetrisPlayer *player);

/**
 * Remove a player
 */
void tetris_player_remove(TetrisPlayer *player);

/**
 * Find a player by its nick. If the player is not found, return NULL
 */
TetrisPlayer *tetris_player_find(int id);

/**
 * Return the list of all the players
 */
GSList *tetris_player_all();

/**
 * Check if a nick is available
 * @TODO also check if the nick is well formed (no spaces etc.)
 */
gboolean tetris_nick_is_available(gchar *nick);

#endif /* LIBTETRIS_PLAYER_H */
