#include <stdlib.h>
// #include "bootlib.h"  // for boot_all_freed()
#include "../munit/munit.h"

typedef enum SnekObjectKind { INTEGER, FLOAT } snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value);

snek_object_t* new_snek_float(float value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = FLOAT;
  obj->data.v_float = value;
  return obj;
}

// don't touch below this line

snek_object_t* new_snek_integer(int value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = INTEGER;
  obj->data.v_int = value;
  return obj;
}

static MunitResult test_positive(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_float(42.0);
  munit_assert_double(obj->data.v_float, ==, 42.0);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_zero(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_float(0.0);

  munit_assert_int(obj->kind, ==, FLOAT);
  munit_assert_double(obj->data.v_float, ==, 0.0);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_negative(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_float(-5.0);

  munit_assert_int(obj->kind, ==, FLOAT);
  munit_assert_double(obj->data.v_float, ==, -5.0);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/positive", test_positive, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/zero", test_zero, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/negative", test_negative, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-float", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
