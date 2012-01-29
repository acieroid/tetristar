#include <stdlib.h>
#include <check.h>
#include <libtetris.h>

static const int PLAYER_ID = 0;
TetrisPlayer *player;
TetrisMatrix *matrix;

void setup()
{
  player = tetris_player_new(PLAYER_ID);
}

void teardown()
{
  tetris_player_free(player);
}

START_TEST(test_game_start)
{
  tetris_game_start();
  fail_unless(tetris_game_is_started() == TRUE,
              "Game has not been started");
}
END_TEST

START_TEST(test_player)
{
  gchar *nick = "foo";

  fail_unless(player != NULL,
              "Player not allocated");

  fail_unless(tetris_player_get_id(player) == PLAYER_ID,
              "Bad ID assigned to player");

  tetris_player_set_nick(player, nick);
  fail_unless(strcmp(tetris_player_get_nick(player), nick) == 0,
              "Bad nick assigned to player");

  tetris_player_set_admin(player, TRUE);
  fail_unless(tetris_player_is_admin(player) == TRUE,
              "Player not correctly set as admin");

  tetris_player_set_playing(player, TRUE);
  fail_unless(tetris_player_is_playing(player) == TRUE,
              "Player not correctly set as playing");

  tetris_player_add(player);
  fail_unless(tetris_player_find(PLAYER_ID) == player,
              "Didn't find the player");

  fail_unless(tetris_nick_is_available(nick) == FALSE,
              "Used player's nick should not be available");
}
END_TEST

START_TEST(test_matrix_simple)
{
  int x, y, width, height;

  matrix = tetris_player_get_matrix(player);
  fail_unless(matrix != NULL,
              "Player do not have a matrix");

  width = tetris_matrix_get_width(matrix);
  fail_unless(width > 0,
              "Matrix have a negative or nul width");

  height = tetris_matrix_get_height(matrix);
  fail_unless(height > 0,
              "Matrix have a negative or nul height");

  for (x = 0; x < width; x++)
    for (y = 0; y < height; y++)
      fail_unless(tetris_matrix_get_cell(matrix, x, y) == 0,
                  "Matrix not empty");

  for (x = 0; x < width; x++) {
    fail_unless(tetris_matrix_get_cell(matrix, x, -1) == 0,
                "Matrix borders should be non-empty");
    fail_unless(tetris_matrix_get_cell(matrix, x, height) != 0,
                "The top of the matrix should be empty");
  }

  for (y = 0; y < height; y++)
    fail_unless(tetris_matrix_get_cell(matrix, -1, y) != 0 &&
                tetris_matrix_get_cell(matrix, width, y) != 0,
                "Matrix borders should be non-empty");
}
END_TEST

START_TEST(test_game_stop)
{
  tetris_game_stop();
  fail_unless(tetris_game_is_started() == FALSE,
              "Game has not been stopped");
}
END_TEST

Suite *suite()
{
  Suite *s = suite_create("Tetristar");

  TCase *tc_libtetris = tcase_create("Libtetris");
  tcase_add_test(tc_libtetris, test_game_start);
  tcase_add_test(tc_libtetris, test_player);
  tcase_add_test(tc_libtetris, test_matrix_simple);
  tcase_add_test(tc_libtetris, test_game_stop);
  suite_add_tcase(s, tc_libtetris);

  return s;
}

int main()
{
  int number_failed;
  setup();
  Suite *s = suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  teardown();
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
