#include <stddef.h>
#include <stdlib.h>
#include "../munit/munit.h"

typedef struct SnekObject snek_object_t;

typedef struct SnekVector {
  snek_object_t* x;
  snek_object_t* y;
  snek_object_t* z;
} snek_vector_t;

typedef struct SnekArray {
  int size;
  snek_object_t** elements;
} snek_array_t;

typedef enum SnekObjectKind {
  INTEGER,
  FLOAT,
  STRING,
  VECTOR3,
  ARRAY
} snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
  char* v_string;
  snek_vector_t v_vector3;
  snek_array_t v_array;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value);
snek_object_t* new_snek_float(float value);
snek_object_t* new_snek_string(char* value);
snek_object_t* new_snek_vector3(snek_object_t* x, snek_object_t* y,
                                snek_object_t* z);

snek_object_t* new_snek_array(size_t size) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  snek_object_t** ptr = calloc(sizeof(snek_object_t*), size);
  if (!ptr) {
    free(obj);
    return NULL;
  }

  obj->kind = ARRAY;
  snek_array_t arr = {.size = size, .elements = ptr};
  obj->data.v_array = arr;

  return obj;
}

// don't touch below this line

snek_object_t* new_snek_vector3(snek_object_t* x, snek_object_t* y,
                                snek_object_t* z) {
  if (x == NULL || y == NULL || z == NULL) {
    return NULL;
  }

  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = VECTOR3;
  obj->data.v_vector3 = (snek_vector_t){.x = x, .y = y, .z = z};

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

static MunitResult test_create_empty_array(const MunitParameter params[],
                                           void* data) {
  snek_object_t* obj = new_snek_array(2);

  munit_assert_int(obj->kind, ==, ARRAY);
  munit_assert_int(obj->data.v_array.size, ==, 2);

  free(obj->data.v_array.elements);
  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_used_calloc(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_array(2);

  munit_assert_null(obj->data.v_array.elements[0]);
  munit_assert_null(obj->data.v_array.elements[1]);

  free(obj->data.v_array.elements);
  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/empty", test_create_empty_array, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/calloc", test_used_calloc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-array", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
