#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"
#include "assert.h"

typedef struct SnekObject snek_object_t;

typedef struct {
  size_t size;
  snek_object_t** elements;
} snek_array_t;

typedef struct {
  snek_object_t* x;
  snek_object_t* y;
  snek_object_t* z;
} snek_vector_t;

typedef enum SnekObjectKind {
  INTEGER,
  FLOAT,
  STRING,
  VECTOR3,
  ARRAY,
} snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
  char* v_string;
  snek_vector_t v_vector3;
  snek_array_t v_array;
} snek_object_data_t;

typedef struct SnekObject {
  int refcount;
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value);
snek_object_t* new_snek_float(float value);
snek_object_t* new_snek_string(char* value);
snek_object_t* new_snek_vector3(snek_object_t* x, snek_object_t* y,
                                snek_object_t* z);
snek_object_t* new_snek_array(size_t size);

void refcount_inc(snek_object_t* obj);

void refcount_inc(snek_object_t* obj) {
  if (!obj) {
    return;
  }
  obj->refcount++;
}

// don't touch below this line

snek_object_t* _new_snek_object() {
  snek_object_t* obj = calloc(1, sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->refcount = 1;

  return obj;
}

snek_object_t* new_snek_array(size_t size) {
  snek_object_t* obj = _new_snek_object();
  if (obj == NULL) {
    return NULL;
  }

  snek_object_t** elements = calloc(size, sizeof(snek_object_t*));
  if (elements == NULL) {
    free(obj);
    return NULL;
  }

  obj->kind = ARRAY;
  obj->data.v_array = (snek_array_t){.size = size, .elements = elements};

  return obj;
}

snek_object_t* new_snek_vector3(snek_object_t* x, snek_object_t* y,
                                snek_object_t* z) {
  if (x == NULL || y == NULL || z == NULL) {
    return NULL;
  }

  snek_object_t* obj = _new_snek_object();
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = VECTOR3;
  obj->data.v_vector3 = (snek_vector_t){.x = x, .y = y, .z = z};

  return obj;
}

snek_object_t* new_snek_integer(int value) {
  snek_object_t* obj = _new_snek_object();
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = INTEGER;
  obj->data.v_int = value;
  return obj;
}

snek_object_t* new_snek_float(float value) {
  snek_object_t* obj = _new_snek_object();
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = FLOAT;
  obj->data.v_float = value;
  return obj;
}

snek_object_t* new_snek_string(char* value) {
  snek_object_t* obj = _new_snek_object();
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

static MunitResult test_inc_refcount(const MunitParameter params[],
                                     void* data) {
  snek_object_t* obj = new_snek_integer(10);
  munit_assert_int(obj->refcount, ==, 1);

  refcount_inc(obj);
  munit_assert_int(obj->refcount, ==, 2);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_inc_refcount_more(const MunitParameter params[],
                                          void* data) {
  snek_object_t* obj = new_snek_float(4.20);
  munit_assert_int(obj->refcount, ==, 1);

  refcount_inc(obj);
  refcount_inc(obj);
  refcount_inc(obj);
  refcount_inc(obj);
  refcount_inc(obj);

  munit_assert_int(obj->refcount, ==, 6);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_null_obj(const MunitParameter params[], void* data) {
  refcount_inc(NULL);
  munit_assert_true(1);  // Should not crash or assert
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/test_inc_refcount", test_inc_refcount, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_inc_refcount_more", test_inc_refcount_more, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_null_obj", test_null_obj, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/refcount", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
