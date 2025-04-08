
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

static MunitResult test_integer_constant(const MunitParameter params[],
                                         void* data) {
  munit_assert_int(INTEGER, ==, 0);  // INTEGER is defined as 0
  return MUNIT_OK;
}

static MunitResult test_integer_obj(const MunitParameter params[], void* data) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  munit_assert_not_null(obj);

  obj->kind = INTEGER;
  obj->data.v_int = 0;

  munit_assert_int(obj->kind, ==, INTEGER);
  munit_assert_int(obj->data.v_int, ==, 0);

  free(obj);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/integer_constant", test_integer_constant, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/integer_obj", test_integer_obj, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-integer-def", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
