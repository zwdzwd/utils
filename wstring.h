/*
 * wstring.h
 * adapted from Ruan Juan's string.h
 * different from the original version
 * the capacity is not necessarily a power of 2
 * removed much irrelevant code
 *
 */
 
#ifndef _WSTRING_H
#define _WSTRING_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#ifndef SWAP_TMP
#define SWAP_TMP
#define swap_tmp(a, b, t) { (t) = (a); (a) = (b); (b) = (t); }
#endif

typedef struct wstring_t {
  char *s;                      /* \0-ended string */
  size_t cap;
} wstring_t;

#define uc(ch) (((ch) >= 'a' && (ch) <= 'z')? (ch) + 'A' - 'a' : (ch))
#define lc(ch) (((ch) >= 'A' && (ch) <= 'Z')? (ch) + 'a' - 'A' : (ch))

static inline wstring_t* init_string(int cap){
  wstring_t *str;
  str = (wstring_t*)malloc(sizeof(wstring_t));
  str->cap = cap>1 ? cap : 2;
  str->s = (char*) malloc(sizeof(char) * (str->cap));
  str->s[0] = 0;
  return str;
}

static inline void encap_string(wstring_t *str, int inc){

  if (strlen(str->s) + 1 + inc > str->cap) {
    while (strlen(str->s) + 1 + inc > str->cap) {
      if (str->cap < 0xFFFFF) {
	str->cap <<= 1;
      } else {
	str->cap += 0xFFFFF;
      }
    }
    str->s = (char*) realloc(str->s, str->cap);
  }
}

static inline void uc_string(wstring_t *str){
  size_t i;
  for(i=0;i<strlen(str->s);i++){
    if(str->s[i] >= 'a' && str->s[i] <= 'z') str->s[i] = str->s[i] + 'A' - 'a';
  }
}

static inline void lc_string(wstring_t *str){
  size_t i;
  for(i=0;i<strlen(str->s);i++){
    if(str->s[i] >= 'A' && str->s[i] <= 'Z') str->s[i] = str->s[i] + 'a' - 'A';
  }
}

static inline char* substr(char *string, size_t start, size_t end, char *dst){
  size_t i, size;
  char *str;
  size = strlen(string);
  if(start > size) start = size;
  if(end > size) end = size;
  if(end < start) size=0;
  else size = end - start;
  if(dst != NULL) str = dst;
  else str = (char*)malloc(sizeof(char) * (size + 1));
  for(i=start;i<end;i++){
    str[i-start] = string[i];
  }
  str[size] = '\0';
  return str;
}

static inline char* catstr(int n_str, ...){
  char *str, *s;
  int i, len;
  va_list params;
	
  len = 0;
  str = NULL;
  va_start(params, n_str);
  for(i=0;i<n_str;i++){
    s = va_arg(params, char*);
    len += strlen(s);
    str = realloc(str, len + 1);
    if(i == 0) str[0] = 0;
    strcat(str, s);
  }
  va_end(params);
  return str;
}

static inline void chomp_string(wstring_t *str){
  if(str->s[0] && str->s[strlen(str->s)] == '\n'){
    str->s[strlen(str->s)] = 0;
  }
}


/* static inline void trim_string(wstring_t *str){ */
/*   int i, j; */
/*   i = strlen(str->string) - 1; */
/*   while(i >= 0 && (str->string[i] == '\n' || str->string[i] == '\t' || str->string[i] == ' ')) i--;  */
/*   i = 0; */
/*   while(i < strlen(str->string) && (str->string[i] == '\n' || str->string[i] == '\t' || str->string[i] == ' ')) i++; */
/*   if(i){			/\* left shift *\/ */
/*     for(j=i;j<str->size;j++){ str->string[j-i] = str->string[j]; } */
/*     str->size -= i; */
/*   } */
/*   str->string[str->size] = 0; */
/* } */

static inline void
append_string(wstring_t *str, const char *src){
  encap_string(str, strlen(src));
  strcat(str->s, src);
}

/* static inline void append_char_string(wstring_t *str, char c, int num){ */
/*   encap_string(str, num); */
/*   while(num-- > 0){ str->string[str->size ++] = c; } */
/*   str->string[str->size] = 0; */
/* } */

static inline wstring_t* as_string(const char *c_str){
  wstring_t *str = init_string(strlen(c_str)+1);
  append_string(str, c_str);
  return str;
}

static inline void putchar_string(wstring_t *str, char ch){
  encap_string(str, 1);
  int len = strlen(str->s);
  str->s[len] = ch;
  str->s[len+1] = 0;
}

static inline void
clear_string(wstring_t *str){
  str->s[0] = 0;
}

static inline void reverse_string(wstring_t *str){
  int i, j;
  char c;
  i = 0;
  j = strlen(str->s) - 1;
  while(i < j){
    swap_tmp(str->s[i], str->s[j], c);
    i++;
    j--;
  }
}

static inline void reverse_str(char *str, int len){
  int i, j;
  char c;
  i = 0;
  j = len - 1;
  while(i < j){
    swap_tmp(str[i], str[j], c);
    i++;
    j--;
  }
}

/* static inline void tidy_string(wstring_t *src, wstring_t *dst, char ch){ */
/*   int i; */
/*   encap_string(dst, src->size); */
/*   for(i=0;i<src->size;i++){ */
/*     if(src->string[i] != ch){ */
/*       dst->string[dst->size ++] = src->string[i]; */
/*     } */
/*   } */
/*   dst->string[dst->size] = 0; */
/* } */

static inline int occ_str(char *str, int len, char c){
  int i, ret;
  for(i=ret=0;i<len;i++){
    if(str[i] == c) ret ++;
  }
  return ret;
}

static inline void trunc_string(wstring_t *str, size_t size){
  if(size >= strlen(str->s)) return;
  str->s[size] = 0;
}

static inline wstring_t* clone_string(wstring_t *str){
  wstring_t *clone;
  clone = init_string(strlen(str->s)+1);
  append_string(clone, str->s);
  return clone;
}

static inline char* clone_native_string(wstring_t *str) {
  char *clone = malloc(strlen(str->s)+1);
  strcpy(clone, str->s);
  return clone;
}

static inline void free_string(wstring_t *str){ free(str->s); free(str); }

static inline void extend_string(wstring_t *str, char *source, int start, int end) {
  encap_string(str, end - start + 1);
  strncat(str->s, source + start, end - start + 1);
}

#endif
