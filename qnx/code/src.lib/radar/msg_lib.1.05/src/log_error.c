/* log_error.c
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


/*
 $Log: log_error.c,v $
 Revision 1.6  2001/06/27 20:53:53  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:50  barnes
 Added Author Name

 Revision 1.4  2000/10/30 16:45:49  barnes
 Fixed bug in return type.

 Revision 1.3  1998/10/28 22:34:20  barnes
 Fixed mis-typed variable.

 Revision 1.2  1998/10/22 13:05:18  barnes
 Changed all message types to long integer.

 Revision 1.1  1998/06/19 10:48:42  root
 Initial revision

 */

#include <sys/timers.h>
#include <sys/kernel.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "message.h"

#define TIME_OUT 10.0
#define OK 'o'
#define ERROR 'e'

char *str_time() {
  char *str;
  time_t clock;
  struct tm *gmt;

  time(&clock);
  gmt = gmtime(&clock); 
  str = asctime(gmt);
  str[strlen(str)-1] = 0; /* get rid of new line */
  return str;
}

int log_error(struct task_id *ptr,char *name,char *buffer) {
  int status;
  char tname[32];
  int size; 
  char msg=ERROR;
 
  void *b_out[4];
  void *b_in[2];
  
  unsigned s_out[3];
  unsigned s_in;

  if (buffer == NULL) return -1;
  size=strlen(buffer)+1;
  
  if (name !=NULL) strncpy(tname,name,32);
  else sprintf(tname,"(unknown)");

  b_out[0]=&tname;
  b_out[1]=&size;
  b_out[2]=buffer;  

  b_in[0]=&msg;
  b_in[1]=NULL;
  
  s_out[0]=sizeof(tname);
  s_out[1]=sizeof(int);
  s_out[2]=size;

  s_in=sizeof(char);

  status = message_tsk_array(ptr,0,b_out,b_in,s_out,&s_in);

  if ((msg != OK) || (status !=0)) 
    fprintf(stderr,"WARNING: Error not logged\n");

  fprintf(stderr,"%s: %d : %s :%s\n",str_time(),getpid(),tname,buffer);
  return status;
}






