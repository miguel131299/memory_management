#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "../munit/munit.h"

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

bool snek_array_set(snek_object_t* snek_obj, size_t index,
                    snek_object_t* value) {
  if (!snek_obj || !value) {
    return false;
  }

  if (snek_obj->kind != ARRAY) {
    return false;
  }

  if (index > snek_obj->data.v_array.size) {
    return false;
  }

  snek_obj->data.v_array.elements[index] = value;
  return true;
}

snek_object_t* snek_array_get(snek_object_t* snek_obj, size_t index) {
  if (!snek_obj) {
    return false;
  }

  if (snek_obj->kind != ARRAY) {
    return false;
  }

  if (index > snek_obj->data.v_array.size) {
    return false;
  }

  return snek_obj->data.v_array.elements[index];
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

static MunitResult test_array(const MunitParameter params[], void* data) {
  snek_object_t* obj = new_snek_array(2);
  snek_object_t* first = new_snek_string("First");
  snek_object_t* second = new_snek_integer(3);

  munit_assert_true(snek_array_set(obj, 0, first));
  munit_assert_true(snek_array_set(obj, 1, second));

  snek_object_t* retrieved_first = snek_array_get(obj, 0);
  munit_assert_not_null(retrieved_first);
  munit_assert_int(retrieved_first->kind, ==, STRING);
  munit_assert_ptr_equal(retrieved_first, first);

  snek_object_t* retrieved_second = snek_array_get(obj, 1);
  munit_assert_not_null(retrieved_second);
  munit_assert_int(retrieved_second->kind, ==, INTEGER);
  munit_assert_ptr_equal(retrieved_second, second);

  free(first->data.v_string);
  free(first);
  free(second);
  free(obj->data.v_array.elements);
  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_set_outside_bounds(const MunitParameter params[],
                                           void* data) {
  snek_object_t* obj = new_snek_array(2);
  snek_object_t* outside = new_snek_string("First");

  munit_assert_true(snek_array_set(obj, 1, outside));
  munit_assert_false(snek_array_set(obj, 100, outside));

  free(outside->data.v_string);
  free(outside);
  free(obj->data.v_array.elements);
  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_get_outside_bounds(const MunitParameter params[],
                                           void* data) {
  snek_object_t* obj = new_snek_array(1);
  snek_object_t* first = new_snek_string("First");

  munit_assert_true(snek_array_set(obj, 0, first));
  munit_assert_null(snek_array_get(obj, 1));

  free(first->data.v_string);
  free(first);
  free(obj->data.v_array.elements);
  free(obj);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/set_and_get", test_array, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/set_outside", test_set_outside_bounds, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/get_outside", test_get_outside_bounds, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-array", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
