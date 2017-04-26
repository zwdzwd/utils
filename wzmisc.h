#ifndef _WZMISC_H
#define _WZMISC_H

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

/* utility function for freeing character array */
static inline void free_char_array(char **char_array, int n) {
  int i;
  for (i=0; i<n; ++i)
    free(char_array[i]);
  free(char_array);
}

/******************************************
 * same as strdup, but free/reuse original
 * Caution! src and dest must not overlap 
 * since realloc will release src and make 
 * its content undefined
 *******************************************/
static inline char *strcpy_realloc(char *dest, char *src) {
  dest = realloc(dest, strlen(src) + 1);
  strcpy(dest, src);
  return dest;
}

/* Exit with message */
static inline void wzfatal(const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end (args);
  fflush(stderr);
  exit(1);
}

static inline int is_number(char *s) {
  int i;
  for (i=0;s[i];++i) {
    if (!isdigit(s[i]) && s[i]!='.') {
      return 0;
    }
  }
  return 1;
}

static inline void ensure_number(char *s) {
  int i;
  for (i=0;s[i];++i) {
    if (!isdigit(s[i]) && s[i]!='.') {
      fprintf(stderr, "[%s:%d] Trying to convert nondigit string to number: %s\n", __func__, __LINE__, s);
      fflush(stderr);
      exit(1);
    }
  }
}

static inline int strcount_char(char *s, char c) {
  int i, n=0;
  for (i=0; s[i]; ++i)
    if (s[i] == c)
      ++n;
  return n;
}

#endif /* _WZMISC_H */
