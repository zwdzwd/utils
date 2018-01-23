/**
 * VCF parser
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

#ifndef _WZVCF_H
#define _WZVCF_H

#include "wzio.h"
#include "wztarget.h"

/*********************
 ** VCF parser *******
 *********************
  
Usage:
  vcf_file_t *vcf = init_vcf_file(vcf_file_path);

  index_vcf_samples(vcf, "SAMPLE1,SAMPLE2");

  vcf_record_t *rec = init_vcf_record();
  char *info_cx; char **fmt_gt; int n_fmt_gt;
  while (vcf_read_record(vcf, rec)) {
    info_cx = get_vcf_record_info("CX", rec->info);
    get_vcf_record_fmt("GT", rec->fmt, vcf, &fmt_gt, &n_fmt_gt);
    free(info_cx);
    free_char_array(fmt_gt, n_fmt_gt);
  }
  free_vcf_record(rec);
  free_vcf_file(vcf);

**/

/****************
 ** INFO Field **
 ****************/
typedef struct vcf_record_info_t {
  int n;
  char **keys;
  char **values;
} vcf_record_info_t;

void free_vcf_record_info(vcf_record_info_t *info);

/******************
 ** FORMAT Field **
 ******************/
typedef struct vcf_record_fmt_t {
  int nfields;
  int nsamples;
  char **field_names;
  char **field_values;  /* nfields X nsamples */
} vcf_record_fmt_t;

void free_vcf_record_fmt(vcf_record_fmt_t *fmt);

/********************
 ** One VCF Record **
 ********************/
typedef struct vcf_record_t {
  int tid;
  int pos;
  char *id;
  char *ref;
  char *alt;
  char *qual;
  char *filter;
  vcf_record_info_t *info;
  vcf_record_fmt_t *fmt;
} vcf_record_t;

vcf_record_t *init_vcf_record();
void free_vcf_record(vcf_record_t *rec);

/***************
 ** VCF File ***
 ***************/
typedef struct vcf_file_t {
  target_v *targets;            /* chromosome names */
  int nsamples;
  char **samples;
  char *file_path;
  gzFile fh;
  char *line;                   /* current line */
  /* target samples, selected by index_vcf_samples */
  int *tsample_indices;         /* indices in *samples */
  int n_tsamples;
} vcf_file_t;

void free_vcf_file(vcf_file_t *vcf);

/* Initialize vcf file for input, read header. */
vcf_file_t *init_vcf_file(char *vcf_file_path);

/***************************
 ** Select Target Samples **
 ***************************/

/* From a sample list to column indices, which are stored in vcf->tsample_indices */
void index_vcf_samples(vcf_file_t *v, char *sample_str);

/*********************
 ** Read One Record **
 *********************/

/* Read one record from the VCF file. */
int vcf_read_record(vcf_file_t *vcf, vcf_record_t *rec);

/* Read line from vcf, returns 1 if success 0 if reaching EOF. */
int vcf_read_line(vcf_file_t *v);

/************************
 ** Get INFO and FORMAT *
 ************************/

/* Get value by key from vcf_record_info_t.
   Returns NULL if key is not found, allocate memory */
char *get_vcf_record_info(const char *key, vcf_record_info_t *info);

/* get FORMAT for a key
   returns values from vcf->tsamples, all the samples if sample_indices == NULL.
   returns NULL if field is not found. */
int get_vcf_record_fmt(const char *key, vcf_record_fmt_t *fmt, vcf_file_t *vcf, char ***ans, int *n_ans);

#endif /* _WZVCF_H */
