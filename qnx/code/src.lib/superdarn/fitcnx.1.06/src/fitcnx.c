/* fitcnx.c
   =========
   Author: R.J.Barnes
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


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#ifdef _QNX4 
#include <sys/select.h>
#endif
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "rtypes.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "connex.h"
#include "decodemsg.h"

/*
 $Log: fitcnx.c,v $
 Revision 1.3  2004/05/04 18:27:58  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/30 22:09:13  barnes
 Changed macro definition.

 Revision 1.1  2004/01/26 01:28:55  barnes
 Initial revision

*/

int FitCnxRead(int num,int *sock,struct RadarParm *prm,
	       struct FitData *fit,int *flag,
	       struct timeval *tmout) {
  fd_set read_set;
  unsigned char *buffer=NULL;
  int status=0,size=0,i=0;
  
  FD_ZERO(&read_set);

  for (i=0;i<num;i++) {
    if (sock[i] !=-1) FD_SET(sock[i],&read_set);
    flag[i]=0;
  }

  if ((status=select(FD_SETSIZE,&read_set,0,0,tmout)) < 0) { 
     perror("while waiting client/master I/O");
     return -1;
  }

  if (status==0) return 0;

  for (i=0;i<num;i++) {
    if ((sock[i] !=-1) && (FD_ISSET(sock[i],&read_set))) {
      size=ConnexReadIP(sock[i],&buffer);
      if (size==-1) return flag[i]=-1;
      if (buffer !=NULL) {
        flag[i]=FitCnxDecodeIPMsg(&prm[i],&fit[i],buffer,size);
        free(buffer);
      }
    } 
  }    
  return status;
}
   

 













