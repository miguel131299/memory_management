#include <stdio.h>
#include <stdlib.h>

int* allocate_scalar_list(int size, int multiplier) {
  int* lst = (int*)malloc(size * sizeof(int));
  if (lst == NULL) {
    return NULL;
  }
  for (int i = 0; i < size; i++) {
    lst[i] = i * multiplier;
  }
  return lst;
}

int main() {
  const int num_lists = 500;
  for (int i = 0; i < num_lists; i++) {
    int* lst = allocate_scalar_list(5000000, 2);
    if (lst == NULL) {
      printf("Failed to allocate list\n");
      return 1;
    } else {
      printf("Allocated list %d\n", i);
      // free was missing:
      free(lst);
    }
  }
  return 0;
}
