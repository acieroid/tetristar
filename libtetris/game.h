/**
 * @file game.h
 * @brief Various game-related utility functions
 * @TODO Put everything related to the game state here (players etc.)
 */
#ifndef LIBTETRIS_GAME_H
#define LIBTETRIS_GAME_H

#include <glib.h>

/**
 * Start a game
 */
void tetris_game_start();

/**
 * Stop a game but doesn't reset it
 */
void tetris_game_stop();

/**
 * Check if a game is started
 */
gboolean tetris_game_is_started();

/**
 * Toggle the pause state of the game
 */
void tetris_game_pause();

/**
 * Return the pause state of the game
 */
gboolean tetris_game_is_paused();

/**
 * Reset a game state
 */
void tetris_game_reset();


#endif /* LIBTETRIS_GAME_H */
