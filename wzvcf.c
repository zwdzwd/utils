#include "wzvcf.h"

/****************
 ** INFO Field **
 ****************/

void free_vcf_record_info(vcf_record_info_t *info) {
  if (info == NULL) return;
  free_fields(info->keys, info->n);
  free_fields(info->values, info->n);
  free(info);
}

char *get_vcf_record_info(const char *key, vcf_record_info_t *info) {
  int i;
  for (i=0; i<info->n; ++i) {
    if (strcmp(info->keys[i], key) == 0) {
      return strdup(info->values[i]);
    }
  }
  return NULL;
}

vcf_record_info_t* line_parse_vcf_info(char *info_str) {

  char **subfields; int nsubfields;
  line_get_fields(info_str, ";", &subfields, &nsubfields);
  int i;
  char *tok;
  vcf_record_info_t *info = calloc(1, sizeof(vcf_record_info_t));
  info->n = nsubfields;
  info->keys = calloc(info->n, sizeof(char*));
  info->values = calloc(info->n, sizeof(char*));
  for (i=0; i<nsubfields; ++i) {
    tok = strtok(subfields[i], "=");
    info->keys[i] = strdup(tok);
    tok = strtok(NULL, "=");
    info->values[i] = strdup(tok);
  }
  free_fields(subfields, nsubfields);
  return info;
}

/******************
 ** FORMAT Field **
 ******************/
void free_vcf_record_fmt(vcf_record_fmt_t *fmt) {
  if (fmt == NULL) return;
  int i;
  for (i=0; i<fmt->nfields; ++i) {
    free(fmt->field_names[i]);
  }
  free(fmt->field_names);

  for (i=0; i<fmt->nfields*fmt->nsamples; ++i) {
    free(fmt->field_values[i]);
  }
  free(fmt->field_values);
  free(fmt);
}

int get_vcf_record_fmt(const char *key, vcf_record_fmt_t *fmt, vcf_file_t *vcf, char ***ans, int *n_ans) {

  /* locate field */
  int field_index = -1; int i;
  for (i=0; i<fmt->nfields; ++i) {
    if (strcmp(fmt->field_names[i], key) == 0) {
      field_index = i;
      break;
    }
  }
  if (field_index < 0) {
    *ans = NULL;
    *n_ans = 0;
    return 0;
  }

  if (vcf->tsample_indices == NULL) {
    *n_ans = vcf->nsamples;
    (*ans) = calloc(vcf->nsamples, sizeof(char*));
    for (i=0; i<vcf->nsamples; ++i) {
      (*ans)[i] = strdup(fmt->field_values[i*fmt->nsamples + field_index]);
    }
  } else {
    *n_ans = vcf->n_tsamples;
    (*ans) = calloc(vcf->n_tsamples, sizeof(char*));
    for (i=0; i<vcf->n_tsamples; ++i) {
      (*ans)[i] = strdup(fmt->field_values[vcf->tsample_indices[i]*vcf->n_tsamples + field_index]);
    }
  }
  return 1;
}

vcf_record_fmt_t* line_parse_vcf_fmt(char **fields, int nfields) {
  vcf_record_fmt_t *fmt = calloc(1, sizeof(vcf_record_fmt_t));
  line_get_fields(fields[8], ":", &fmt->field_names, &fmt->nfields);
  fmt->nsamples = nfields - 9;
  fmt->field_values = calloc(fmt->nfields * fmt->nsamples, sizeof(char*));
  int i;
  for (i=9; i<nfields; ++i) {  /* loop over samples */
    char **subfields; int nsubfields;
    line_get_fields(fields[i], ":", &subfields, &nsubfields);
    if (nsubfields != fmt->nfields) {
      fprintf(stderr, "[Warning] Invalid number of fields in FMT, Skip line.\n");
      fflush(stderr);
      free(fmt->field_values);
      free(fmt);
      return NULL;
    }
    memcpy(fmt->field_values + fmt->nfields * (i-9), subfields, fmt->nfields * sizeof(char*));
    free(subfields);
  }
  return fmt;
}

/********************
 ** One VCF Record **
 ********************/

vcf_record_t *init_vcf_record() {
  return calloc(1, sizeof(vcf_record_t));
}

void free_vcf_record(vcf_record_t *rec) {
  free(rec->id);
  free(rec->ref);
  free(rec->alt);
  free(rec->qual);
  free(rec->filter);
  free_vcf_record_info(rec->info);
  free_vcf_record_fmt(rec->fmt);
  free(rec);
}

/***************
 ** VCF File ***
 ***************/
