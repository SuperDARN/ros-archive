/* msg_io.c
   ======== */

/* 
 $Log: msg_io.c,v $
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

