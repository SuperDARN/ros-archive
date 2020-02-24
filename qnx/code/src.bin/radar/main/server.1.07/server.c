/* server.c
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
 $Log: server.c,v $
 Revision 3.13  2002/02/02 23:50:17  barnes
 Modification to deal with multiple channels.

 Revision 3.12  2001/12/17 16:17:49  barnes
 Fixed bug in releasing memory.

 Revision 3.11  2001/09/05 21:06:24  barnes
 Modification to encode XCF elevation angles in the data stream.

 Revision 3.10  2001/06/27 20:56:29  barnes
 Added license tag

 Revision 3.9  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 3.8  2000/09/22 02:27:44  barnes
 Changed command line option to "-ec".

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
struct frame_data xframe;
   
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

  int channel=-1;

  ptr=&tblk;


  if (argc>1) {
       int c;
       for (c=1;c<argc;c++) { 
         if (strcmp(argv[c],"--help")==0) help=1;
         else if (strcmp(argv[c],"-ec")==0) {
           strcpy(echo_name,argv[c+1]);
           c++;
         } else if (strcmp(argv[c],"-e")==0) {
           strcpy(errlog_name,argv[c+1]);
           c++;
         } else if (strcmp(argv[c],"-n")==0) {
           strcpy(prog_name,argv[c+1]);
           c++;
         } else if (strcmp(argv[c],"-cn")==0) {
           if (tolower(argv[c+1][0])=='a') channel=1;
           if (tolower(argv[c+1][0])=='b') channel=2;
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
            if ((channel !=-1) && (fdt->prms.CHANNEL !=0)) {
              if ((channel==1) && (fdt->prms.CHANNEL==2)) break;
              if ((channel==2) && (fdt->prms.CHANNEL==1)) break;
	    }
            buf_len=compress_fit(fdt,&frame);
            if (outpipe ==-1) outpipe=fork_inet(port);
            if (outpipe !=-1) transmit(outpipe,(char *) &frame,buf_len);
            if (fdt->prms.XCF==1) {
              buf_len=compress_xcf(fdt,&xframe);
              if (outpipe ==-1) outpipe=fork_inet(port);
              if (outpipe !=-1) transmit(outpipe,(char *) &xframe,buf_len);
	    }

            break;
          default :
            break;
        }
      }
      if (ptr->store !=NULL) free (ptr->store);
      ptr->store=NULL;
    }
  } while(loop);
}
   

 
