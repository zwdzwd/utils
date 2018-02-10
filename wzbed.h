/**
 * Bed file parser
 * The MIT License (MIT)
 *
 * Copyright (c) 2016,2017 Wanding.Zhou@vai.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#ifndef _WZBED_H
#define _WZBED_H

#include <inttypes.h>
#include "wztarget.h"
#include "wzio.h"

/* This file defines several bed file parsers */

/*************************************
 ** General-purpose bed file parser **
 *************************************
 bed1_v *beds = bed_read(file_path);
**/

/****************
 ** Bed Record **
 ****************/
typedef struct bed1_t {
  int tid;
  int64_t beg;
  int64_t end;
  void *data;
} bed1_t;

DEFINE_VECTOR(bed1_v, bed1_t)

typedef void (*init_data_f)(bed1_t *b, void *aux_data);
typedef void (*parse_data_f)(bed1_t *b, char **fields, int nfields);
typedef void (*free_data_f)(void *data);

static inline bed1_t *init_bed1(init_data_f init_data, void *aux_data) {
  bed1_t *b = calloc(1, sizeof(bed1_t));
  if (init_data != NULL)
    init_data(b, aux_data);
  return b;
}

static inline void free_bed1(bed1_t *b, free_data_f free_data) {
  if (free_data != NULL)
    free_data(b->data);
  free(b);
}

/**************
 ** Bed File **
 **************/

typedef struct bed_file_t {
  char *file_path;
  gzFile fh;
  char *line;
  target_v *targets;
} bed_file_t;

static inline bed_file_t *init_bed_file(char *file_path) {
  bed_file_t *bed = calloc(1, sizeof(bed_file_t));
  bed->file_path = strdup(file_path);
  bed->fh = wzopen(bed->file_path);
  bed->targets = init_target_v(2);
  bed->line = NULL;
  return bed;
}

static inline void free_bed_file(bed_file_t *bed) {
  wzclose(bed->fh);
  destroy_target_v(bed->targets);
  free(bed->file_path);
  free(bed->line);
  free(bed);
}

static inline int bed_read1(bed_file_t *bed, bed1_t *b, parse_data_f parse_data) {
  if (bed->fh == NULL) return 0;
  if (gzFile_read_line(bed->fh, &bed->line) == 0) return 0;

  char **fields; int nfields;
  line_get_fields(bed->line, "\t", &fields, &nfields);
  if (nfields < 3)
    wzfatal("[%s:%d] Bed file has fewer than 3 columns.\n", __func__, __LINE__);

  b->tid = locate_or_insert_target_v(bed->targets, fields[0]);

  ensure_number(fields[1]);
  b->beg = atoi(fields[1]);

  ensure_number(fields[2]);
  b->end = atoi(fields[2]);

  if (parse_data != NULL) parse_data(b, fields, nfields);
  else b->data = NULL;
  free_fields(fields, nfields);

  return 1;
}

/* static inline int bed_parse1(char *line, target_v *targets, bed1_t *b, void (*dataparser)(bed1_t*, char**)) { */

/*   char *tok; char **linerest; */
/*   tok=strtok_r(line, "\t", linerest); */
/*   b->tid = locate_or_insert_target_v(targets, tok); */

/*   /\* start *\/ */
/*   tok=strtok(NULL, "\t", linerest); */
/*   ensure_number(tok); */
/*   b->beg = atoi(tok); */

/*   /\* end *\/ */
/*   tok=strtok(NULL, "\t", linerest); */
/*   ensure_number(tok); */
/*   ob->pos = atoi(tok); */

/*   if (dataparser) */
/*     dataparser(b, linerest); */
/*   else */
/*     b->data = NULL; */

/*   return 1; */
/* } */

/* static inline bed1_v *bed_read_all(char *bedfn) { */

/*   target_v *targets = init_target_v(2); */
/*   bed1_v *beds = init_bed_v(2); */

