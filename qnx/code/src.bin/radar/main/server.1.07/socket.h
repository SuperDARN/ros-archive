/* socket.h
   ========
   Author: R.J.Barnes
*/

#define CLIENT_MAX 16
#define BUF_SIZE 10*1024

struct client {
  int sock;
  int in_sze;
  int out_sze;
  char host[256];
  char in_buf[BUF_SIZE]; /* data from client */
  char out_buf[BUF_SIZE]; /* data to client */
};

