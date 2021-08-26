/*-
 * alloc_dmabuf, free_dmabuf - access dma buffers.
 */

#ifndef _dma_alloc_h
#define _dma_alloc_h

#ifndef __TYPES_H_INCLUDED
#include <sys/types.h>
#endif

#if defined __cplusplus
extern "C" {
#endif

typedef struct {
  long int             segno;     /* allocated segment number */
  long int             seglen;    /* length of segment */
  paddr_t         phys_addr; /* physical address */
  caddr_t         bufp; /* logical address */
}               dma_t;

extern int      alloc_dmabuf(dma_t * dbuf, long int nbytes);
extern int      free_dmabuf(dma_t * dbuf);

#define P_ADDR(d)  ((d)->phys_addr)
#define V_ADDR(d)  ((d)->bufp)


#if defined __cplusplus
}
#endif

#endif
