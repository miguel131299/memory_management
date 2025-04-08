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
  bool is_marked;
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
  if (!vm || !obj) {
    return;
  }

  stack_push(vm->objects, obj);
}

void frame_reference_object(frame_t* frame, snek_object_t* obj) {
  if (!frame || !obj) {
    return;
  }
  stack_push(frame->references, obj);
}

void mark(vm_t* vm) {
  for (size_t i = 0; i < vm->frames->count; ++i) {
    frame_t* frame = vm->frames->data[i];

    for (size_t j = 0; j < frame->references->count; ++j) {
      snek_object_t* obj = frame->references->data[j];
      obj->is_marked = true;
    }
  }
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

bool snek_array_set(snek_object_t* array, size_t index, snek_object_t* value) {
  if (array == NULL || value == NULL) {
    return false;
  }

  if (array->kind != ARRAY) {
    return false;
  }

  if (index >= array->data.v_array.size) {
    return false;
  }

  // No need to change refcounts, we will find the garbage values
  // later through mark-and-sweep.

  // Set the value directly now (already checked size constraint)
  array->data.v_array.elements[index] = value;
  return true;
}

snek_object_t* snek_array_get(snek_object_t* array, size_t index) {
  if (array == NULL) {
    return NULL;
  }

  if (array->kind != ARRAY) {
    return NULL;
  }

  if (index >= array->data.v_array.size) {
    return NULL;
  }

  // Set the value directly now (already checked size constraint)
  return array->data.v_array.elements[index];
}

void trace_mark_object(stack_t* gray_objects, snek_object_t* obj);

void trace_blacken_object(stack_t* gray_objects, snek_object_t* obj) {
  if (!gray_objects || !obj) {
    return;
  }

  switch (obj->kind) {
    case INTEGER:
    case FLOAT:
    case STRING:
      return;
    case VECTOR3:
      trace_mark_object(gray_objects, obj->data.v_vector3.x);
      trace_mark_object(gray_objects, obj->data.v_vector3.y);
      trace_mark_object(gray_objects, obj->data.v_vector3.z);
      return;
    case ARRAY:
      snek_array_t arr = obj->data.v_array;
      for (size_t i = 0; i < arr.size; i++) {
        trace_mark_object(gray_objects, snek_array_get(obj, i));
      }
      return;
    default:
      return;
  }
}

void trace(vm_t* vm) {
  if (!vm) {
    return;
  }

  stack_t* gray_objects = stack_new(8);
  if (!gray_objects) {
    return;
  }

  for (size_t i = 0; i < vm->objects->count; ++i) {
    snek_object_t* obj = vm->objects->data[i];
    if (obj && obj->is_marked) {
      stack_push(gray_objects, obj);
    }
  }

  while (gray_objects->count) {
    snek_object_t* obj = stack_pop(gray_objects);
    trace_blacken_object(gray_objects, obj);
  }

  stack_free(gray_objects);
}

void trace_mark_object(stack_t* gray_objects, snek_object_t* obj) {
  if (!obj || obj->is_marked) {
    return;
  }

  obj->is_marked = true;
  stack_push(gray_objects, obj);
}
//-----------------------------------------------------------------------------
//---------------------------------- Tests-------------------------------------
static MunitResult test_trace_vector(const MunitParameter params[],
                                     void* data) {
  vm_t* vm = vm_new();
  frame_t* frame = vm_new_frame(vm);

  snek_object_t* x = new_snek_integer(vm, 5);
  snek_object_t* y = new_snek_integer(vm, 5);
  snek_object_t* z = new_snek_integer(vm, 5);
  snek_object_t* vector = new_snek_vector3(vm, x, y, z);

  munit_assert_false(x->is_marked);
  munit_assert_false(y->is_marked);
  munit_assert_false(z->is_marked);
  munit_assert_false(vector->is_marked);

  frame_reference_object(frame, vector);
  mark(vm);
  munit_assert_true(vector->is_marked);
  munit_assert_false(x->is_marked);
  munit_assert_false(y->is_marked);
  munit_assert_false(z->is_marked);

  trace(vm);
  munit_assert_true(vector->is_marked);
  munit_assert_true(x->is_marked);
  munit_assert_true(y->is_marked);
  munit_assert_true(z->is_marked);

  vm_free(vm);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_trace_array(const MunitParameter params[], void* data) {
  vm_t* vm = vm_new();
  frame_t* frame = vm_new_frame(vm);

  snek_object_t* devs = new_snek_array(vm, 2);
  snek_object_t* lane = new_snek_string(vm, "Lane");
  snek_object_t* teej = new_snek_string(vm, "Teej");
  snek_array_set(devs, 0, lane);
  snek_array_set(devs, 1, teej);

  munit_assert_false(devs->is_marked);
  munit_assert_false(lane->is_marked);
  munit_assert_false(teej->is_marked);

  frame_reference_object(frame, devs);
  mark(vm);
  munit_assert_true(devs->is_marked);
  munit_assert_false(lane->is_marked);
  munit_assert_false(teej->is_marked);

  trace(vm);
  munit_assert_true(devs->is_marked);
  munit_assert_true(lane->is_marked);
  munit_assert_true(teej->is_marked);

  vm_free(vm);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_trace_nested(const MunitParameter params[],
                                     void* data) {
  vm_t* vm = vm_new();
  frame_t* frame = vm_new_frame(vm);

  snek_object_t* bootdevs = new_snek_array(vm, 2);
  snek_object_t* lane = new_snek_string(vm, "Lane");
  snek_object_t* hunter = new_snek_string(vm, "Hunter");
  snek_array_set(bootdevs, 0, lane);
  snek_array_set(bootdevs, 1, hunter);

  snek_object_t* terminaldevs = new_snek_array(vm, 4);
  snek_object_t* prime = new_snek_string(vm, "Prime");
  snek_object_t* teej = new_snek_string(vm, "Teej");
  snek_object_t* dax = new_snek_string(vm, "Dax");
  snek_object_t* adam = new_snek_string(vm, "Adam");
  snek_array_set(terminaldevs, 0, prime);
  snek_array_set(terminaldevs, 1, teej);
  snek_array_set(terminaldevs, 2, dax);
  snek_array_set(terminaldevs, 3, adam);

  snek_object_t* alldevs = new_snek_array(vm, 2);
  snek_array_set(alldevs, 0, bootdevs);
  snek_array_set(alldevs, 1, terminaldevs);

  frame_reference_object(frame, alldevs);
  mark(vm);
  trace(vm);

  munit_assert_true(bootdevs->is_marked);
  munit_assert_true(lane->is_marked);
  munit_assert_true(hunter->is_marked);
  munit_assert_true(terminaldevs->is_marked);
  munit_assert_true(prime->is_marked);
  munit_assert_true(teej->is_marked);
  munit_assert_true(dax->is_marked);
  munit_assert_true(adam->is_marked);
  munit_assert_true(alldevs->is_marked);

  vm_free(vm);
  // munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

int main() {
  MunitTest tests[] = {{"/test_trace_vector", test_trace_vector, NULL, NULL,
                        MUNIT_TEST_OPTION_NONE, NULL},
                       {"/test_trace_array", test_trace_array, NULL, NULL,
                        MUNIT_TEST_OPTION_NONE, NULL},
                       {"/test_trace_nested", test_trace_nested, NULL, NULL,
                        MUNIT_TEST_OPTION_NONE, NULL},
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
