/* shmem.c
   =======
   Author R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>


/*
 $Log: shmem.c,v $
 Revision 1.3  2008/03/14 23:39:18  code
 Added the ability to control unlinking when freeing memory.

 Revision 1.2  2008/03/14 21:26:22  code
 Fixed minor bugs.

 Revision 1.1  2008/03/14 21:24:04  code
 Initial revision

*/



unsigned char *ShMemAlloc(char *memname,int size,int flags,int unlink,
			  int *fdptr) {

  int s;
  char *p;
  int fd;

  if (unlink) {
    s=shm_unlink(memname); /* ignore errors on this */
  }
 
 
  fd=shm_open(memname,flags,0777);
  if (fd==-1) {
    return NULL;
  }

 
  if (flags & O_CREAT) {
    s=ltrunc(fd,size,SEEK_SET);
    if (s==-1) {
      close(fd);
      return NULL;
    }
  }

  p=mmap(0,size,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
  if ((int) p==-1) {
    close(fd);
    return NULL;
  }

  if (fdptr !=NULL) *fdptr=fd;
  return p;

}

int ShMemFree(unsigned char *p,char *memname,int size,int unlink,int fd) {
  int s=0;
  munmap(p,size);
  close(fd);
  if (unlink) s=shm_unlink(memname);
  return s;
}

