#include <stddef.h>
#include <stdlib.h>
#include <string.h>
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
void stack_push_multiple_types(stack_t* s) {
  float* f_ptr = malloc(sizeof(float));
  if (!f_ptr) {
    return;
  }
  *f_ptr = 3.14;
  stack_push(s, f_ptr);

  char* c_ptr = malloc(sizeof(char) * 29);
  if (!c_ptr) {
    return;
  }
  strcpy(c_ptr, "Sneklang is blazingly slow!");
  stack_push(s, c_ptr);
}
//-----------------------------------------------------------------------------
static MunitResult multiple_types_stack(const MunitParameter params[],
                                        void* data) {
  stack_t* s = stack_new(4);
  munit_assert_not_null(s);

  stack_push_multiple_types(s);
  munit_assert_int(s->count, ==, 2);

  float* f = s->data[0];
  munit_assert_double_equal((double)*f, 3.14, 0);  // Using double for safety

  char* string = s->data[1];
  munit_assert_string_equal(string, "Sneklang is blazingly slow!");

  free(f);
  free(string);
  stack_free(s);
  return MUNIT_OK;
}

static MunitTest tests[] = {
    {"/multiple_types_stack", multiple_types_stack, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

static const MunitSuite suite = {"/snekstack", tests, NULL, 1,
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char* argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
