#include <stdlib.h>
#include <check.h>
/*#include <libtetris.h>*/

START_TEST(test_dummy)
{
  fail_unless(1 == 1,
              "One does not equals one");
}
END_TEST

Suite *suite()
{
  Suite *s = suite_create("Tetristar");

  TCase *tc_dummy = tcase_create("Dummy");
  tcase_add_test(tc_dummy, test_dummy);
  suite_add_tcase(s, tc_dummy);

  return s;
}

int main()
{
  int number_failed;
  Suite *s = suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
