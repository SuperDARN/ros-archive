/* sd_summary.c
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
$Log: sd_summary.c,v $
Revision 1.5  2001/12/17 16:46:57  barnes
Fixed bug in memory release.

Revision 1.4  2001/06/27 20:56:55  barnes
Added license tag

Revision 1.3  2001/01/29 18:12:09  barnes
Added Author Name

Revision 1.2  1999/04/10 20:34:57  barnes
Added the help message system.

Revision 1.1  1999/03/31 15:38:00  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <errno.h>
#include <unistd.h>

#include "types.h"
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

#include "smr_write.h"

#include "version.h"
#include "task_name.h"
#include "hlpstr.h"

#define DEFAULT_BEAMNO 2
#define DEFAULT_BEAMS 5,10
#define DEFAULT_POWER 3
#define GET_BEAM 'g'
#define PUT_BEAM 'p'

char errlog_name[32]={ERRLOG};
char echo_name[32]={ECHO_DATA};
char sd_name[32]={TASK_NAME};

struct task_decode tblk;
struct task_decode *ptr; 

struct radar_id *rid;

int parse_beam(char *str,int *lst) {
  int i=0,j=0,k=0;
  
  for (j=0;str[j] !=0;j++) {
    if (str[j]==',') {
      str[j]=0;
      lst[k]=atoi(str+i);
      i=j+1;
      k++;
      if (k>16) break;
    }
  }  
  if (k<16) {
    lst[k]=atoi(str+i);
    i=j+1;
    k++;
  }
  return k;
}

void main( int argc, char * argv[]) {

  char *envstr;
  FILE *fp;
  FILE * smrfp=NULL;
  int help=0;

  char rev_string[32], *env_str;
  char fname[80];

  nid_t ntid;
  pid_t reply_tid;

  struct fitdata *fdt=NULL;

  char msg;
  char rmsg;

  int flag;
  char sfx;

  int beamno=DEFAULT_BEAMNO;
  int beamlist[16]={DEFAULT_BEAMS};
  int select_power, ngood, i,k, c, n;
  long int rec_time;
  int status;

  char echo_pass=FIT_TYPE;
  struct task_id *tsk;
 	
  char errbuf[256];
  char *smrname=NULL;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }

 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);

  ptr=&tblk;

  select_power = DEFAULT_POWER;

  if (argc>1) { 
    int c;
    for (c=1;c<argc;c++) { 
      if (strcmp(argv[c],"--help")==0) help=1;
      if (strcmp(argv[c],"-e")==0) {
        strcpy(errlog_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-ec")==0) {
        strcpy(echo_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-b")==0) {
        beamno=parse_beam(argv[c+1],beamlist);
        c++;
      }  else if (strcmp(argv[c],"-p")==0) {
        select_power=atoi(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-n")==0) {
        strcpy(sd_name,argv[c+1]);
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


  if ( (ntid=qnx_name_attach(0,sd_name) ) == -1) { 
    log_error(tsk,sd_name,"Unable to attach name");
    exit(1);
  }

  if (echo_register(sd_name,echo_name,&echo_pass,1) != 0) {
    log_error(tsk,sd_name,"Unable to attach to echo_data.");
    exit(EXIT_FAILURE);
  }


  sprintf(errbuf,"Started (version %s)",VSTRING);
  log_error(tsk,sd_name,errbuf);  
			
  /* OK now allocate the memory needed for the fitdata */
	
  while(1) {
    reply_tid=Receive(0,&msg,sizeof(msg));
    rmsg=decode_msg(reply_tid,msg,ptr);
    if (msg==GET_BEAM) {
      /* send the stored beams back to the task */
      struct _mxfer_entry reply_mx[3];
      _setmx(&reply_mx[0],&select_power,sizeof(int));      
      _setmx(&reply_mx[1],&beamno,sizeof(int)); 
      _setmx(&reply_mx[2],beamlist,sizeof(int)*beamno); 
      Replymx(reply_tid,3,reply_mx);
    } else if (msg==PUT_BEAM) {
      /* decode the message block */
      struct _mxfer_entry send_mx[3];
      _setmx(&send_mx[0],&select_power,sizeof(int));
      _setmx(&send_mx[1],&beamno,sizeof(int));
      Readmsgmx(reply_tid,sizeof(char),2,send_mx);
      _setmx(&send_mx[0],beamlist,sizeof(int)*beamno);
      Readmsgmx(reply_tid,sizeof(char)+2*sizeof(int),1,send_mx);
      rmsg=TASK_OK;
      Reply(reply_tid,&rmsg,sizeof(char));
    } else {
      Reply(reply_tid,&rmsg,sizeof(char));
      if (msg==TASK_RESET) {
         log_error(tsk,sd_name,"Reset");
         if (smrname !=NULL) free(smrname);
         smrname=NULL;
      } else if (msg==TASK_QUIT) {
         log_error(tsk,sd_name,"Stopped");
         if (smrname !=NULL) free(smrname);
         exit (0);
      } else if (msg==TASK_DATA) {
        for (i=0;i<ptr->block.num;i++) {
          if (ptr->block.data[i].type !=FIT_TYPE) continue;
          fdt=(struct fitdata *) (ptr->block.ptr[ptr->block.data[i].index]);
          if (close_poll(ptr,i) !=0) {
            log_error(tsk,sd_name,"Closing file");
            if (smrname !=NULL) free(smrname);
            smrname=NULL;
          }
          if (open_poll(ptr,i) !=0) {
            log_error(tsk,sd_name,"Opening file");
            sfx=0;
            smrname=open_file(getenv("SD_SMR_PATH"),
							ptr->block.data[i].time,
							radar_code(rid,fdt->prms.ST_ID),"smr",0,&sfx,0);
 
            smrfp=fopen(smrname,"a");

            if (smrfp !=NULL) 
              status=smr_header_fwrite(smrfp,VSTRING,ptr->open.ptr,
									radar_name(rid,fdt->prms.ST_ID));
            else status=1;
         
            if (status !=0) {
              if (smrfp !=NULL) fclose(smrfp);     
              log_error(tsk,sd_name,
					"Write header failed");
              smrfp=NULL;
            }   

          } 

          if ((smrfp==NULL)  && (smrname !=NULL)) smrfp=fopen(smrname,"a");
        
          if (smrfp==NULL) {
             log_error(tsk,sd_name,"File not open");
             continue;
          }        

          /* process the records in here */

          for (k=0;(k<beamno) && (fdt->prms.BMNUM !=beamlist[k]);k++);

          if (k<beamno) {
             smr_fwrite(smrfp,fdt,select_power);
             if (status==-1) log_error(tsk,sd_name,"Error writing record");
          }
        }
        fclose(smrfp);
        smrfp=NULL;
        free(ptr->store);         
        ptr->store=NULL; 
      }
    }
  }
}
		
