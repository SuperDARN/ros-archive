/* main.c
   ====== */

/*
 $Log: main.c,v $
 Revision 1.6  1999/04/10 20:48:12  barnes
 Added help message system.

 Revision 1.5  1999/03/30 19:28:00  barnes
 Moved task name into a header.

 Revision 1.4  1998/11/04 20:57:45  barnes
 Added extra informational messages.

 Revision 1.3  1998/11/04 18:40:52  barnes
 Changed the startup text.

 Revision 1.2  1998/11/04 18:36:35  barnes
 Changed method of operation so that we no longer have to wait an
 entire integration period until we enter the shell.

 Revision 1.1  1998/10/28 15:51:50  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <string.h>
#include <sys/sendmx.h>
#include <termios.h>
#include "print_info.h"
#include "task_name.h"
#include "hlpstr.h"

#define SHELL_SEND 's'
#define SHELL_SEND_MSG 'm'
#define SHELL_SEND_ERR 'e'
#define SHELL_REPLY 'r'

extern FILE *input_fp;
extern FILE *output_fp;

char *buffer=NULL;
char control_name[32]={CONTROL_NAME};

void main(int argc,char *argv[]) {
  char msg;
  int wait=1;
  pid_t control_pid;
  int size=0;
  struct _mxfer_entry smx[1];
  struct _mxfer_entry mx[3];
  int status=0;
  int input_fd;

  if (argc>1) {
    if (strcmp(argv[1],"--help")==0) {
      print_info(stdout,hlpstr);
      exit(0);
    }
    strcpy(control_name,argv[1]);
  } 
  if ( (control_pid=qnx_name_locate(0,control_name,0,0) ) == -1) { 
    perror("Unable to locate the control program");
    exit(1);
  }

  fprintf(stdout,"radar_shell\n");
  fprintf(stdout,"===========\n\n");
  fprintf(stdout,"Connecting to:%s\n\n",control_name);

  fprintf(stdout,"Downloading parameters..\n");

  msg=SHELL_SEND;
  _setmx(&smx[0],&msg,sizeof(msg));
  _setmx(&mx[0],&size,sizeof(size));
  status=Sendmx(control_pid,1,1,smx,mx);

  if (status !=0) { 
    fprintf(stderr,"Failed to get parameters.\n");
    exit(1);
  }  

  /* download the parameters */
  
  if (size !=0) {
    buffer=malloc(size);
    if (buffer !=NULL) msg=SHELL_SEND_MSG;
    else msg=SHELL_SEND_ERR;
    _setmx(&smx[0],&msg,sizeof(msg));
    _setmx(&mx[0],buffer,size);
    status=Sendmx(control_pid,1,1,smx,mx);
  } else {
    fprintf(stderr,"No parameters can be accessed.\n");
    exit(0);
  }  
   
      
  /* enter the shell */
  status=shell(buffer);
      
  msg=SHELL_REPLY;
  _setmx(&mx[0],&msg,sizeof(msg));
  _setmx(&mx[1],&size,sizeof(size));
  _setmx(&mx[2],buffer,size);
	
  Sendmx(control_pid,3,0,mx,0);
  if (buffer !=NULL) free(buffer); 

  exit(0);
}