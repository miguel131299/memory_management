#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"

typedef struct SnekObject snek_object_t;

typedef enum SnekObjectKind {
  INTEGER,
  FLOAT,
  STRING,
  VECTOR3
} snek_object_kind_t;

typedef struct SnekVector {
  snek_object_t* x;
  snek_object_t* y;
  snek_object_t* z;
} snek_vector_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
  char* v_string;
  snek_vector_t v_vector3;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value);
snek_object_t* new_snek_float(float value);
snek_object_t* new_snek_string(char* value);

snek_object_t* new_snek_vector3(snek_object_t* x, snek_object_t* y,
                                snek_object_t* z) {
  if (!x || !y || !z) {
    return NULL;
  }

  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (!obj) {
    return NULL;
  }

  obj->kind = VECTOR3;
  snek_vector_t vec = {.x = x, .y = y, .z = z};
  obj->data.v_vector3 = vec;

  return obj;
}

snek_object_t* new_snek_integer(int value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = INTEGER;
  obj->data.v_int = value;
  return obj;
}

snek_object_t* new_snek_float(float value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = FLOAT;
  obj->data.v_float = value;
  return obj;
}

snek_object_t* new_snek_string(char* value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  int len = strlen(value);
  char* dst = malloc(len + 1);
  if (dst == NULL) {
    free(obj);
    return NULL;
  }

  strcpy(dst, value);

  obj->kind = STRING;
  obj->data.v_string = dst;
  return obj;
}

static MunitResult test_returns_null(const MunitParameter params[],
                                     void* data) {
  snek_object_t* vec = new_snek_vector3(NULL, NULL, NULL);
  munit_assert_null(vec);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_vec_multiple_objects(const MunitParameter params[],
                                             void* data) {
  snek_object_t* x = new_snek_integer(1);
  snek_object_t* y = new_snek_integer(2);
  snek_object_t* z = new_snek_integer(3);
  snek_object_t* vec = new_snek_vector3(x, y, z);

  munit_assert_not_null(vec);

  // Vectors reference existing objects
  munit_assert_ptr_equal(vec->data.v_vector3.x, x);
  munit_assert_ptr_equal(vec->data.v_vector3.y, y);
  munit_assert_ptr_equal(vec->data.v_vector3.z, z);

  // Validate integer values
  munit_assert_int(vec->data.v_vector3.x->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector3.y->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector3.z->data.v_int, ==, 3);

  free(x);
  free(y);
  free(z);
  free(vec);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_vec_same_object(const MunitParameter params[],
                                        void* data) {
  snek_object_t* i = new_snek_integer(1);
  snek_object_t* vec = new_snek_vector3(i, i, i);

  munit_assert_not_null(vec);

  munit_assert_ptr_equal(vec->data.v_vector3.x, i);
  munit_assert_ptr_equal(vec->data.v_vector3.y, i);
  munit_assert_ptr_equal(vec->data.v_vector3.z, i);

  // Values before update
  munit_assert_int(vec->data.v_vector3.x->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector3.y->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector3.z->data.v_int, ==, 1);

  // Update original and check that it reflects in all
  i->data.v_int = 2;

  munit_assert_int(vec->data.v_vector3.x->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector3.y->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector3.z->data.v_int, ==, 2);

  free(i);
  free(vec);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/returns_null", test_returns_null, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/multiple_objects", test_vec_multiple_objects, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/same_object", test_vec_same_object, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-vector", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
