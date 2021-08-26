/* errlog.c
   ======== */

/*
 $Log: errlog.c,v $
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
#include "print_info.h"
#include "task_name.h"
#include "hlpstr.h"

#define ERRLOG_PRIORITY 19

#define ERROR 'e'
#define OK 'o'


char errlog_name[32]={TASK_NAME};

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
  char  day_num[4];
  char mode[10];
  time_t tclock;
  struct tm *gmt;

  tclock=time(NULL);
  gmt = gmtime(&tclock);
				
  if ((gmt->tm_yday + 1) != yday)	{
     strcpy (name, getenv("SD_ERRLOG_PATH"));
	 strcat (name, "/errlog.");
	 sprintf(day_num, "%3.3d",(gmt->tm_yday + 1));
	 day_num[3]=0;
	 strcat (name, day_num);	/* create name of new log file */
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
  		
  if (argc>1) {
   if (strcmp(argv[1],"--help")==0) {
     print_info(stdout,hlpstr);
     exit(0);
   }
   strcpy(errlog_name,argv[1]);
  }

  if ((ntid = qnx_name_attach(0, errlog_name)) == -1) {
    perror ("Unable to attach name 'ERRLOG'\n");
	exit (errno);
  }
 
  fprintf(stderr,"errlog started under the name: %s\n",errlog_name);

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
