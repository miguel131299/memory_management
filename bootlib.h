// bootlib.h
#ifndef BOOTLIB_H
#define BOOTLIB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

void* boot_malloc(size_t size);
void boot_free(void* ptr);
bool boot_all_freed(void);

#define malloc boot_malloc
#define free boot_free

typedef struct Node {
  void* ptr;
  struct Node* next;
} Node;

static Node* allocations = NULL;

void* boot_malloc(size_t size) {
  void* ptr = malloc(size);
  if (!ptr)
    return NULL;

  Node* node = malloc(sizeof(Node));
  node->ptr = ptr;
  node->next = allocations;
  allocations = node;

  return ptr;
}

void boot_free(void* ptr) {
  if (!ptr)
    return;

  Node** curr = &allocations;
  while (*curr) {
    if ((*curr)->ptr == ptr) {
      Node* to_free = *curr;
      *curr = (*curr)->next;
      free(to_free);
      break;
    }
    curr = &((*curr)->next);
  }

  free(ptr);
}

bool boot_all_freed(void) {
  return allocations == NULL;
}

#endif
