/* transmit.c
   ========== */


/*
 $Log: transmit.c,v $
 Revision 2.1  1997/12/18 13:25:55  root
 New version of the Server task using pipes and non-blocking sockets.

 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

 
void transmit(int fd,char *out_buf,int length) { 
  int status;
  status=write_msg(fd,out_buf,length); 
}  