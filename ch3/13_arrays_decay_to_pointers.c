#include <stdio.h>

void core_utils_func(int core_utilization[]) {
  printf("sizeof core_utilization in core_utils_func: %zu\n", sizeof(core_utilization));
}

// don't touch below this line

int main() {
  int core_utilization[] = {43, 67, 89, 92, 71, 43, 56, 12};
  int len = sizeof(core_utilization) / sizeof(core_utilization[0]);
  printf("sizeof core_utilization in main: %zu\n", sizeof(core_utilization));
  printf("len of core_utilization: %d\n", len);
  core_utils_func(core_utilization);
  return 0;
}
