/* msgmem.c
   =========
   Author: R.J.Barnes
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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <sys/uio.h>
#include <errno.h>

#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>


#include <fcntl.h>

#include "socket.h"
#include "connect.h"

/*
 $Log: msgmem.c,v $
 Revision 1.2  2004/06/22 12:56:20  barnes
 QNX4 Code audit.

 Revision 1.1  2004/05/10 15:36:07  barnes
 Initial revision

*/

extern struct client client[CLIENT_MAX];
extern int msgmax;

int writeraw(char *buf,int sze) { 
  /* this bit adds a message onto the end of the write buffer */
  int i;
  for (i=0;i<msgmax;i++) 
    if (client[i].sock !=0) {
      if (sze+client[i].out_sze > BUF_SIZE) {
        int delta;      
        delta=sze+client[i].out_sze-BUF_SIZE;
        memmove(client[i].out_buf,client[i].out_buf+delta,
                client[i].out_sze-delta);
        client[i].out_sze=BUF_SIZE-sze;
      } 
      memcpy(client[i].out_buf+client[i].out_sze,buf,sze);
      client[i].out_sze+=sze;
    }
    return 0;
}

void readsock(fd_set *fdset,char *tmp_buf,int tmp_sze) {
  /* reads in a message from a client */         
  int i;
  int byte;

  for (i=0;i<msgmax;i++) 
    if ( (client[i].sock !=-1) && 
         (FD_ISSET(client[i].sock,fdset))) {
         byte=read(client[i].sock,tmp_buf,tmp_sze);
         if (byte ==0) closesock(i);
         else if ((byte <0) && (errno !=EAGAIN)) closesock(i);
         else if (byte > 0) {
           if (byte+client[i].in_sze > BUF_SIZE) {
             int delta;
             delta=byte+client[i].in_sze-BUF_SIZE;
             memmove(client[i].in_buf,client[i].in_buf+delta,
                   client[i].in_sze-delta);
             client[i].out_sze=BUF_SIZE-byte;
           } 
           memcpy(client[i].in_buf+client[i].in_sze,tmp_buf,byte);
           client[i].in_sze+=byte;
	 }
    }
}

int writesock() {
  int i;
  int byte,cnt=0;
  for (i=0;i<msgmax;i++) 
    if ((client[i].sock !=-1) && (client[i].out_sze !=0)) {   
      byte=write(client[i].sock,client[i].out_buf,client[i].out_sze);
   
      if (byte <0) {
        if (errno==EAGAIN) continue;
        else closesock(i);
      } else if (byte >0) {
        memmove(client[i].out_buf,client[i].out_buf+byte,
                client[i].out_sze-byte);
        client[i].out_sze-=byte;
        if (client[i].out_sze >0) cnt++; 
      }
    }
    return cnt; /* returns number of clients with outstanding data */
}




