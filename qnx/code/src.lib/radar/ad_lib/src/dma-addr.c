/* dma-addr.c
   ========== */

/*
 $Log: dma-addr.c,v $
 Revision 1.1  1998/09/08 15:47:29  barnes
 Initial revision

*/

/*-

SYNOPSIS
dma_addr - returns calculates the address of a DMA buffer

DESCRIPTION
  This routine takes a previously defined structure of the type dma_t
  and calculates the correct logical address parameter for the address 
  space of the calling process.
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
/* #include <sys/proc_msg.h> */
#include <sys/seginfo.h>

#if defined MMAP_BASED

#include <fcntl.h>
#include <sys/mman.h>

#endif

#include "dma-alloc.h"

int dmaaddr(dma_t *p,pid_t task_id) {
  struct     _seginfo   sbuf;
  int   selector;
#if defined MMAP_BASED
  int   fd;
#endif

    if (qnx_segment_info(PROC_PID,task_id, p->segno, &sbuf) == -1) return -1;
    P_ADDR(p)  = sbuf.addr;

#if defined MMAP_BASED
    if ((fd=shm_open(PHYSICAL_MEMORY,O_RDWR,0)) == -1) return -1;
   
    V_ADDR(p) = mmap(0,p->seglen, PROT_READ|PROT_WRITE, MAP_SHARED,
                     fd, P_ADDR(p));
 
    close(fd);
    if (P_ADDR(p) == (paddr_t)-1) return -1;
    
#elif defined SEGMENT_BASED
     V_ADDR(p) = MK_FP(selector, 0);
#else
#error "must configure" __FILE__ "for your implementation"
#endif
     return 0;
	 
}






































