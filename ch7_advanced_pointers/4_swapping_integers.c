#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "../munit/munit.h"

void swap_ints(int* a, int* b) {
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

static MunitResult test_swap_ints(const MunitParameter params[], void* data) {
  int a = 5;
  int b = 6;

  swap_ints(&a, &b);

  munit_assert_int(a, ==, 6);  // a is now 6
  munit_assert_int(b, ==, 5);  // b is now 5
  return MUNIT_OK;
}

static MunitResult test_swap_ints_same(const MunitParameter params[],
                                       void* data) {
  int a = 5;

  swap_ints(&a, &a);

  munit_assert_int(a, ==, 5);  // a is still 5
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/swap_ints", test_swap_ints, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/swap_ints_same", test_swap_ints_same, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/void-pointer", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
