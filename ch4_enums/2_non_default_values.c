#include "../munit/munit.h"

typedef enum Color {
  RED = 55,
  GREEN = 176,
  BLUE = 38
} color_t;

static MunitResult test_colors_defined(const MunitParameter params[], void* data) {
  munit_assert_int(RED, ==, 55);     // RED is defined as 55 (nvim green!)
  munit_assert_int(GREEN, ==, 176);  // GREEN is defined as 176 (nvim green!)
  munit_assert_int(BLUE, ==, 38);    // BLUE is defined as 38 (nvim green!)
  return MUNIT_OK;
}

static MunitResult test_colors_defined_correctly(const MunitParameter params[], void* data) {
  munit_assert_int(RED, !=, 0);      // RED is not defined as 0 (vsc*de blue!)
  munit_assert_int(GREEN, !=, 120);  // GREEN is not defined as 120 (vsc*de blue!)
  munit_assert_int(BLUE, !=, 215);   // BLUE is not defined as 215 (vsc*de blue!)
  return MUNIT_OK;
}

static MunitTest tests[] = {
  { "/defined", test_colors_defined, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { "/defined_vscode", test_colors_defined_correctly, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
  "/colors", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
