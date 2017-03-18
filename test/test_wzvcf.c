
/**
   gcc wzvcf.c test/test_wzvcf.c -I. -I ../klib/  -L ../klib/kstring.c -lz -g
   valgrind --leak-check=full ./a.out
 */
#include "wzvcf.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  vcf_file_t *vcf = init_vcf_file("/secondary/projects/shen/projects/2015_07_24_biscuit_align/HumanBrainCpH/pileup_hg38/humanBrainCpH.vcf.gz");

  index_vcf_samples(vcf, "ALL");

  vcf_record_t *rec = init_vcf_record();
  int i = 0;
  char *value = 0; char **values = 0; int nvalues = 0;
  while (vcf_read_record(vcf, rec)) {
    printf("%s, %d, %s\n", target_name(vcf->targets, rec->tid), rec->pos, rec->id);
    value = get_vcf_record_info("CX", rec->info);
    get_vcf_record_fmt("GT", rec->fmt, vcf, &values, &nvalues);
    if (value) {
      printf("INFO value: %s\n", value);
    }
    if (values) {
      printf("FORMAT value: %s\n", values[0]);
    }
    free(value);
    free_fields(values, nvalues);
    ++i;
    if (i > 10) break;
  }
  free_vcf_record(rec);
  free_vcf_file(vcf);
  
  return 0;
}
