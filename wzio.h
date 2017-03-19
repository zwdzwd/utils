#include <zlib.h>
#include "wstring.h"
#include "wzmisc.h"

/**********************************
 ** Open file and error handling **
 **********************************/
static inline gzFile wzopen(char *file_path) {
  gzFile fh;
  if (strcmp(file_path, "-") == 0) {
    fh = gzdopen(fileno(stdin), "r");
  } else {
    fh = gzopen(file_path, "r");
    if (!fh) {
      fprintf(stderr, "[%s:%d] Fatal, cannot open file: %s\n", __func__, __LINE__, file_path);
      fflush(stderr);
      exit(1);
    }
  }
  return fh;
}

/*****************************
 ** Read one line from file **
 *****************************
 returns 1 if hitting \n 0 if EOF */
static inline int gzFile_read_line(gzFile fh, wstring_t *s) {

  if (s == NULL) {
    fprintf(stderr, "[%s:%s] Fatal, empty string.\n", __func__, __LINE__);
    fflush(stderr);
    exit(1);
  }
  
  /* reset s */
  clear_string(s);

  /* read until '\n' or EOF */
  while (1) {
    int c = gzgetc(fh);
    if (c == '\n') return 1;
    if (c == EOF) return 0;
    putchar_string(s, c);
  }
  return 0;                     /* should not come here */
}

/****************************
 ** Get one field by index **
 ****************************
 field_index is 0-based
 result creates a new allocated object,
 return 0 if there are not enough fields, 1 if success */
static inline int line_get_field(const char *line, int field_index, const char *sep, char **field) {

  char *working = calloc(strlen(line) + 1, sizeof(char));
  strcpy(working, line);
  char *tok;

  tok = strtok(working, sep);
  int i;
  for (i=0; i<field_index; ++i)
    tok = strtok(NULL, sep);

  if (tok == NULL) {            /* not enough fields */
    free(working);
    return 0;
  }

  *field = strdup(tok);
  free(working);
  return 1;
}

/********************************
 ** Get all fields of one line **
 ********************************
Usage:
   char **fields; int nfields;
   line_get_fields("my line", " ", &fields, &nfields);
   free_fields(fields, nfields);

   Note: separators/delimiters are not merged - the most likely use-case. */
#define free_fields(fds, nfds) free_char_array(fds, nfds)
static inline void line_get_fields(const char *line, const char *sep, char ***fields, int *nfields) {

  *nfields = 1;
  const char *s = line;
  while ((s = strpbrk(s, sep)) != NULL) { (*nfields)++; s++; }

  *fields = calloc(*nfields, sizeof(char *));
  char *working = calloc(strlen(line) + 1, sizeof(char));
  strcpy(working, line);
  char *tok; int i;

  tok = strtok(working, sep);
  for (i=0; tok != NULL; ++i) {
    (*fields)[i] = strdup(tok);
    tok = strtok(NULL, sep);
  }
  free(working);
}
