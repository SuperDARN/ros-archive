/* tcpipmsg.h
   ==========
   Author: J.Spaleta & R.J.Barnes
*/

/*
 $License$
*/


#ifndef _TCPIPMSG_H
#define _TCPIPMSG_H

#define UNKNOWN_HOST 1
#define OPEN_FAIL 2
#define CONNECT_FAIL 3

extern int TCPIPMsgErr;

struct TCPIPMsgHost {
  char host[256];
  int port;
  int sock;
};


int TCPIPMsgOpen(char *hostip, int port);
int  TCPIPMsgSend(int fd,void  *buf,size_t buflen);
int  TCPIPMsgRecv(int fd,void *buf,size_t buflen);

#endif
