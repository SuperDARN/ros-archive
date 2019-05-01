/* dma-addr.c
   ==========
   Author: QNX 
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
 $Log: dma-addr.c,v $
 Revision 1.3  2004/06/16 22:05:14  barnes
 Removed redundant code.

 Revision 1.2  2004/05/03 16:04:37  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/01/12 18:27:30  barnes
 Initial revision

*/

#ifdef _QNX4

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

/*
 $Log: dma-addr.c,v $
 Revision 1.3  2004/06/16 22:05:14  barnes
 Removed redundant code.

 Revision 1.2  2004/05/03 16:04:37  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/01/12 18:27:30  barnes
 Initial revision

*/

int dmaaddr(dma_t *p,pid_t task_id) {
  struct     _seginfo   sbuf;
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

#endif




































