#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../munit/munit.h"

int* allocate_scalar_list(int size, int multiplier) {
  int* ptr = malloc(sizeof(int) * size);
  if (!ptr) {
    return NULL;
  }

  for (size_t i = 0; i < size; ++i) {
    ptr[i] = i * multiplier;
  }

  return ptr;
}

static MunitResult test_allocate_scalar_list_size(const MunitParameter params[],
                                                  void* data) {
  int size = 5;
  int multiplier = 2;
  int* result = allocate_scalar_list(size, multiplier);
  munit_assert_not_null(result);  // Function should return a non-null pointer
  free(result);
  return MUNIT_OK;
}

static MunitResult test_allocate_scalar_list_values(
    const MunitParameter params[], void* data) {
  int size = 5;
  int multiplier = 2;
  int* result = allocate_scalar_list(size, multiplier);
  int expected[] = {0, 2, 4, 6, 8};
  for (int i = 0; i < size; i++) {
    munit_assert_int(result[i], ==,
                     expected[i]);  // Element does not match expected value
  }
  free(result);
  return MUNIT_OK;
}

static MunitResult test_allocate_scalar_list_zero_multiplier(
    const MunitParameter params[], void* data) {
  int size = 3;
  int multiplier = 0;
  int* result = allocate_scalar_list(size, multiplier);
  for (int i = 0; i < size; i++) {
    munit_assert_int(result[i], ==,
                     0);  // All elements should be 0 with multiplier 0
  }
  free(result);
  return MUNIT_OK;
}

static MunitResult test_allocate_too_much(const MunitParameter params[],
                                          void* data) {
  int size = 1024 * 1024 * 100;  // Large allocation (likely to fail)
  int multiplier = 1;
  int* result = allocate_scalar_list(size, multiplier);
  munit_assert_null(result);  // Giant allocation should result in NULL
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/allocate_scalar_list_size", test_allocate_scalar_list_size, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/allocate_scalar_list_values", test_allocate_scalar_list_values, NULL,
     NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/allocate_scalar_list_zero_multiplier",
     test_allocate_scalar_list_zero_multiplier, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/allocate_too_much", test_allocate_too_much, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/allocate_scalar_list", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
