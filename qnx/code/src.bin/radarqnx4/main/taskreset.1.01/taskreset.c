/* taskreset.c
   ===========
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include "rtypes.h"
#include "option.h"
#include "rmsg.h"
#include "rmsgsnd.h"
#include "taskid.h"
#include "errstr.h"
#include "hlpstr.h"

/*
 $Log: taskreset.c,v $
 Revision 1.1  2004/05/10 15:30:45  barnes
 Initial revision

*/

int arg=0;
struct OptionData opt;


void main(int argc,char *argv[]) {
  unsigned char help=0;
  struct TaskID *tsk;

  OptionAdd(&opt,"-help",'x',&help);
  
  if (argc>1) { 
    arg=OptionProcess(1,argc,argv,&opt,NULL);   
  } else {
    OptionPrintInfo(stderr,errstr);
    exit(-1);
  }

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  tsk=TaskIDMake(argv[1]);
  if (tsk==NULL) {
    fprintf(stderr,"Could not locate task %s.\n",argv[1]);
    exit(-1);
  }  


  if (RMsgSndReset(tsk) !=0) {
    fprintf(stderr,"Error sending message to task %s.\n",argv[1]);
    exit(-1);
  }
  fprintf(stderr,"Sent reset message to task %s.\n",argv[1]);
  exit(0);
}
