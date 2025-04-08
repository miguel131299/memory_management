#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"
// #include "bootlib.h"

typedef enum SnekObjectKind { INTEGER, FLOAT, STRING } snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  float v_float;
  char* v_string;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t* new_snek_integer(int value);
snek_object_t* new_snek_float(float value);

snek_object_t* new_snek_string(char* value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  int len = strlen(value);
  char* ptr = malloc(len * sizeof(char) + 1);
  if (!ptr) {
    return NULL;
  }

  strcpy(ptr, value);

  obj->kind = STRING;
  obj->data.v_string = ptr;
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

snek_object_t* new_snek_float(float value) {
  snek_object_t* obj = malloc(sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = FLOAT;
  obj->data.v_float = value;
  return obj;
}

static MunitResult test_str_copied(const MunitParameter params[], void* data) {
  const char* input = "Hello, World!";
  snek_object_t* obj = new_snek_string(input);

  munit_assert_int(obj->kind, ==, STRING);

  // Should not have the same pointer (must copy the string)
  munit_assert_ptr_not_equal(obj->data.v_string, input);

  // But contents should be equal
  munit_assert_string_equal(obj->data.v_string, input);

  // Memory should include space for null terminator (14 bytes string + 1 = 15)
  // munit_assert_int(boot_alloc_size(), ==, 22);

  // Free string and object memory
  free(obj->data.v_string);
  free(obj);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/copies_value", test_str_copied, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/object-string", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
