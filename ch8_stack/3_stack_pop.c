#include <stdlib.h>
#include <string.h>
#include "../munit/munit.h"
// #include "bootlib.h"

typedef struct Stack {
  size_t count;
  size_t capacity;
  void** data;
} stack_t;

stack_t* stack_new(size_t capacity);
void stack_push(stack_t* stack, void* obj);
void* stack_pop(stack_t* stack);

void* stack_pop(stack_t* stack) {
  if (!stack->count) {
    return NULL;
  }

  stack->count--;
  return stack->data[stack->count];
}

void stack_push(stack_t* stack, void* obj) {
  if (stack->count == stack->capacity) {
    stack->capacity *= 2;
    void** temp = realloc(stack->data, stack->capacity * sizeof(void*));
    if (temp == NULL) {
      stack->capacity /= 2;

      exit(1);
    }
    stack->data = temp;
  }
  stack->data[stack->count] = obj;
  stack->count++;
  return;
}

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

static MunitResult create_stack(const MunitParameter params[], void* data) {
  stack_t* s = stack_new(10);
  munit_assert_int(s->capacity, ==, 10);
  munit_assert_int(s->count, ==, 0);
  munit_assert_not_null(s->data);

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult push_stack(const MunitParameter params[], void* data) {
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

  stack_push(s, &a);

  munit_assert_int(s->capacity, ==, 4);
  munit_assert_int(s->count, ==, 3);

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult pop_stack(const MunitParameter params[], void* data) {
  stack_t* s = stack_new(2);
  munit_assert_not_null(s);

  munit_assert_int(s->capacity, ==, 2);
  munit_assert_int(s->count, ==, 0);
  munit_assert_not_null(s->data);

  int one = 1;
  int two = 2;
  int three = 3;

  stack_push(s, &one);
  stack_push(s, &two);
  stack_push(s, &three);

  munit_assert_int(s->capacity, ==, 4);
  munit_assert_int(s->count, ==, 3);

  int* popped = stack_pop(s);
  munit_assert_int(*popped, ==, three);

  popped = stack_pop(s);
  munit_assert_int(*popped, ==, two);

  popped = stack_pop(s);
  munit_assert_int(*popped, ==, one);

  popped = stack_pop(s);
  munit_assert_null(popped);

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult pop_stack_empty(const MunitParameter params[], void* data) {
  stack_t* s = stack_new(2);
  munit_assert_not_null(s);

  munit_assert_int(s->capacity, ==, 2);
  munit_assert_int(s->count, ==, 0);
  munit_assert_not_null(s->data);

  int* popped = stack_pop(s);
  munit_assert_null(popped);

  free(s->data);
  free(s);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/create_stack", create_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/push_stack", push_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/pop_stack", pop_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/pop_stack_empty", pop_stack_empty, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/snekstack", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
