/* task_reset.c
   ============ */

/* 
 $Log: task_reset.c,v $
 Revision 1.2  1999/04/10 20:56:09  barnes
 Added help message system.

 Revision 1.1  1998/06/11 15:34:38  root
 Initial revision

 */

/* sends a close file message to a specified task */

#include <stdlib.h>
#include <stdio.h>
#include "print_info.h"
#include "message.h"
#include "task_write.h"
#include "errstr.h"
#include "hlpstr.h"

void main(int argc,char *argv[]) {

  struct task_id *tsk;

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


  if (task_reset(tsk) !=0) {
    fprintf(stderr,"Error sending message to task %s.\n",argv[1]);
    exit(-1);
  }
  fprintf(stderr,"Sent reset message to task %s.\n",argv[1]);
  exit(0);
}
