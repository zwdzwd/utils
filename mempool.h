/**************************
 * A memory pool of reads *
 **************************

 * Similar to the ones in samtools.
 * Note memory pool doesn't keep track
 * of allocation, it only keep track of recycles.
 * 
 * This setting slightly improve the performance
 * of memory allocation.
 **************************/

typedef struct __mempool_t {
  int cnt;                     /* number of prima facie allocations */
  int n;                       /* number of recycles */
  int max;                     /* maximum possible recycles */
  rnode_t **recycle_stack;     /* address of recycled */
} mempool_t;

static mempool_t *mp_init(void) {
  mempool_t *mp;
  mp = (mempool_t*) calloc(1, sizeof(mempool_t));
  return mp;
}

static void mp_destroy(mempool_t *mp) {

  /* clean recycled nodes */
  int k;
  for (k = 0; k < mp->n; ++k) {
    free(mp->buf[k]->b.data);
    free(mp->buf[k]);
  }
  free(mp->buf);
  free(mp);
}

static inline rnode_t *mp_alloc(mempool_t *mp)
{
    ++mp->cnt;
    if (mp->n == 0) return (rnode_t*)calloc(1, sizeof(rnode_t));
    else return mp->buf[--mp->n];
}

static inline void mp_free(mempool_t *mp, rnode_t *p) {
  --mp->cnt;
  if (mp->n == mp->max) {
    mp->max = mp->max? mp->max<<1 : 256;
    mp->buf = (rnode_t**)realloc(mp->buf, sizeof(rnode_t*) * mp->max);
  }
  mp->buf[mp->n++] = p;
}
