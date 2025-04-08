#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../munit/munit.h"

char* get_full_greeting(char* greeting, char* name, int size) {
  char* full_greeting = malloc(size);
  snprintf(full_greeting, size, "%s %s", greeting, name);
  return full_greeting;
}

// Helper function to check if a pointer is on the stack
static bool is_on_stack(void* ptr) {
  void* stack_top = __builtin_frame_address(0);
  uintptr_t stack_top_addr = (uintptr_t)stack_top;
  uintptr_t ptr_addr = (uintptr_t)ptr;

  // Check within a small threshold (1KB for safety)
  uintptr_t threshold = 1024;
  return ptr_addr >= (stack_top_addr - threshold) &&
         ptr_addr <= (stack_top_addr + threshold);
}

static MunitResult test_basic_greeting(const MunitParameter params[],
                                       void* data) {
  char* result = get_full_greeting("Hello", "Alice", 20);
  munit_assert_string_equal(result,
                            "Hello Alice");  // Basic greeting should be correct
  munit_assert_false(is_on_stack(result));  // Result should not be on the stack
  free(result);
  return MUNIT_OK;
}

static MunitResult test_short_buffer(const MunitParameter params[],
                                     void* data) {
  char* result = get_full_greeting("Hey", "Bob", 4);
  munit_assert_string_equal(result, "Hey");  // Should truncate to fit buffer
  munit_assert_false(is_on_stack(result));  // Result should not be on the stack
  free(result);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/test_basic_greeting", test_basic_greeting, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_short_buffer", test_short_buffer, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/get_full_greeting", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
