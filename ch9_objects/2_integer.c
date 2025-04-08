#include <stdlib.h>
#include "../munit/munit.h"

typedef enum SnekObjectKind { INTEGER } snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (!obj) {
    return NULL;
  }

  obj->kind = INTEGER;
  obj->data.v_int = value;

  return obj;
}

static MunitResult test_positive(const MunitParameter params[], void* data) {
  snek_object_t* int_object = new_snek_integer(42);
  munit_assert_int(int_object->data.v_int, ==,
                   42);  // Must allow positive numbers

  free(int_object);
  return MUNIT_OK;
}

static MunitResult test_zero(const MunitParameter params[], void* data) {
  snek_object_t* int_object = new_snek_integer(0);

  munit_assert_int(int_object->kind, ==, INTEGER);  // Must be INTEGER type
  munit_assert_int(int_object->data.v_int, ==, 0);  // Must equal zero

  free(int_object);
  return MUNIT_OK;
}

static MunitResult test_negative(const MunitParameter params[], void* data) {
  snek_object_t* int_object = new_snek_integer(-5);

  munit_assert_int(int_object->kind, ==, INTEGER);
  munit_assert_int(int_object->data.v_int, ==,
                   -5);  // Must allow negative numbers

  free(int_object);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/positive", test_positive, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/zero", test_zero, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/negative", test_negative, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-integer", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
