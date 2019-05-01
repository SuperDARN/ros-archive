/* dma-alloc.h
   ===========
   Author: QNX
*/

/*-
 * alloc_dmabuf, free_dmabuf - access dma buffers.
 */

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

/*
 $Log: dma-alloc.h,v $
 Revision 1.1  2004/07/12 22:36:01  barnes
 Initial revision

*/



#ifndef _dma_alloc_h
#define _dma_alloc_h
#ifdef _QNX4
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

#endif