vcf_file_t *init_vcf_file(char *vcf_file_path) {

  vcf_file_t *vcf = calloc(1, sizeof(vcf_file_t));
  vcf->targets = init_target_v(2);
  vcf->file_path = strdup(vcf_file_path);
  vcf->fh = wzopen(vcf->file_path);
  vcf->line = init_string(10);
  char *pch;
  while (gzFile_read_line(vcf->fh, vcf->line)) {
    if (!vcf->line) continue;
    if (vcf->line->s[0] == '#' && vcf->line->s[1] == '#') continue;
    if (strncmp(vcf->line->s, "#CHROM", 6) == 0) {

      /* CHROM - FORMAT */
      int i;
      pch = strtok(vcf->line->s, "\t");
      for (i=0; i<9; ++i)
        pch = strtok(NULL, "\t");

      /* SAMPLES */
      while (pch != NULL) {
        vcf->nsamples++;
        vcf->samples = realloc(vcf->samples, sizeof(char*)*vcf->nsamples);
        vcf->samples[vcf->nsamples-1] = strdup(pch);
        pch = strtok(NULL, "\t");
      }
      break;
    }
  }
  return vcf;
}

void free_vcf_file(vcf_file_t *vcf) {
  destroy_target_v(vcf->targets);
  free_char_array(vcf->samples, vcf->nsamples);
  free(vcf->file_path);
  gzclose(vcf->fh);
  free_string(vcf->line);
  free(vcf->tsample_indices);
  free(vcf);
}

int vcf_read_line(vcf_file_t *vcf) {
  if (vcf->fh == NULL) return 0;
  if (gzFile_read_line(vcf->fh, vcf->line)) return 1;
  return 0;
}

void index_vcf_samples(vcf_file_t *vcf, char *sample_str) {

  if (!vcf->tsample_indices)
    free(vcf->tsample_indices); /* free previous samples */

  int i, j;
  if (strcmp(sample_str, "ALL")==0) { /* all samples */
    vcf->n_tsamples = vcf->nsamples;
    vcf->tsample_indices = realloc(vcf->tsample_indices, vcf->n_tsamples*sizeof(int));
    for (i=0; i<vcf->n_tsamples; ++i)
      vcf->tsample_indices[i] = i;
  } else if (strcmp(sample_str, "FIRST") == 0) {
    vcf->n_tsamples = 1;
    vcf->tsample_indices = realloc(vcf->tsample_indices, sizeof(int));
    vcf->tsample_indices[0] = 0;
  } else {
    char **tsample_names;
    line_get_fields(sample_str, ",", &tsample_names, &vcf->n_tsamples);
    vcf->tsample_indices = realloc(vcf->tsample_indices, vcf->n_tsamples*sizeof(int));
    for (i=0; i<vcf->n_tsamples; ++i) {
      vcf->tsample_indices[i] = -1;
      for (j=0; j<vcf->nsamples; ++j) {
        if (strcmp(tsample_names[i], vcf->samples[j])==0) {
          vcf->tsample_indices[i] = j;
          break;
        }
      }
      if (vcf->tsample_indices[i] < 0) {
        fprintf(stderr, "[Error] Unknown sample: %s.\n", tsample_names[i]);
        fflush(stderr);
        exit(1);
      }
    }
    free_fields(tsample_names, vcf->n_tsamples);
  }
}

int vcf_read_record(vcf_file_t *vcf, vcf_record_t *rec) {
  if (!vcf_read_line(vcf))
    return 0;

  char **fields; int nfields;
  line_get_fields(vcf->line->s, "\t", &fields, &nfields);
  if (nfields < 8) {
    fprintf(stderr, "[Error] Invalid VCF records: fewer than 8 columns. Exit.\n");
    free_fields(fields, nfields);
    exit(1);
  }

  rec->tid = locate_or_insert_target_v(vcf->targets, fields[0]);
  rec->pos = atoi(fields[1]);
  rec->id = strcpy_realloc(rec->id, fields[2]);
  rec->ref = strcpy_realloc(rec->ref, fields[3]);
  rec->alt = strcpy_realloc(rec->alt, fields[4]);
  rec->qual = strcpy_realloc(rec->qual, fields[5]);
  rec->filter = strcpy_realloc(rec->filter, fields[6]);

  free_vcf_record_info(rec->info);
  rec->info = line_parse_vcf_info(fields[7]);

  free_vcf_record_fmt(rec->fmt);
  rec->fmt = NULL;
  if (nfields >= 10)
    rec->fmt = line_parse_vcf_fmt(fields, nfields);

  free_fields(fields, nfields);
  return 1;
}
