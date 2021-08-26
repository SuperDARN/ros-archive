 /* server.c
   ======== */

/*
 $Log: server.c,v $
 Revision 3.7  1999/04/10 20:52:00  barnes
 Added help message system.

 Revision 3.6  1999/03/30 19:28:56  barnes
 Moved task names into a header.

 Revision 3.5  1999/03/19 17:17:15  barnes
 Removed redundant header.

 Revision 3.4  1998/11/09 23:03:03  barnes
 Incorporated rawdata header.

 Revision 3.3  1998/06/11 15:01:23  root
 Moved the usage message into another file.

 Revision 3.2  1998/06/05 11:17:19  root
 Modifications to use the new task_id method of locating a process.

 Revision 3.1  1998/06/05 11:12:41  root
 New version using the improved message passing mechanism.

 Revision 2.3  1997/12/18 15:38:16  root
 Modifications so that the task can start and stop the TCP/IP handler
 and recover from TCP/IP crashes.

 Revision 2.2  1997/12/18 13:52:40  root
 Report error if TCP/IP fails and exit.

 Revision 2.1  1997/12/18 13:25:55  root
 New version of the Server task using pipes and non-blocking sockets.

 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


#include "version.h"
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "message.h"
#include "task_msg.h"
#include "decode_msg.h"
#include "file_io.h"
#include "log_error.h"
#include "echo_util.h"

#include "cmp_fit.h"
#include "socket.h"
#include "task_name.h"
#include "hlpstr.h"


char msg;
struct frame_data frame;
     
char msg;
char errstr[256];
char echo_name[64]={ECHO_DATA};
char errlog_name[64]={ERRLOG};
char prog_name[64]={TASK_NAME};
int outpipe=-1;

struct task_id *tsk;

struct task_decode tblk;
struct task_decode *ptr; 

void trap_pipe(int signal) {
  /* trap the broken pipe signal when we lose the TCP/IP handler */
  if (signal==SIGCHLD) wait(NULL);
  close(outpipe);  
  outpipe=-1;
}

void main(int argc,char *argv[]) {

  int port=0,status=0,i;  
  pid_t cpid;
  pid_t pid;
  int buf_len;
  int loop=1;
  int help=0;

  struct sigaction act;
  sigset_t set;


  struct fitdata *fdt=NULL;

  char echo_pass={FIT_TYPE};
  int task_id;
  char type;
  char rmsg;

  ptr=&tblk;


  if (argc>1) {
       int c;
       for (c=1;c<argc;c++) { 
         if (strcmp(argv[c],"--help")==0) help=1;
         else if (strcmp(argv[c],"-d")==0) {
           strcpy(echo_name,argv[c+1]);
           c++;
         } else if (strcmp(argv[c],"-e")==0) {
           strcpy(errlog_name,argv[c+1]);
           c++;
         } else if (strcmp(argv[c],"-n")==0) {
           strcpy(prog_name,argv[c+1]);
           c++;
         }
       }
       port=atoi(argv[argc-1]);   
   }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  /* attach to echo data */


  tsk=make_task_id(errlog_name);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

 
  sprintf(errstr,"Started (version %s)",VSTRING);
  log_error(tsk,prog_name,errstr);
 

  /* attach the task name */

  if ( (task_id=qnx_name_attach(0,prog_name) ) == -1) { 
    log_error(tsk,prog_name,"Unable to register name.\n");
    exit(EXIT_FAILURE);
  }


  if (echo_register(prog_name,echo_name,&echo_pass,2) !=0) {
    log_error(tsk,prog_name,"Unable to attach to echo_data.");
    exit(EXIT_FAILURE);
   }

  /* start the internet end of things */
  outpipe=fork_inet(port);
  
  sigemptyset(&set);
  act.sa_flags=0;
  act.sa_mask=set;
  act.sa_handler=&trap_pipe;
  sigaction(SIGPIPE,&act,NULL);
  sigaction(SIGCHLD,&act,NULL);

  do {
    pid=Receive(0,&msg,sizeof(char));
    rmsg=decode_msg(pid,msg,ptr);
    Reply(pid,&rmsg,sizeof(char));
    if (msg==TASK_QUIT) {
      loop=0;
      break; 
    } else if (msg==TASK_DATA) {
      for (i=0;i<ptr->block.num;i++) {
        switch (ptr->block.data[i].type) {
          case FIT_TYPE: /* display fitted data */
            fdt=(struct fitdata *) 
                (ptr->block.ptr[ptr->block.data[i].index]);
            buf_len=compress_fit(fdt,&frame);
            if (outpipe ==-1) outpipe=fork_inet(port);
            if (outpipe !=-1) transmit(outpipe,(char *) &frame,buf_len);
            break;
          default :
            break;
        }
      }
      if (ptr->store !=NULL) free (ptr->store);
    }
  } while(loop);
}
   

 