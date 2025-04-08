// #include "exercise.h"

void concat_strings(char *str1, const char *str2) {
  char *i1 = str1;
  char *i2 = str2;

  // find null terminator of str1
  while (*i1) {
    i1++;
  }

  // copy over str2
  while(*i2) {
    *i1 = *i2;
    i1++;
    i2++;
  }

  *i1 = 0;
}
