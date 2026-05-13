/* errlog.c
   ========
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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
 $Log: errlog.c,v $
 Revision 2.14  2004/05/10 15:15:02  barnes
 Modifications to deal with the new libraries.

 Revision 2.13  2001/06/27 20:56:03  barnes
 Added license tag

 Revision 2.12  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 2.11  2000/10/30 16:43:44  barnes
 Fixed bug in reporting errors in decoding messages.

 Revision 2.10  1999/04/10 20:43:11  barnes
 Added help message system.

 Revision 2.9  1999/03/30 19:26:53  barnes
 Moved task name into a header.

 Revision 2.8  1998/11/09 23:07:22  barnes
 Fixed problem with variable types.

 Revision 2.7  1998/10/22 13:13:59  barnes
 Changed all integers to type int.

 Revision 2.6  1998/08/18 21:30:45  barnes
 Fixed problem that occured when the error log could not be opened.

 Revision 2.5  1998/06/11 14:51:32  root
 Moved the usage message into a different file.

 Revision 2.4  1998/06/03 19:19:14  root
 Modification to the version control system.

 Revision 2.3  1998/05/27 15:57:05  root
 Changed the registered name to lower case.

 Revision 2.2  1997/05/12 15:09:28  root
 Fixed memory leak.

 Revision 2.1  1997/03/25 18:06:22  root
 New version of error log task.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/name.h>
#include <errno.h>
#include <sys/kernel.h>
#include <sys/sched.h>
#include "option.h"
#include "taskname.h"
#include "hlpstr.h"

#define ERRLOG_PRIORITY 19

#define ERROR 'e'
#define OK 'o'

int arg=0;
struct OptionData opt;

char taskname[32]={TASK_NAME};

char name[256];
int yday=0;

char *get_time() {
  char *str;
  time_t tclock;
  struct tm *gmt;

  tclock=time(NULL);
  gmt = gmtime(&tclock); 
  str = asctime(gmt);
  str[strlen(str)-1] = 0; /* get rid of new line */
  return str;
}

FILE *open_file() {

  FILE *fp=NULL;
  char  daynum[16];
  char mode[10];
  time_t tclock;
  struct tm *gmt;

  tclock=time(NULL);
  gmt = gmtime(&tclock);
				
  if ((gmt->tm_yday + 1) != yday)	{
     strcpy (name, getenv("SD_ERRLOG_PATH"));
	 strcat (name, "/errlog.");
	 sprintf(daynum, "%.4d%.2d%.2d",1900+gmt->tm_year,gmt->tm_mon+1,
                                         gmt->tm_mday);
	 strcat (name, daynum);	/* create name of new log file */
     if (access(name,F_OK) == -1) 
	    strcpy(mode,"w");
	    else strcpy(mode,"a");
        fp = fopen (name, mode);	/* open new file */
		yday = gmt->tm_yday + 1;
        if (fp !=NULL) {
		  fprintf(fp,"%s opened at %s",name,asctime(gmt));
          fprintf(stderr,"%s opened at %s",name,asctime(gmt));
        } 
	  } else {
	    fp = fopen(name,"a");
	  }
   return fp;
}



main (int argc,char *argv[]) {	
  unsigned char help=0;
  int ntid, reply_tid, status;
  FILE *fp;

  char msg;
  char *buffer=NULL; 
  int size;
  char name[32];
  int i;

  if (getenv("SD_ERRLOG_PATH")==NULL) {
     fprintf(stderr,
             "Environment variable SD_ERRLOG_PATH must be defined.\n");
     exit(-1);
  }
  	

  OptionAdd(&opt,"-help",'x',&help);
  

  arg=OptionProcess(1,argc,argv,&opt,NULL);  
  
  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  }

  if (arg<argc) strcpy(taskname,argv[arg]);
  

  if ((ntid = qnx_name_attach(0, taskname)) == -1) {
    perror ("Unable to attach name 'ERRLOG'\n");
	exit (errno);
  }
 
  fprintf(stderr,"errlog started under the name: %s\n",taskname);

  if (setprio (0, ERRLOG_PRIORITY) == -1) {
    perror("Can't change ERRLOG priority");
  }

  /* This is the main loop for the error administrator */
  while (1) {
    reply_tid = Receive(0,&name,sizeof(name));
	status=(Readmsg(reply_tid,sizeof(name),&size,
           sizeof(int)) !=sizeof(int));
    if (status==0) status=(size<=0);
	if (status==0) status=((buffer=malloc(size)) == NULL);
	if (status==0) status=(Readmsg(reply_tid,
	                              sizeof(name)+sizeof(int),
					              buffer,size) !=size);

    /* make sure that the message is null terminated */
    if (status==0) {
      for (i=0;(i<size) && (buffer[i] !=0);i++);
      buffer[i]=0;	         
    }

	fp=open_file();
    if (fp==NULL) fprintf(stderr,"WARNING : Error log not recording\n"); 
   
    if (status==0) {
      if (fp !=NULL) fprintf(fp,"%s : %d : %s :%s\n",get_time(),reply_tid,name,buffer);
      fprintf(stderr,"%s : %d : %s :%s\n",get_time(),reply_tid,name,buffer);
    } else {
      if (fp !=NULL) fprintf(fp,"%s: %s : - :Error decoding message\n",
              get_time(),reply_tid);
      fprintf(stderr,"%s: %s : - :Error decoding message\n",
              get_time(),reply_tid);
    }
    if (buffer !=NULL) {
      free(buffer);
      buffer=0;
    }
	if (status !=0) msg=ERROR;
	else msg=OK;
	Reply(reply_tid,&msg,sizeof(char));
    if ((fp !=NULL) && (fp !=stderr)) fclose(fp);
  }
}
