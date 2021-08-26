/* task_kill.c
   =========== */

/* 
 $Log: task_kill.c,v $
 Revision 1.3  1999/04/10 20:54:51  barnes
 Fixe bug in the help option.

 Revision 1.2  1999/04/10 20:54:08  barnes
 Added help message system.

 Revision 1.1  1999/03/25 15:49:24  barnes
 Initial revision

 */

/* sends a quit file message to a specified task */

#include <stdlib.h>
#include <stdio.h>
#include "print_info.h"
#include "message.h"
#include "task_write.h"
#include "errstr.h"
#include "hlpstr.h"

void main(int argc,char *argv[]) {

  struct task_id *tsk;
  int i;
  if (argc !=2) {
    print_info(stderr,errstr);
    exit(-1);
  }

  if (strcmp(argv[1],"--help")==0) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  tsk=make_task_id(argv[1]);
  if (tsk==NULL) {
    fprintf(stderr,"Could not locate task %s.\n",argv[1]);
    exit(-1);
  }  


  if (task_quit(tsk) !=0) {
    fprintf(stderr,"Error sending message to task %s.\n",argv[1]);
    exit(-1);
  }
  fprintf(stderr,"Sent quit message to task %s.\n",argv[1]);
  exit(0);
}
