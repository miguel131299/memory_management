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
  refcount_dec(snek_obj->data.v_array.elements[index]);
  snek_obj->data.v_array.elements[index] = value;
  refcount_inc(value);
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
    case ARRAY:
      for (size_t i = 0; i < obj->data.v_array.size; ++i) {
        refcount_dec(obj->data.v_array.elements[i]);
      }
      free(obj->data.v_array.elements);
      return;
    default:
      assert(false);
  }
  free(obj);
}

// don't touch below this line

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

static MunitResult test_array_set(const MunitParameter params[], void* data) {
  snek_object_t* foo = new_snek_integer(1);
  snek_object_t* array = new_snek_array(1);

  snek_array_set(array, 0, foo);
  munit_assert_int(foo->refcount, ==, 2);
  // munit_assert_false(boot_is_freed(foo));

  refcount_dec(foo);
  refcount_dec(array);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_array_free(const MunitParameter params[], void* data) {
  snek_object_t* foo = new_snek_integer(1);
  snek_object_t* bar = new_snek_integer(2);
  snek_object_t* baz = new_snek_integer(3);
  snek_object_t* array = new_snek_array(2);

  snek_array_set(array, 0, foo);
  snek_array_set(array, 1, bar);

  munit_assert_int(foo->refcount, ==, 2);
  munit_assert_int(bar->refcount, ==, 2);
  munit_assert_int(baz->refcount, ==, 1);

  refcount_dec(foo);
  // munit_assert_false(boot_is_freed(foo));

  snek_array_set(array, 0, baz);
  // munit_assert_true(boot_is_freed(foo));

  refcount_dec(bar);
  refcount_dec(baz);
  refcount_dec(array);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_vector3_refcounting(const MunitParameter params[],
                                            void* data) {
  snek_object_t* foo = new_snek_integer(1);
  snek_object_t* bar = new_snek_integer(2);
  snek_object_t* baz = new_snek_integer(3);
  snek_object_t* vec = new_snek_vector3(foo, bar, baz);

  munit_assert_int(foo->refcount, ==, 2);
  munit_assert_int(bar->refcount, ==, 2);
  munit_assert_int(baz->refcount, ==, 2);

  refcount_dec(foo);
  // munit_assert_false(boot_is_freed(foo));

  refcount_dec(vec);
  // munit_assert_true(boot_is_freed(foo));
  // munit_assert_false(boot_is_freed(bar));
  // munit_assert_false(boot_is_freed(baz));

  refcount_dec(bar);
  refcount_dec(baz);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_int_has_refcount(const MunitParameter params[],
                                         void* data) {
  snek_object_t* obj = new_snek_integer(10);
  munit_assert_int(obj->refcount, ==, 1);
  free(obj);
  return MUNIT_OK;
}

static MunitResult test_inc_refcount(const MunitParameter params[],
                                     void* data) {
  snek_object_t* obj = new_snek_float(4.20);
  munit_assert_int(obj->refcount, ==, 1);

  refcount_inc(obj);
  munit_assert_int(obj->refcount, ==, 2);

  free(obj);
  return MUNIT_OK;
}

static MunitResult test_dec_refcount(const MunitParameter params[],
                                     void* data) {
  snek_object_t* obj = new_snek_float(4.20);

  refcount_inc(obj);
  munit_assert_int(obj->refcount, ==, 2);

  refcount_dec(obj);
  munit_assert_int(obj->refcount, ==, 1);
  // munit_assert_false(boot_is_freed(obj));

  free(obj);
  return MUNIT_OK;
}

static MunitResult test_refcount_free_is_called(const MunitParameter params[],
                                                void* data) {
  snek_object_t* obj = new_snek_float(4.20);

  refcount_inc(obj);
  munit_assert_int(obj->refcount, ==, 2);

  refcount_dec(obj);
  munit_assert_int(obj->refcount, ==, 1);

  refcount_dec(obj);
  // munit_assert_true(boot_is_freed(obj));
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_allocated_string_is_freed(const MunitParameter params[],
                                                  void* data) {
  snek_object_t* obj = new_snek_string("Hello @wagslane!");

  refcount_inc(obj);
  munit_assert_int(obj->refcount, ==, 2);

  refcount_dec(obj);
  munit_assert_int(obj->refcount, ==, 1);
  munit_assert_string_equal(obj->data.v_string, "Hello @wagslane!");

  refcount_dec(obj);
  // munit_assert_true(boot_is_freed(obj));
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

int main() {
  MunitTest tests[] = {
      {"/array_set", test_array_set, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"/array_free", test_array_free, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {"/has_refcount", test_int_has_refcount, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"/inc_refcount", test_inc_refcount, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {"/dec_refcount", test_dec_refcount, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {"/free_refcount", test_refcount_free_is_called, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"/string_freed", test_allocated_string_is_freed, NULL, NULL,
       MUNIT_TEST_OPTION_NONE, NULL},
      {"/vector3", test_vector3_refcounting, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  MunitSuite suite = {"/refcount", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};

  return munit_suite_main(&suite, NULL, 0, NULL);
}
