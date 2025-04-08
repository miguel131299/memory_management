#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"

typedef struct Stack {
  size_t count;
  size_t capacity;
  void** data;
} stack_t;

stack_t* stack_new(size_t capacity) {
  stack_t* stack = malloc(sizeof(stack_t));
  if (stack == NULL) {
    return NULL;
  }

  stack->count = 0;
  stack->capacity = capacity;
  stack->data = malloc(stack->capacity * sizeof(void*));
  if (stack->data == NULL) {
    free(stack);
    return NULL;
  }

  return stack;
}

void stack_push(stack_t* stack, void* obj) {
  if (stack->capacity == stack->count) {
    // Could also use realloc
    void* buffer = malloc(2 * stack->capacity * sizeof(void*));
    memcpy(buffer, stack->data, stack->capacity * sizeof(void*));

    stack->capacity = 2 * stack->capacity;
    free(stack->data);
    stack->data = buffer;
  }

  stack->data[stack->count] = obj;
  stack->count++;
}

static MunitResult create_stack(const MunitParameter params[], void* data) {
  stack_t* s = stack_new(10);
  munit_assert_int(s->capacity, ==, 10);  // Sets capacity to 10
  munit_assert_int(s->count, ==, 0);      // No elements yet
  munit_assert_not_null(s->data);         // Allocates stack data

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());  // Should be no memory leaks
  return MUNIT_OK;
}

static MunitResult push_stack(const MunitParameter params[], void* data) {
  stack_t* s = stack_new(2);
  munit_assert_not_null(s);  // Must allocate new stack

  munit_assert_int(s->capacity, ==, 2);
  munit_assert_int(s->count, ==, 0);
  munit_assert_not_null(s->data);

  int a = 1;
  stack_push(s, &a);
  stack_push(s, &a);

  munit_assert_int(s->capacity, ==, 2);    // Still fits in original capacity
  munit_assert_int(s->count, ==, 2);       // 2 elements pushed
  munit_assert_ptr_equal(s->data[0], &a);  // First element is correct

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());  // Should be no memory leaks
  return MUNIT_OK;
}

static MunitResult push_double_capacity(const MunitParameter params[],
                                        void* data) {
  stack_t* s = stack_new(2);
  munit_assert_not_null(s);

  munit_assert_int(s->capacity, ==, 2);
  munit_assert_int(s->count, ==, 0);
  munit_assert_not_null(s->data);

  int a = 1;
  stack_push(s, &a);
  stack_push(s, &a);

  munit_assert_int(s->capacity, ==, 2);
  munit_assert_int(s->count, ==, 2);

  stack_push(s, &a);                     // Triggers doubling
  munit_assert_int(s->capacity, ==, 4);  // Capacity doubled
  munit_assert_int(s->count, ==, 3);     // 3 elements in the stack
  // munit_assert_int(boot_realloc_count(), ==,
  //                  1);  // Exactly one realloc occurred

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/create_stack", create_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/push_stack", push_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/push_double_capacity", push_double_capacity, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/snekstack", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
