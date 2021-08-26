/* fit_write.c
   =========== */

/*
 $Log: fit_write.c,v $
 Revision 1.14  2000/09/18 18:22:36  barnes
 Added missing header.

 Revision 1.13  1999/04/10 20:44:22  barnes
 Added help message system.

 Revision 1.12  1999/03/30 19:22:46  barnes
 Moved task names into a header.

 Revision 1.11  1999/03/19 18:37:44  barnes
 Modifications to support the new file libraries.

 Revision 1.10  1998/11/09 23:03:56  barnes
 Incorporated rawdata header.

 Revision 1.9  1998/08/20 17:10:45  barnes
 Changed file suffix to lower case.

 Revision 1.8  1998/06/19 14:39:27  root
 Use the new radar_name function.

 Revision 1.7  1998/06/11 15:53:20  root
 Fixed problem with the reset message.

 Revision 1.6  1998/06/11 15:46:13  root
 Added support for the TASK_RESET message.

 Revision 1.5  1998/06/11 14:53:37  root
 Moved usage into a different file.

 Revision 1.4  1998/06/04 18:59:09  root
 Modification to stop the files being opened and closed each time a
 data block is processed.

 Revision 1.3  1998/06/03 20:20:23  root
 Modification to the version control system.

 Revision 1.2  1998/06/02 12:21:34  root
 Fixed problems in formatting the output file.

 Revision 1.1  1998/05/29 11:59:35  root
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <sys/sendmx.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "radar_name.h"
#include "fit_write.h"
#include "message.h"
#include "decode_msg.h"
#include "task_msg.h"
#include "file_io.h"
#include "log_error.h"

#include "version.h"
#include "task_name.h"
#include "hlpstr.h"

struct radar_id *rid;

main (int argc,char *argv[]) {

  FILE *fp;
  char *envstr;
  int status;
  int fit_id;
  pid_t task_id;
  int reply_tid;
  char msg;
  char rmsg;
  int i,j; 
  struct fitdata *fdt;
  char *fitname=NULL;
  char *inxname=NULL;
  FILE *fitfp;
  FILE *inxfp;
  char sfx;  
  int drec=0;
  int dnum=0;
  int irec=0;

  struct task_decode tblk;
  struct task_decode *ptr; 
  char errbuf[256];
  char fit_name[32]={TASK_NAME};
  char errlog_name[32]={ERRLOG};
  struct task_id *tsk;
  int thresh=0;
  int help=0;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }
 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);

  ptr=&tblk;
 
  /* allocate memory to store the records */
 
  if (argc>1) { 
    int c;
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1;
      if (strcmp(argv[c],"-t")==0) {
        thresh=atoi(argv[c+1]);
        c++; 
      } else if (strcmp(argv[c],"-e")==0) {
        strcpy(errlog_name,argv[c+1]);
        c++;
      } else strcpy(fit_name,argv[c]);
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

  if ( (fit_id=qnx_name_attach(0,fit_name) ) == -1) { 
    perror("Unable to attach fit_write task");
    log_error(tsk,fit_name,
						"Unable to register name");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version %s)",VSTRING);
  log_error(tsk,fit_name,errbuf);  

  while (1) {
   
    reply_tid=Receive(0,&msg,sizeof(char));
    rmsg=decode_msg(reply_tid,msg,ptr);
    Reply(reply_tid,&rmsg,sizeof(char));
    if (msg==TASK_RESET) {
       log_error(tsk,fit_name,"Reset");
       status=0;
       if ((fitname !=NULL) && (inxname !=NULL)) {
         log_error(tsk,fit_name,"Closing file"); 
         status=((inxfp=fopen(inxname,"r+")) ==NULL);
         if (status==0) status=fit_inx_fclose(inxfp,fdt,irec-1);            
         if (inxfp !=NULL) fclose(inxfp);
         inxfp=NULL;
       }
       if (inxname !=NULL) free(inxname);
       if (fitname !=NULL) free(fitname);
       inxname=NULL;
       fitname=NULL;
    } else if (msg==TASK_QUIT) {
        log_error(tsk,fit_name,"Stopped");
  
       /* close down here */
       status=0;
       if ((fitname !=NULL) && (inxname !=NULL)) {
         log_error(tsk,fit_name,"Closing file"); 
         status=((inxfp=fopen(inxname,"r+")) ==NULL);
         if (status==0) status=fit_inx_fclose(inxfp,fdt,irec-1);            
         if (inxfp !=NULL) fclose(inxfp);
         inxfp=NULL;
       }
       if (inxname !=NULL) free(inxname);
       if (fitname !=NULL) free(fitname);
       if (status !=0) log_error(tsk,fit_name,"Closing index file failed");
       exit (0);
    } else if (msg==TASK_DATA) {
      for (i=0;i<ptr->block.num;i++) {
        if (ptr->block.data[i].type !=FIT_TYPE) continue;
        fdt=(struct fitdata *) (ptr->block.ptr[ptr->block.data[i].index]);
        if (close_poll(ptr,i) !=0) {
          log_error(tsk,fit_name,"Closing file");
          if ((fitname !=NULL) && (inxname !=NULL)) {
            status=((inxfp=fopen(inxname,"r+")) ==NULL);
            if (status==0) status=fit_inx_fclose(inxfp,fdt,irec-1);            
            if (inxfp !=NULL) fclose(inxfp);
            inxfp=NULL;
          }
 
          if (fitname !=NULL) free(fitname);
          if (inxname !=NULL) free(inxname);
          inxname=NULL;
          fitname=NULL;
        }
        if (open_poll(ptr,i) !=0) {
          log_error(tsk,fit_name,"Opening file");
          sfx=0;
          fitname=open_file(getenv("SD_FIT_PATH"),
							ptr->block.data[i].time,
							radar_code(rid,fdt->prms.ST_ID),"fit",0,&sfx,0);
          inxname=open_file(getenv("SD_FIT_PATH"),
							ptr->block.data[i].time,
							radar_code(rid,fdt->prms.ST_ID),"inx",0,&sfx,1);
 
          fitfp=fopen(fitname,"a");
          if (fitfp !=NULL) 
            status=fit_header_fwrite(fitfp,"fitwrite","fitacf","4.0");
          else status=1;
        
          if (status !=0) {
            if (fitfp !=NULL) fclose(fitfp);
            /* failed to write header */
            log_error(tsk,fit_name,
					"Write header failed");
            fitfp=NULL;
          }

          inxfp=fopen(inxname,"a");
          if (inxfp !=NULL) status=fit_inx_header_fwrite(inxfp,fdt);
          else status=1;
        
          if (status !=0) {
            if (inxfp !=NULL) fclose(inxfp);
            /* failed to write header */
            log_error(tsk,fit_name,
					"Write index header failed");
            inxfp=NULL;
          }

          drec=2;
          irec=1;
        } 

        if ((fitfp==NULL)  && (fitname !=NULL)) fitfp=fopen(fitname,"a");
        if ((inxfp==NULL)  && (inxname !=NULL)) inxfp=fopen(inxname,"a");

        if (fitfp==NULL) {
           log_error(tsk,fit_name,"File not open");
           continue;
        }      

        if (inxfp==NULL) {
           log_error(tsk,fit_name,"Index file not open");
           continue;
        }      

        /* process the records in here */
        dnum=fit_fwrite(fitfp,fdt);
        if (dnum ==-1) log_error(tsk,fit_name,"Error writing record");
        fit_inx_fwrite(inxfp,drec,dnum,fdt);
        drec+=dnum;
        irec++;


      }
      fclose(fitfp);
      fitfp=NULL;
      fclose(inxfp);
      inxfp=NULL;
      free(ptr->store);          
    }
  }
} 
