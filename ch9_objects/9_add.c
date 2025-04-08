#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"

typedef struct SnekObject snek_object_t;

int snek_length(snek_object_t* obj);
snek_object_t* snek_add(snek_object_t* a, snek_object_t* b);

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

snek_object_t* snek_add(snek_object_t* a, snek_object_t* b) {
  if (!a || !b) {
    return NULL;
  }

  switch (a->kind) {

    case INTEGER:
      switch (b->kind) {
        case INTEGER:
          return new_snek_integer(a->data.v_int + b->data.v_int);
        case FLOAT:
          return new_snek_float(a->data.v_int + b->data.v_float);

        case STRING:
        case VECTOR3:
        case ARRAY:
          return NULL;
      }

    case FLOAT:
      switch (b->kind) {
        case INTEGER:
          return new_snek_float(a->data.v_float + b->data.v_int);
        case FLOAT:
          return new_snek_float(a->data.v_float + b->data.v_float);
        case STRING:
        case VECTOR3:
        case ARRAY:
        default:
          return NULL;
      }

    case STRING:
      if (b->kind != STRING) {
        return NULL;
      }

      // +1 for null terminator
      int new_len = strlen(a->data.v_string) + strlen(b->data.v_string) + 1;
      char* tmp = calloc(sizeof(char), new_len);

      strcat(tmp, a->data.v_string);
      strcat(tmp, b->data.v_string);

      snek_object_t* obj = new_snek_string(tmp);
      free(tmp);
      return obj;

    case VECTOR3:
      if (b->kind != VECTOR3) {
        return NULL;
      }

      return new_snek_vector3(
          snek_add(a->data.v_vector3.x, b->data.v_vector3.x),
          snek_add(a->data.v_vector3.y, b->data.v_vector3.y),
          snek_add(a->data.v_vector3.z, b->data.v_vector3.z));
    case ARRAY:
      if (b->kind != ARRAY) {
        return NULL;
      }
      size_t a_len = a->data.v_array.size;
      size_t b_len = b->data.v_array.size;

      snek_object_t* arr = new_snek_array(a_len + b_len);

      for (size_t i = 0; i < a->data.v_array.size; ++i) {
        snek_array_set(arr, i, snek_array_get(a, i));
      }

      for (size_t i = 0; i < b->data.v_array.size; ++i) {
        snek_array_set(arr, a_len + i, snek_array_get(b, i));
      }

      return arr;

    default:
      return NULL;
  }
}

// don't touch below this line

int snek_length(snek_object_t* obj) {
  if (obj == NULL) {
    return -1;
  }

  switch (obj->kind) {
    case INTEGER:
      return 1;
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

static MunitResult test_integer_add(const MunitParameter params[], void* data) {
  snek_object_t* one = new_snek_integer(1);
  snek_object_t* three = new_snek_integer(3);
  snek_object_t* four = snek_add(one, three);

  munit_assert_not_null(four);
  munit_assert_int(four->kind, ==, INTEGER);
  munit_assert_int(four->data.v_int, ==, 4);

  free(one);
  free(three);
  free(four);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_float_add(const MunitParameter params[], void* data) {
  snek_object_t* one = new_snek_float(1.5);
  snek_object_t* three = new_snek_float(3.5);
  snek_object_t* five = snek_add(one, three);

  munit_assert_not_null(five);
  munit_assert_int(five->kind, ==, FLOAT);
  munit_assert_double(five->data.v_float, ==, 5.0);

  free(one);
  free(three);
  free(five);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_string_add(const MunitParameter params[], void* data) {
  snek_object_t* hello = new_snek_string("hello");
  snek_object_t* world = new_snek_string(", world");
  snek_object_t* greeting = snek_add(hello, world);

  munit_assert_not_null(greeting);
  munit_assert_int(greeting->kind, ==, STRING);
  munit_assert_string_equal(greeting->data.v_string, "hello, world");

  free(hello->data.v_string);
  free(hello);
  free(world->data.v_string);
  free(world);
  free(greeting->data.v_string);
  free(greeting);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_string_add_self(const MunitParameter params[],
                                        void* data) {
  snek_object_t* repeated = new_snek_string("(repeated)");
  snek_object_t* result = snek_add(repeated, repeated);

  munit_assert_not_null(result);
  munit_assert_int(result->kind, ==, STRING);
  munit_assert_string_equal(result->data.v_string, "(repeated)(repeated)");

  free(repeated->data.v_string);
  free(repeated);
  free(result->data.v_string);
  free(result);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_vector3_add(const MunitParameter params[], void* data) {
  snek_object_t* one = new_snek_float(1.0);
  snek_object_t* two = new_snek_float(2.0);
  snek_object_t* three = new_snek_float(3.0);
  snek_object_t* four = new_snek_float(4.0);
  snek_object_t* five = new_snek_float(5.0);
  snek_object_t* six = new_snek_float(6.0);

  snek_object_t* v1 = new_snek_vector3(one, two, three);
  snek_object_t* v2 = new_snek_vector3(four, five, six);
  snek_object_t* result = snek_add(v1, v2);

  munit_assert_not_null(result);
  munit_assert_int(result->kind, ==, VECTOR3);
  munit_assert_double(result->data.v_vector3.x->data.v_float, ==, 5.0);
  munit_assert_double(result->data.v_vector3.y->data.v_float, ==, 7.0);
  munit_assert_double(result->data.v_vector3.z->data.v_float, ==, 9.0);

  free(v1->data.v_vector3.x);
  free(v1->data.v_vector3.y);
  free(v1->data.v_vector3.z);
  free(v1);
  free(v2->data.v_vector3.x);
  free(v2->data.v_vector3.y);
  free(v2->data.v_vector3.z);
  free(v2);
  free(result->data.v_vector3.x);
  free(result->data.v_vector3.y);
  free(result->data.v_vector3.z);
  free(result);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_array_add(const MunitParameter params[], void* data) {
  snek_object_t* one = new_snek_integer(1);
  snek_object_t* ones = new_snek_array(2);
  munit_assert_true(snek_array_set(ones, 0, one));
  munit_assert_true(snek_array_set(ones, 1, one));

  snek_object_t* hi = new_snek_string("hi");
  snek_object_t* hellos = new_snek_array(3);
  munit_assert_true(snek_array_set(hellos, 0, hi));
  munit_assert_true(snek_array_set(hellos, 1, hi));
  munit_assert_true(snek_array_set(hellos, 2, hi));

  snek_object_t* result = snek_add(ones, hellos);
  munit_assert_not_null(result);
  munit_assert_int(result->kind, ==, ARRAY);

  snek_object_t* first = snek_array_get(result, 0);
  munit_assert_not_null(first);
  munit_assert_int(first->data.v_int, ==, 1);

  snek_object_t* third = snek_array_get(result, 2);
  munit_assert_not_null(third);
  munit_assert_string_equal(third->data.v_string, "hi");

  free(one);
  free(ones->data.v_array.elements);
  free(ones);
  free(hi->data.v_string);
  free(hi);
  free(hellos->data.v_array.elements);
  free(hellos);
  free(result->data.v_array.elements);
  free(result);
  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/integer", test_integer_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/float", test_float_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/string", test_string_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/string-repeated", test_string_add_self, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/vector3", test_vector3_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/array", test_array_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-add", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
