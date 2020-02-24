/* msg_io.c
   ========
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

/* 
 $Log: msg_io.c,v $
 Revision 2.3  2001/06/27 20:56:29  barnes
 Added license tag

 Revision 2.2  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 2.1  1997/12/18 13:25:55  root
 New version of the Server task using pipes and non-blocking sockets.

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int msg_err=0;

int write_msg(int fildes,char *buffer,unsigned int size) {
  int byte;
  int cnt=0;
  int dat_sze=0;

  struct {
     unsigned char msg_id[4];
     unsigned char length[4];
  } msg={{0x49,0x08,0x1e,0x00}, {0,0,0,0}};
 
  /* force little endian alignment */
  msg.length[0]=size & 0xff;
  msg.length[1]=(size & 0xff00) >>8;
  msg.length[2]=(size & 0xff0000) >>16;
  msg.length[3]=(size & 0xff000000) >> 24;
  
  while (cnt<8) { 
    byte=write(fildes,((char*) &msg)+cnt,8-cnt);
    if (byte<=0) break;
    cnt+=byte;
  }
  if (cnt<8) return -1;

  cnt=0;  /* write out the message */
  while (cnt<size) {
    byte=write(fildes,buffer+cnt,size-cnt); 
    if (byte<=0) break;
    cnt+=byte;
  }
  return size;
}  

