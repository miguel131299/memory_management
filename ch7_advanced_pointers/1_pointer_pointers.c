#include <stdlib.h>
#include "../munit/munit.h"
#include "stdlib.h"

void allocate_int(int** pointer_pointer, int value) {
  int* ptr = malloc(sizeof(int));
  *ptr = value;
  *pointer_pointer = ptr;
}

static MunitResult test_allocate(const MunitParameter params[], void* data) {
  int* pointer = NULL;
  allocate_int(&pointer, 10);

  munit_assert_not_null(pointer);      // Should allocate pointer
  munit_assert_int(*pointer, ==, 10);  // Should assign value to pointer

  free(pointer);
  return MUNIT_OK;
}

static MunitResult test_does_not_overwrite(const MunitParameter params[],
                                           void* data) {
  int value = 5;
  int* pointer = &value;

  allocate_int(&pointer, 20);

  munit_assert_int(value, ==, 5);      // Should not overwrite original value
  munit_assert_not_null(pointer);      // Should allocate pointer
  munit_assert_int(*pointer, ==, 20);  // Should assign value to pointer

  free(pointer);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/create", test_allocate, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/overwrite", test_does_not_overwrite, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/allocate_list", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
