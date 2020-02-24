/* load_schedule.c
   ===============
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
 $Log: load_schedule.c,v $
 Revision 1.7  2001/06/27 20:56:24  barnes
 Added license tag

 Revision 1.6  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 1.5  2000/09/18 20:45:40  barnes
 Added code so that the schedule is reloaded the moment it is altered.

 Revision 1.4  1999/11/08 15:15:21  barnes
 Fixed bug that meant that leading spaces were not being stripped out of
 the command line.

 Revision 1.3  1999/09/13 15:19:49  barnes
 Fixed bug in getting command line arguments for default program.

 Revision 1.2  1999/03/30 18:45:20  barnes
 Fixed problem of trailing spaces in the schedule.

 Revision 1.1  1999/03/30 18:29:38  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "cnv_time.h"
#include "schedule.h"
#include "log_info.h"

int parse_schedule_line(char *line,struct scd_blk *ptr) {
 
  char *token,i;
  int year,month,day,hour,minute;
  if ((token=strtok(line,DELIM))==NULL) return -1; /* year/default */
  
  if (strcmp(token,"default")==0) {
    /* default entry */
    if ((token=strtok(NULL,""))==NULL) return -1; /* command */
    strcpy(ptr->entry[ptr->num].command,token);
    ptr->entry[ptr->num].stime=-1;
	ptr->num++;
    return 1;
  } else if (strcmp(token,"path")==0) {
    /* the path variable */
    if ((token=strtok(NULL,""))==NULL) return -1; /* command */
    strcpy(ptr->path,token);
  } else {
    /* extract time data */
	year=atoi(token);
    if ((token=strtok(NULL,DELIM))==NULL) return -1; /* month */
	month=atoi(token);
    if ((token=strtok(NULL,DELIM))==NULL) return -1; /* day */
    day=atoi(token);
    if ((token=strtok(NULL,DELIM))==NULL) return -1; /* hour */
	hour=atoi(token);
    if ((token=strtok(NULL,DELIM))==NULL) return -1; /* minute */
	minute=atoi(token);
    ptr->entry[ptr->num].stime=time_epoch(year,month,day,hour,minute,0);
    if (ptr->entry[ptr->num].stime==-1) return -1;
    if ((token=strtok(NULL,""))==NULL) return -1; /* command */
        
    /* strip leading spaces from command */
    for (i=0;(token[i] !=0) && ((token[i]==' ') || (token[i]=='\t'));i++); 
    if (token[i]==0) return -1;   
    strcpy(ptr->entry[ptr->num].command,token+i);
    ptr->num++;
  }
  return 0;	
}
				 
int cmp_scd(void *a,void *b) {
  struct scd_entry *as;
  struct scd_entry *ab;

   as=(struct scd_entry *) a;
   ab=(struct scd_entry *) b;
  
   if (as->stime<ab->stime) return -1;
   if (as->stime>ab->stime) return 1;
   return 0;
}

int load_schedule(FILE *fp,struct scd_blk *ptr) {
  int chr;
  int count=0;
  char txt[128];
  char line[SCHED_LINE_LENGTH];
  int default_present=0;
    
  ptr->num=0;
  ptr->cnt=0;
  while ( ( (chr=fgetc(fp)) != EOF) && 
          (ptr->num<SCHED_MAX_ENTRIES) ) {
    switch (chr) {
     case '#' :
	    /* comment so ignore rest of line */
		while ( ((chr=fgetc(fp)) != EOF) && (chr !='\n'));  
		break;
	 case '\n' :
	    /* ignore new lines */
		break;
	 case ' ' :
	   /* ignore leading spaces */
	   break;		
	 default : 
	   /* build string */
	   line[0]=(char) chr;
	   count=1;
	   while ( (count<SCHED_LINE_LENGTH) && ( (chr=fgetc(fp)) != EOF) && 
	   			(chr !='\n') ) {
		 line[count]=(char) chr;		
	     count++;
	   }
	   line[count]=NULL;
	   if (parse_schedule_line(line,ptr)==1) default_present=1;
	   break;
	}	
  }
 	
  if (ptr->num==0) {
    log_info(0,"No programs in schedule");
    return -1;
  }
  if (default_present==0) {
    log_info(0,"No default program in schedule");
    return -1;
  }

  if (ptr->num >= SCHED_MAX_ENTRIES) {
    sprintf(txt,"Schedule truncated : > %d entries",SCHED_MAX_ENTRIES);
    log_info(0,txt);
  }
  /* sort the events into order */
  qsort(ptr->entry,ptr->num,sizeof(struct scd_entry),cmp_scd);						    
  return 0;
}
