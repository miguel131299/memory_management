#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "../munit/munit.h"
//-----------------------------------------------------------------------------
//----------------------------------Snek---------------------------------------
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

void snek_object_free(snek_object_t* obj) {
  switch (obj->kind) {
    case INTEGER:
    case FLOAT:
      break;
    case STRING:
      free(obj->data.v_string);
      break;
    case VECTOR3: {
      break;
    }
    case ARRAY: {
      free(obj->data.v_array.elements);
      break;
    }
  }
  free(obj);
}

//-----------------------------------------------------------------------------
//---------------------------------- Stack-------------------------------------
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
//-----------------------------------------------------------------------------
//----------------------------------VM-----------------------------------------
typedef struct VirtualMachine {
  stack_t* frames;
  stack_t* objects;
} vm_t;

typedef struct StackFrame {
  stack_t* references;
} frame_t;

void vm_frame_push(vm_t* vm, frame_t* frame) {
  stack_push(vm->frames, frame);
}

frame_t* vm_new_frame(vm_t* vm) {
  frame_t* frame = malloc(sizeof(frame_t));
  frame->references = stack_new(8);
  vm_frame_push(vm, frame);
  return frame;
}

void frame_free(frame_t* frame) {
  if (!frame) {
    return;
  }

  stack_free(frame->references);
  free(frame);
}

// don't touch below this line

vm_t* vm_new() {
  vm_t* vm = malloc(sizeof(vm_t));
  if (vm == NULL) {
    return NULL;
  }

  vm->frames = stack_new(8);
  vm->objects = stack_new(8);
  return vm;
}

void vm_free(vm_t* vm) {
  for (int i = 0; i < vm->frames->count; i++) {
    frame_free(vm->frames->data[i]);
  }
  stack_free(vm->frames);
  stack_free(vm->objects);
  free(vm);
}

void vm_track_object(vm_t* vm, snek_object_t* obj) {
  stack_push(vm->objects, obj);
}

void frame_reference_object(frame_t* frame, snek_object_t* obj) {
  stack_push(frame->references, obj);
}

//-----------------------------------------------------------------------------
//-----------------------------------Sneknew-------------------------------------
snek_object_t* _new_snek_object(vm_t* vm) {
  snek_object_t* obj = calloc(1, sizeof(snek_object_t));
  if (obj == NULL) {
    return NULL;
  }
  vm_track_object(vm, obj);
  return obj;
}

// don't touch below this line

snek_object_t* new_snek_array(vm_t* vm, size_t size) {
  snek_object_t* obj = _new_snek_object(vm);
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

snek_object_t* new_snek_vector3(vm_t* vm, snek_object_t* x, snek_object_t* y,
                                snek_object_t* z) {
  if (x == NULL || y == NULL || z == NULL) {
    return NULL;
  }

  snek_object_t* obj = _new_snek_object(vm);
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = VECTOR3;
  obj->data.v_vector3 = (snek_vector_t){.x = x, .y = y, .z = z};

  return obj;
}

snek_object_t* new_snek_integer(vm_t* vm, int value) {
  snek_object_t* obj = _new_snek_object(vm);
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = INTEGER;
  obj->data.v_int = value;

  return obj;
}

snek_object_t* new_snek_float(vm_t* vm, float value) {
  snek_object_t* obj = _new_snek_object(vm);
  if (obj == NULL) {
    return NULL;
  }

  obj->kind = FLOAT;
  obj->data.v_float = value;
  return obj;
}

snek_object_t* new_snek_string(vm_t* vm, char* value) {
  snek_object_t* obj = _new_snek_object(vm);
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
//-----------------------------------------------------------------------------
//---------------------------------- Tests-------------------------------------
static MunitResult test_one_ref(const MunitParameter params[], void* data) {
  vm_t* vm = vm_new();
  frame_t* frame = vm_new_frame(vm);

  snek_object_t* lanes_wpm = new_snek_integer(vm, 9);
  frame_reference_object(frame, lanes_wpm);

  munit_assert_int(frame->references->count, ==, 1);
  munit_assert_ptr_equal(frame->references->data[0], lanes_wpm);

  vm_free(vm);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_multi_ref(const MunitParameter params[], void* data) {
  vm_t* vm = vm_new();
  frame_t* frame = vm_new_frame(vm);

  snek_object_t* lanes_wpm = new_snek_integer(vm, 9);
  snek_object_t* teej_wpm = new_snek_integer(vm, 160);
  frame_reference_object(frame, lanes_wpm);
  frame_reference_object(frame, teej_wpm);

  munit_assert_int(frame->references->count, ==, 2);
  munit_assert_ptr_equal(frame->references->data[0], lanes_wpm);
  munit_assert_ptr_equal(frame->references->data[1], teej_wpm);

  vm_free(vm);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

int main() {
  MunitTest tests[] = {
      {"/test_one_ref", test_one_ref, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
      {"/test_multi_ref", test_multi_ref, NULL, NULL, MUNIT_TEST_OPTION_NONE,
       NULL},
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
