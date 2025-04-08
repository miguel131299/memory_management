#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"

// #include "bootlib.h"
typedef struct SnekObject snek_object_t;

int snek_length(snek_object_t* obj);

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
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value);
snek_object_t* new_snek_float(float value);
snek_object_t* new_snek_string(char* value);
snek_object_t* new_snek_vector3(snek_object_t* x, snek_object_t* y,
                                snek_object_t* z);
snek_object_t* new_snek_array(size_t size);
bool snek_array_set(snek_object_t* array, size_t index, snek_object_t* value);
snek_object_t* snek_array_get(snek_object_t* array, size_t index);

int snek_length(snek_object_t* obj) {
  switch (obj->kind) {

    case INTEGER:
    case FLOAT:
      return 1;
    case STRING:
      return strlen(obj->data.v_string);
    case VECTOR3:
      return 3;
    case ARRAY:
      return obj->data.v_array.size;
    default:
      return -1;
  }
}

// don't touch below this line

snek_object_t* new_snek_array(size_t size) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
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

bool snek_array_set(snek_object_t* array, size_t index, snek_object_t* value) {
  if (array == NULL || value == NULL) {
    return false;
  }

  if (array->kind != ARRAY) {
    return false;
  }

  if (index >= array->data.v_array.size) {
    return false;
  }

  // Set the value directly now (already checked size constraint)
  array->data.v_array.elements[index] = value;
  return true;
}

snek_object_t* snek_array_get(snek_object_t* array, size_t index) {
  if (array == NULL) {
    return NULL;
  }

  if (array->kind != ARRAY) {
    return NULL;
  }

  if (index >= array->data.v_array.size) {
    return NULL;
  }

  // Set the value directly now (already checked size constraint)
  return array->data.v_array.elements[index];
}

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

static MunitResult test_integer(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_integer(42);
  munit_assert_int(snek_length(obj), ==, 1);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_float(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_float(3.14);
  munit_assert_int(snek_length(obj), ==, 1);

  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_string(const MunitParameter params[], void* data) {
  snek_object_t* shorter = new_snek_string("hello");
  munit_assert_int(snek_length(shorter), ==, 5);

  snek_object_t* longer = new_snek_string("hello, world");
  munit_assert_int(snek_length(longer), ==, strlen("hello, world"));

  free(shorter->data.v_string);
  free(shorter);
  free(longer->data.v_string);
  free(longer);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_vector3(const MunitParameter params[], void* data) {
  snek_object_t* i = new_snek_integer(1);
  snek_object_t* vec = new_snek_vector3(i, i, i);

  munit_assert_int(snek_length(vec), ==, 3);

  free(i);
  free(vec);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_array(const MunitParameter params[], void* data) {
  snek_object_t* i = new_snek_integer(1);
  snek_object_t* arr = new_snek_array(4);

  munit_assert_true(snek_array_set(arr, 0, i));
  munit_assert_true(snek_array_set(arr, 1, i));
  munit_assert_true(snek_array_set(arr, 2, i));

  munit_assert_int(snek_length(arr), ==, 4);

  free(i);
  free(arr->data.v_array.elements);
  free(arr);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/integer", test_integer, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/float", test_float, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/string", test_string, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vector", test_vector3, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/array", test_array, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-length", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
