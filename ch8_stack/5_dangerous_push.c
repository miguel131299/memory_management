#include <stdlib.h>
// #include "bootlib.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "../munit/munit.h"
//-----------------------------------------------------------------------------

typedef struct Stack {
  size_t count;
  size_t capacity;
  void** data;
} stack_t;

stack_t* stack_new(size_t capacity);
void stack_push(stack_t* stack, void* obj);
void* stack_pop(stack_t* stack);
void stack_free(stack_t* stack);

void stack_free(stack_t* stack) {
  if (stack == NULL) {
    return;
  }

  if (stack->data != NULL) {
    free(stack->data);
  }

  free(stack);
}

void* stack_pop(stack_t* stack) {
  if (stack->count == 0) {
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
//-----------------------------------------------------------------------------
void scary_double_push(stack_t* s) {
  stack_push(s, (void*)1337);
  int* ptr = malloc(sizeof(int));
  if (!ptr) {
    return;
  }
  *ptr = 1024;
  stack_push(s, ptr);
}
//-----------------------------------------------------------------------------
static MunitResult heterogenous_stack(const MunitParameter params[],
                                      void* data) {
  stack_t* s = stack_new(2);
  munit_assert_not_null(s);

  scary_double_push(s);

  munit_assert_int(s->count, ==, 2);

  int value = (int)(intptr_t)s->data[0];  // Cast with intptr_t for portability
  munit_assert_int(value, ==, 1337);

  int* pointer = s->data[1];
  munit_assert_int(*pointer, ==, 1024);

  free(pointer);
  stack_free(s);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/heterogenous_stack", heterogenous_stack, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/snekstack", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
