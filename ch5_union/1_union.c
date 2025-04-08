#include <stdio.h>
#include "../munit/munit.h"

typedef enum SnekObjectKind { INTEGER, STRING } snek_object_kind_t;

typedef union SnekObjectData {
  int v_int;
  char* v_string;
} snek_object_data_t;

typedef struct SnekObject {
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

snek_object_t new_integer(int i) {
  return (snek_object_t){.kind = INTEGER, .data = {.v_int = i}};
}

snek_object_t new_string(char* str) {
  return (snek_object_t){.kind = STRING, .data = {.v_string = str}};
}
void format_object(snek_object_t obj, char* buffer) {
  switch (obj.kind) {
    case INTEGER:
      sprintf(buffer, "int:%d", obj.data.v_int);
      break;
    case STRING:
      sprintf(buffer, "string:%s", obj.data.v_string);
      break;
  }
}

static MunitResult test_formats_int1(const MunitParameter params[],
                                     void* data) {
  char buffer[100];
  snek_object_t i = new_integer(5);
  format_object(i, buffer);

  munit_assert_string_equal("int:5", buffer);  // formats INTEGER
  return MUNIT_OK;
}

static MunitResult test_formats_string1(const MunitParameter params[],
                                        void* data) {
  char buffer[100];
  snek_object_t s = new_string("Hello!");
  format_object(s, buffer);

  munit_assert_string_equal("string:Hello!", buffer);  // formats STRING
  return MUNIT_OK;
}

static MunitResult test_formats_int2(const MunitParameter params[],
                                     void* data) {
  char buffer[100];
  snek_object_t i = new_integer(2014);
  format_object(i, buffer);

  munit_assert_string_equal("int:2014", buffer);  // formats INTEGER
  return MUNIT_OK;
}

static MunitResult test_formats_string2(const MunitParameter params[],
                                        void* data) {
  char buffer[100];
  snek_object_t s = new_string("nvim btw");
  format_object(s, buffer);

  munit_assert_string_equal("string:nvim btw", buffer);  // formats STRING
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/integer", test_formats_int2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/string", test_formats_string2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/integer_nvim", test_formats_int1, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/string_nvim", test_formats_string1, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/format", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
