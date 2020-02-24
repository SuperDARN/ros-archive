/* task_reset.c
   ============
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
 $Log: task_reset.c,v $
 Revision 1.4  2001/06/27 20:56:37  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:12:08  barnes
 Added Author Name

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