/*   gzFile FH = gzopen(bedfn); */
/*   kstring_t line; */
/*   line.l = line.m = 0; line.s = 0; */
/*   bed1_t *b; */
/*   FILE *fh = open(argv[1],"r"); */
/*   while (1) { */
/*     int c=gzgetc(FH); */
/*     if (c=='\n' || c==EOF) { */
/*       b = next_ref_bed_v(beds); */
/*       bed_parse1(line.s, targets, b, NULL); */
/*       line.l = 0; */
/*       if (c==EOF) { */
/*         break; */
/*       } */
/*     } else { */
/*       kputc(c, &line); */
/*     } */
/*     free(line.s); */
/*   } */
/*   return beds; */
/* } */

/* bed1_v *target2bed(target_v *targets) { */
/*   unsigned i; */
/*   bed1_v *beds = init_bed1_v(2); */
/*   for (i=0; i<targets->size; ++i) { */
/*     target_t *t = ref_target_v(targets, i); */
/*     bed1_t *b = next_ref_bed1_v(beds); */
/*     b->tid = i; */
/*     b->beg = 0; */
/*     b->end = t->len; */
/*     b->data = NULL; */
/*   } */
/*   return beds; */
/* } */

/* static inline void bamregion2bed(bed1_t *bed, target_v *targets, char *str) { */

/*   char *s; */
/* 	int i, l, k, name_end; */

/* 	*ref_id = b->beg = b->end = -1; */
/* 	name_end = l = strlen(str); */
/* 	s = (char*)malloc(l+1); */
/* 	// remove space */
/* 	for (i = k = 0; i < l; ++i) */
/* 		if (!isspace(str[i])) s[k++] = str[i]; */
/* 	s[k] = 0; l = k; */
/* 	// determine the sequence name */
/* 	for (i = l - 1; i >= 0; --i) if (s[i] == ':') break; // look for colon from the end */
/* 	if (i >= 0) name_end = i; */
/* 	if (name_end < l) { // check if this is really the end */
/* 		int n_hyphen = 0; */
/* 		for (i = name_end + 1; i < l; ++i) { */
/* 			if (s[i] == '-') ++n_hyphen; */
/* 			else if (!isdigit(s[i]) && s[i] != ',') break; */
/* 		} */
/* 		if (i < l || n_hyphen > 1) name_end = l; // malformated region string; then take str as the name */
/* 		s[name_end] = 0; */
    
/* 		target_t *t = get_target(targets, s); */
/* 		if (!t) { // cannot find the sequence name */
/* 			t = get_target(targets, str); // try str as the name */
/* 			if (!t) { */
/*         fprintf(stderr, "[%s:%d] fail to determine sequence name.\n", __func__, __LINE__); */
/*         fflush(stderr); */
/* 				free(s); return -1; */
/* 			} else s[name_end] = ':', name_end = l; */
/* 		} */
/* 	} else t = get_target(targets, str); */
/*   if (!t) { */
/*     free(s);  */
/*     return -1; */
/*   } */
/*   b->tid = t->tid; */
/* 	// parse the interval */
/* 	if (name_end < l) { */
/* 		for (i = k = name_end + 1; i < l; ++i) */
/* 			if (s[i] != ',') s[k++] = s[i]; */
/* 		s[k] = 0; */
/* 		b->beg = atoi(s + name_end + 1); */
/* 		for (i = name_end + 1; i != k; ++i) if (s[i] == '-') break; */
/* 		b->end = i < k? atoi(s + i + 1) : 1<<29; */
/* 		if (b->beg > 0) b->beg--; */
/* 	} else b->beg = 0, b->end = 1<<29; */
/* 	free(s); */
/* 	return b->beg <= b->end? 0 : -1; */
/* } */

/* static inline bed1_v *bamregion2bedlist(target_v *targets, char *region) { */
/*   bed1_v *beds = init_bed1_v(2); */
/*   bed1_t *b = next_ref_bed1_v(beds); */
/*   if (bam_region2bed(b, targets, region) < 0) { */
/*     fprintf(stderr, "[%s:%d] failed to parse region\n", __func__, __LINE__); */
/*     fflush(stderr); */
/*     exit(1); */
/*   } */
/*   return beds; */
/* } */

#endif /* _WZBED_H */
