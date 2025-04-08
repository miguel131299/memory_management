#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../munit/munit.h"

typedef struct Stack {
  size_t count;
  size_t capacity;
  void** data;
} stack_t;

void stack_push(stack_t* stack, void* obj) {
  if (stack->count == stack->capacity) {
    // Double stack capacity to avoid reallocing often
    stack->capacity *= 2;
    stack->data = realloc(stack->data, stack->capacity * sizeof(void*));
    if (stack->data == NULL) {
      // Unable to realloc, just exit :) get gud
      exit(1);
    }
  }

  stack->data[stack->count] = obj;
  stack->count++;

  return;
}

void* stack_pop(stack_t* stack) {
  if (stack->count == 0) {
    return NULL;
  }

  stack->count--;
  return stack->data[stack->count];
}

void stack_free(stack_t* stack) {
  if (stack == NULL) {
    return;
  }

  if (stack->data != NULL) {
    free(stack->data);
  }

  free(stack);
}

void stack_remove_nulls(stack_t* stack) {
  size_t new_count = 0;

  // Iterate through the stack and compact non-NULL pointers.
  for (size_t i = 0; i < stack->count; ++i) {
    if (stack->data[i] != NULL) {
      stack->data[new_count++] = stack->data[i];
    }
  }

  // Update the count to reflect the new number of elements.
  stack->count = new_count;

  // Optionally, you might want to zero out the remaining slots.
  for (size_t i = new_count; i < stack->capacity; ++i) {
    stack->data[i] = NULL;
  }
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

typedef struct VirtualMachine {
  stack_t* frames;
  stack_t* objects;
} vm_t;

vm_t* vm_new() {
  vm_t* vm = malloc(sizeof(vm_t));
  if (!vm) {
    return NULL;
  }

  vm->frames = stack_new(8);
  vm->objects = stack_new(8);

  return vm;
}

void vm_free(vm_t* vm) {
  if (!vm) {
    return;
  }

  stack_free(vm->frames);
  stack_free(vm->objects);
  free(vm);
}

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
  snek_object_kind_t kind;
  snek_object_data_t data;
} snek_object_t;

static MunitResult test_vm_new(const MunitParameter params[], void* data) {
  vm_t* vm = vm_new();

  munit_assert_int(vm->frames->capacity, ==, 8);
  munit_assert_int(vm->objects->capacity, ==, 8);

  vm_free(vm);
  return MUNIT_OK;
}

static MunitResult test_vm_new_free(const MunitParameter params[], void* data) {
  vm_t* vm = vm_new();
  vm_free(vm);

  // munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

int main() {
  MunitTest tests[] = {
      {"/vm", test_vm_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"/vm_free", test_vm_new_free, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

  MunitSuite suite = {
      .prefix = "mark-and-sweep",
      .tests = tests,
      .suites = NULL,
      .iterations = 1,
      .options = MUNIT_SUITE_OPTION_NONE,
  };

  return munit_suite_main(&suite, NULL, 0, NULL);
}
