#include <stdlib.h>

/* utility function for freeing character array */
static inline void free_char_array(char **char_array, int n) {
  int i;
  for (i=0; i<n; ++i)
    free(char_array[i]);
  free(char_array);
}

/* same as strdup, but free/reuse original */
static inline char *strcpy_realloc(char *dest, char *src) {
  dest = realloc(dest, strlen(src) + 1);
  strcpy(dest, src);
  return dest;
}
