#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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
void refcount_dec(snek_object_t* obj);
void refcount_free(snek_object_t* obj);

bool snek_array_set(snek_object_t* array, size_t index, snek_object_t* value);
snek_object_t* snek_array_get(snek_object_t* snek_obj, size_t index);

bool snek_array_set(snek_object_t* snek_obj, size_t index,
                    snek_object_t* value) {
  if (snek_obj == NULL || value == NULL) {
    return false;
  }
  if (snek_obj->kind != ARRAY) {
    return false;
  }
  if (index >= snek_obj->data.v_array.size) {
    return false;
  }
  refcount_inc(value);
  if (snek_obj->data.v_array.elements[index] != NULL) {
    refcount_dec(snek_obj->data.v_array.elements[index]);
  }
  snek_obj->data.v_array.elements[index] = value;
  return true;
}

void refcount_free(snek_object_t* obj) {
  switch (obj->kind) {
    case INTEGER:
    case FLOAT:
      break;
    case STRING:
      free(obj->data.v_string);
      break;
    case VECTOR3: {
      snek_vector_t vec = obj->data.v_vector3;
      refcount_dec(vec.x);
      refcount_dec(vec.y);
      refcount_dec(vec.z);
      break;
    }
    case ARRAY: {
      snek_array_t array = obj->data.v_array;
      for (size_t i = 0; i < array.size; i++) {
        refcount_dec(array.elements[i]);
      }
      free(array.elements);
      break;
    }
    default:
      assert(false);
  }
  free(obj);
}

snek_object_t* snek_array_get(snek_object_t* snek_obj, size_t index) {
  if (snek_obj == NULL) {
    return NULL;
  }

  if (snek_obj->kind != ARRAY) {
    return NULL;
  }

  if (index >= snek_obj->data.v_array.size) {
    return NULL;
  }

  return snek_obj->data.v_array.elements[index];
}

void refcount_inc(snek_object_t* obj) {
  if (obj == NULL) {
    return;
  }

  obj->refcount++;
  return;
}

void refcount_dec(snek_object_t* obj) {
  if (obj == NULL) {
    return;
  }
  obj->refcount--;
  if (obj->refcount == 0) {
    return refcount_free(obj);
  }
  return;
}

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
  refcount_inc(x);
  refcount_inc(y);
  refcount_inc(z);
  return obj;
}

static MunitResult test_correctly_free(const MunitParameter params[],
                                       void* data) {
  snek_object_t* first = new_snek_array(1);
  snek_object_t* second = new_snek_array(1);

  // refcounts: first = 1, second = 1
  snek_array_set(first, 0, second);
  // refcounts: first = 1, second = 2
  snek_array_set(second, 0, first);
  // refcounts: first = 2, second = 2

  refcount_dec(first);
  refcount_dec(second);

  munit_assert_int(first->refcount, ==, 1);
  munit_assert_int(second->refcount, ==, 1);

  return MUNIT_OK;
}

int main() {
  MunitTest tests[] = {{"/correctly_free", test_correctly_free, NULL, NULL,
                        MUNIT_TEST_OPTION_NONE, NULL}};

  MunitSuite suite = {.prefix = "refcount",
                      .tests = tests,
                      .suites = NULL,
                      .iterations = 1,
                      .options = MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
