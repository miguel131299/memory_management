#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"

typedef struct Stack {
  size_t count;
  size_t capacity;
  void** data;
} stack_t;

stack_t* stack_new(size_t capacity) {
  printf("Test\n");
  stack_t* stk_ptr = malloc(sizeof(stack_t));
  if (!stk_ptr) {
    return NULL;
  }
  stk_ptr->count = 0;
  stk_ptr->capacity = capacity;
  stk_ptr->data = malloc(capacity * sizeof(void*));
  if (!stk_ptr->data) {
    free(stk_ptr);
    return NULL;
  }
  return stk_ptr;
}

static MunitResult create_stack_small(const MunitParameter params[],
                                      void* data) {
  stack_t* s = stack_new(3);

  munit_assert_int(s->capacity, ==, 3);  // Sets capacity to 3
  munit_assert_int(s->count, ==, 0);     // No elements in the stack yet
  munit_assert_not_null(s->data);        // Allocates the stack data

  free(s->data);
  free(s);

  //   munit_assert_true(boot_all_freed());  // No memory leaks
  return MUNIT_OK;
}

static MunitResult create_stack_large(const MunitParameter params[],
                                      void* data) {
  stack_t* s = stack_new(100);

  munit_assert_int(s->capacity, ==, 100);  // Sets capacity to 100
  munit_assert_int(s->count, ==, 0);       // No elements in the stack yet
  munit_assert_not_null(s->data);          // Allocates the stack data

  free(s->data);
  free(s);

  //   munit_assert_true(boot_all_freed());  // No memory leaks
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/create_stack_small", create_stack_small, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/create_stack_large", create_stack_large, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/snekstack", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
