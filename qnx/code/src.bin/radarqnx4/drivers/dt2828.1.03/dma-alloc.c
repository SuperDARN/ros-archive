/* dma-alloc.c
   ===========
   Author:QNX
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
 $Log: dma-alloc.c,v $
 Revision 1.1  2004/05/10 14:32:20  barnes
 Initial revision

*/

/*-

SYNOPSIS
alloc_dmabuf, free_dmabuf -- access memory for direct
              memory access devices.

DESCRIPTION

This module provides two routines, which encapsulate
mechanisms for allocating and addressing memory regions
which are appropriate for use with direct memory access
devices.

alloc_dmabuf requires two arguments, a number of bytes, and
a pointer to a dma_t.   The dma_t should not be modified nor
accessed directly by calling routines.  The useful members
are exported through the manifests PADDR, VADDR for Physical
ADDRess and Virtual ADDRess respectively.

The actual amount of dma memory allocated may be more than
nbytes, but applications should not rely upon this.

free_dmabuf removes the mapping and allocation of memory for
the dma region.   After calling free_dmabuf, the value of
the dma_t structure is undefined.

*/

#if defined __386__
#define MMAP_BASED 1
#define PHYSICAL_MEMORY  "Physical"
#else
#define SEGMENT_BASED 1
#endif

#include <stdio.h>
#include <stdlib.h>

#include <limits.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/kernel.h>
#include <sys/proc_msg.h>
#include <sys/seginfo.h>

#if defined MMAP_BASED

#include <fcntl.h>
#include <sys/mman.h>

#endif

#include "dma-alloc.h"

int
   alloc_dmabuf(dma_t *p, long int nbytes)
{
  struct     _seginfo   sbuf;
  int   selector;
#if defined MMAP_BASED
  int   fd;
#endif

	if (nbytes >= USHRT_MAX) {
    	return -1;
    }
    if ((selector = qnx_segment_alloc_flags(nbytes, _PMF_DMA_SAFE)) == -1) {
        return -1;
    }
    if (qnx_segment_info(PROC_PID, 0, selector, &sbuf) == -1) {
        qnx_segment_free(selector);
        return -1;
    }
    P_ADDR(p)  = sbuf.addr;
    p->segno = selector;
    p->seglen = nbytes;
	
#if defined MMAP_BASED
    if ((fd=shm_open(PHYSICAL_MEMORY,O_RDWR,0)) == -1) {
            qnx_segment_free(selector);
            return -1;
    }
    V_ADDR(p) = mmap(0,nbytes, PROT_READ|PROT_WRITE, MAP_SHARED,
                     fd, P_ADDR(p));
    close(fd);
    if (P_ADDR(p) == (paddr_t)-1) {
            qnx_segment_free(selector);
            return -1;
    }
#elif defined SEGMENT_BASED
     V_ADDR(p) = MK_FP(selector, 0);
#else
#error "must configure" __FILE__ "for your implementation"
#endif

     return 0;
	 
}

int
free_dmabuf(dma_t *dp)
{
#if defined MMAP_BASED
     munmap(V_ADDR(dp), dp->seglen);
     qnx_segment_free(dp->segno);
#elif defined SEGMENT_BASED
     qnx_segment_free(FP_SEG(V_ADDR(dp)));
#else
#error "must configure " __FILE__ " for your implementation"
#endif
     return 0;
}









































