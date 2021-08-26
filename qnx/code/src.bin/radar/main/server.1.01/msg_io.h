/* msg_io.h
   ======== */

#define MSG_ID 0x001e0849
#define MSGERR_PIPE_CLOSED 0x00
#define MSGERR_OUT_OF_SYNC 0x01
#define MSGERR_NOMEM 0x02

extern int msg_err;

int write_msg(int fildes,char *buffer,unsigned int size);
int read_msg(int fildes,char **buffer);
