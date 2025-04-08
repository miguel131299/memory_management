#include <stdio.h>
#include <stdlib.h>

// #include "bootlib.h"
#include <stdbool.h>
#include <stddef.h>
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

static MunitResult test_vector3_refcounting_same(const MunitParameter params[],
                                                 void* data) {
  snek_object_t* foo = new_snek_integer(1);
  snek_object_t* vec = new_snek_vector3(foo, foo, foo);

  munit_assert_int(foo->refcount, ==, 4);

  refcount_dec(foo);
  munit_assert_int(foo->refcount, ==, 3);

  refcount_dec(vec);
  // munit_assert_true(boot_is_freed(foo));
  // munit_assert_true(boot_is_freed(vec));
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
