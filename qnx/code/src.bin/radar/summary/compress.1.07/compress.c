/* compress.c
   ==========
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
 $Log: compress.c,v $
 Revision 1.17  2002/02/02 21:46:57  barnes
 Modification to deal with new format files that can contain multiple channels.

 Revision 1.16  2001/12/17 16:46:57  barnes
 Fixed bug in memory release.

 Revision 1.15  2001/06/27 20:56:45  barnes
 Added license tag

 Revision 1.14  2001/01/29 18:12:09  barnes
 Added Author Name

 Revision 1.13  2000/11/13 20:17:31  barnes
 Fixed bug in passing scale limits.

 Revision 1.12  2000/02/16 20:17:20  barnes
 Changed upper limits to prevent saturation.

 Revision 1.11  1999/04/10 20:33:17  barnes
 Added help message system.

 Revision 1.10  1999/03/30 19:17:52  barnes
 Moved task names to a header.

 Revision 1.9  1999/03/19 18:36:48  barnes
 Modifications to support the new file libraries.

 Revision 1.8  1999/01/28 14:17:57  barnes
 Modifications to cope with variations in frang and rsep.

 Revision 1.7  1998/11/09 23:08:35  barnes
 Incorporates rawdata header.

 Revision 1.6  1998/06/19 15:51:29  root
 Added support for the new radar_name function.

 Revision 1.5  1998/06/12 10:17:22  root
 Changed the command line option that specifies the version of echo_data.

 Revision 1.4  1998/06/11 15:50:39  root
 Fixed problem with reset messsage which would have terminated the task.

 Revision 1.3  1998/06/11 15:48:49  root
 Added support for the TASK_RESET message.

 Revision 1.2  1998/06/11 15:12:36  root
 Moved usage message to another file.

 Revision 1.1  1998/06/05 10:06:57  root
 Initial revision

 */

#include <stdio.h>
#include <sys/name.h>
#include <stdlib.h>
#include <sys/kernel.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "radar_name.h"
#include "message.h"
#include "task_msg.h"
#include "decode_msg.h"
#include "file_io.h"
#include "log_error.h"
#include "echo_util.h"
#include "task_name.h"

#include "cmpdata.h"
#include "fit_cmp.h"
#include "cmp_write.h"

#include "version.h"
#include "hlpstr.h"

int cmp_id;
char errlog_name[256]=ERRLOG;
char errstr[256];
char msg;

struct cmpdata cmp;
struct cmphdr cmp_hdr;

struct task_decode tblk;
struct task_decode *ptr;

struct radar_id *rid; 

main (int argc,char *argv[]) {

  int task_id;
  pid_t reply_tid;
  char rmsg,sfx;
  int status,i;
  int running=1;
  int help=0;

  struct fitdata *fdt;
  FILE *fp;
  char *envstr;
  FILE *cmpfp;
  int sflag=0x00;
  double pmax=60.0;
  double vmax=2000.0;
  double wmax=1000.0;
  char file_name[64];
  char echo_name[32]={ECHO_DATA};
  double min_pwr=3.0;
  int rflg=0;
  char *cmpname;

  char echo_pass=FIT_TYPE;
  struct task_id *tsk;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }

 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);
 
  ptr=&tblk;

  /* files are time codes by minutes into the year */
 
  if (argc>1) {
    int c;
    for (c=1;c<argc;c++) { 
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-p")==0) {
        pmax=atof(argv[c+1]);
        sflag=sflag | 0x01;
        c++;
      } else if (strcmp(argv[c],"-v")==0) {
        vmax=atof(argv[c+1]);
        sflag=sflag | 0x02;
        c++;
      } else if (strcmp(argv[c],"-w")==0) {
        wmax=atof(argv[c+1]);
        sflag=sflag | 0x04;
        c++;
      } else if (strcmp(argv[c],"-a")==0) sflag=sflag | 0x07;
      else if (strcmp(argv[c],"-e")==0) {
        strcpy(errlog_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-t")==0) {
        min_pwr=atof(argv[c+1]);
        c++;
        if (min_pwr<0) min_pwr=0;
        if (min_pwr>30) min_pwr=30;
      } else if (strcmp(argv[c],"-ec")==0) {
        strcpy(echo_name,argv[c+1]);
        c++;
      } 
    } 
  }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  tsk=make_task_id(errlog_name);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

  sprintf(errstr,"Started (version %s)",VSTRING);
  log_error(tsk,TASK_NAME,errstr);   
   	    
  if (echo_register(TASK_NAME,echo_name,&echo_pass,1) != 0) {
    log_error(tsk,TASK_NAME,"Unable to attach to echo_data.");
    exit(EXIT_FAILURE);
  }

  /* wait for messages */
  
  running=1; 
  while (running) {
    reply_tid=Receive(0,&msg,sizeof(msg));
    rmsg=decode_msg(reply_tid,msg,ptr);
    Reply(reply_tid,&rmsg,sizeof(char));
    if (msg==TASK_RESET) {
       log_error(tsk,TASK_NAME,"Reset");
       if (cmpname !=NULL) free(cmpname);
       cmpname=NULL;
    } else if (msg==TASK_QUIT) {
       log_error(tsk,TASK_NAME,"Stopped");
       if (cmpname !=NULL) free(cmpname);
       exit (0);
    } else if (msg==TASK_DATA) {
      for (i=0;i<ptr->block.num;i++) {
        if (ptr->block.data[i].type !=FIT_TYPE) continue;
        fdt=(struct fitdata *) (ptr->block.ptr[ptr->block.data[i].index]);
        if (close_poll(ptr,i) !=0) {
          log_error(tsk,TASK_NAME,"Closing file");
          if (cmpname !=NULL) free(cmpname);
          cmpname=NULL;
        }
        if (open_poll(ptr,i) !=0) {
          log_error(tsk,TASK_NAME,"Opening file");
          sfx=0;
          cmpname=open_file(getenv("SD_CMP_PATH"),
							ptr->block.data[i].time,
							radar_code(rid,fdt->prms.ST_ID),"cmp",0,&sfx,0);
 
          cmpfp=fopen(cmpname,"a");

          fit_cmphdr(pmax,vmax,wmax,fdt,&cmp_hdr);
          if (cmpfp !=NULL) status=cmp_header_fwrite(cmpfp,&cmp_hdr);
          else status=1;

          if (status !=0) {
            if (cmpfp !=NULL) fclose(cmpfp);
            /* failed to write header */
            log_error(tsk,TASK_NAME,
					"Write header failed");
            cmpfp=NULL;
          }   
        } 

        if ((cmpfp==NULL)  && (cmpname !=NULL)) cmpfp=fopen(cmpname,"a");
        
        if (cmpfp==NULL) {
           log_error(tsk,TASK_NAME,"File not open");
           continue;
        }      

        /* process the records in here */
        rflg=fit_cmp(sflag,min_pwr,pmax,vmax,wmax,fdt,&cmp,0);
        status=cmp_fwrite(cmpfp,&cmp,1 | (rflg<<1) );
        if (status==-1) log_error(tsk,TASK_NAME,"Error writing record");
       
      }
      fclose(cmpfp);
      cmpfp=NULL;
      free(ptr->store);  
      ptr->store=NULL;        
    }
  }
} 
