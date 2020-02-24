/* gridfit.c
   =========
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
 $Log: gridfit.c,v $
 Revision 2.30  2002/02/08 03:35:43  barnes
 Fixed further niggly bug.

 Revision 2.29  2002/02/08 01:51:12  barnes
 Fixed bug in setting the channel number.

 Revision 2.28  2002/02/08 00:40:15  barnes
 Modification to deal with multiple channels.

 Revision 2.27  2001/12/17 16:46:57  barnes
 Fixed bug in memory release.

 Revision 2.26  2001/06/27 20:56:49  barnes
 Added license tag

 Revision 2.25  2001/01/29 18:12:09  barnes
 Added Author Name

 Revision 2.24  2000/11/13 22:12:08  barnes
 Fixed another bug in reading the hardware information.

 Revision 2.23  2000/11/13 22:07:56  barnes
 Modification for the inertial reference frame and to correct bug in
 reading the hardware information.

 Revision 2.22  2000/10/30 18:40:13  barnes
 Fixed crash at start because the scan structure is not initialized.

 Revision 2.21  2000/09/18 18:03:55  barnes
 Removed redundant header.

 Revision 2.20  1999/12/15 21:59:08  barnes
 Moved to individual hardware files.

 Revision 2.19  1999/10/13 21:17:21  barnes
 Modifications for the extended file format.

 Revision 2.18  1999/09/13 22:36:29  barnes
 Fixed bug that meant the routine was defaulting to recording power.

 Revision 2.17  1999/09/13 14:07:23  barnes
 Modifications to the new libraries.

 Revision 2.16  1999/04/10 22:00:17  barnes
 Added support for extended file format.

 Revision 2.15  1999/03/30 19:16:18  barnes
 Moved task names to a header.

 Revision 2.14  1999/03/19 18:37:13  barnes
 Modifications to support changes in how scan data is managed.

 Revision 2.13  1999/01/27 18:51:31  barnes
 Modifications to cope with the new radardata structure.

 Revision 2.12  1998/11/10 15:37:47  barnes
 Removed diagnostic code.

 Revision 2.11  1998/11/09 23:09:12  barnes
 Incorporated rawdata header.

 Revision 2.10  1998/11/09 17:12:20  barnes
 Modification to fix time stamp problems with the records.

 Revision 2.9  1998/10/30 21:35:03  barnes
 Changed coding of the time stamp.

 Revision 2.8  1998/10/29 19:46:27  barnes
 Corrected bug that caused a segment violation when one of the
 environment variables was not defined.

 Revision 2.7  1998/07/30 22:01:45  root
 Fixed the problem of the open and close messages not being synchronized
 to the data written in the file.

 Revision 2.6  1998/07/30 17:42:38  root
 Changed the code to use the structure gtable.

 Revision 2.5  1998/06/19 14:52:28  root
 Added support for the radar_name function.

 Revision 2.4  1998/06/12 10:17:51  root
 Changed the command line option that specifies the version of echo_data.

 Revision 2.3  1998/06/11 15:50:05  root
 Added support for the TASK_RESET message.

 Revision 2.2  1998/06/07 18:49:13  root
 Modifications to use the new radardata structure.

 Revision 2.1  1998/06/05 10:54:57  root
 New version using the master library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/name.h>
#include <sys/kernel.h>
#include <errno.h>
#include <unistd.h>

#include "print_info.h"
#include "rawdata.h"
#include "hardware.h"
#include "radar_name.h" 
#include "fitdata.h"

#include "radar_name.h"
#include "message.h"
#include "task_msg.h"
#include "decode_msg.h"
#include "file_io.h"
#include "log_error.h"
#include "echo_util.h"

#include "version.h"

#include "radar_scan.h"

#include "filter.h"
#include "bound.h"
#include "check_ops.h"
#include "gtable.h"
#include "write_grid.h"
#include "radar_scan.h"

#include "process_beam.h"

#include "task_name.h"
#include "hlpstr.h"

#define SFX "grd"


char errlog_name[32]={ERRLOG};
char echo_name[32]={ECHO_DATA};
char task_name[32]={TASK_NAME};

char errbuf[256];
struct radardata scan;

struct task_decode tblk;
struct task_decode *ptr; 
struct radar_id *rid;
struct radar_hardware *hdt=NULL;


struct gtable grid; 
struct radardata buffer[3];


main(int argc, char * argv[]) {

  FILE *fp;
  char *envstr;
  pid_t reply_tid;
  FILE *grdfp=NULL;
  char *path;
  char *grdname=NULL;
  int tlen=2*60; 
  int nbox=3;
  int status;
  int i,j;
  struct radar_site *hdw=NULL;

  struct fitdata *fdt=NULL;
  char suffix[10];

  char msg;
  char rmsg;
  int help=0;
 
  int flag;
  char sfx;
  int cnt=0;
  int bxcar=1;
  int mode=0;
  int iflg=0;

 
  double min[4]={35,3,10,0};
  double max[4]={2000,50,1000,200};
 
  int channel=-1;
 
  int limit=1;

  int bflg=1;
  int xtd=0;

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


  envstr=getenv("SD_HARDWARE");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_HARDWARE' must be defined.\n");
    exit(1);
  }

  hdt=load_hdw(envstr,rid);
 

  ptr=&tblk;

  grid.st_time=-1;
  grid.status=0;
  grid.pnt=NULL;
  grid.gsct=0;
  
  if (argc>1) { 
    int c;
    for (c=1;c<argc;c++) {
      if (strcmp(argv[c],"--help")==0) help=1; 
      else if (strcmp(argv[c],"-e")==0) {
        strcpy(errlog_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-ec")==0) {
        strcpy(echo_name,argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-fwgt")==0) {
        mode=atoi(argv[c+1])-1;
        c++;
      } else if (strcmp(argv[c],"-pmax")==0) {
        max[1]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-pmin")==0) {
        min[1]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-vmax")==0) {
        max[0]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-vmin")==0) {
        min[0]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-wmax")==0) {
        max[2]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-wmin")==0) {
        min[2]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-vemax")==0) {
        max[3]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-vemin")==0) {
        min[3]=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-cn")==0) {
        if (tolower(argv[c+1][0])=='a') channel=1;
        if (tolower(argv[c+1][0])=='b') channel=2;
        c++;
     } else if (strcmp(argv[c],"-gs")==0) grid.gsct=1;
      else if (strcmp(argv[c],"-ion")==0) grid.gsct=0;
      else if (strcmp(argv[c],"-both")==0) grid.gsct=2;
      else if (strcmp(argv[c],"-nlm")==0) limit=0;
      else if (strcmp(argv[c],"-nav")==0) bxcar=0;
      else if (strcmp(argv[c],"-nb")==0) bflg=0;
      else if (strcmp(argv[c],"-xtd")==0) xtd=1;
      else if (strcmp(argv[c],"-inertial")==0) iflg=1;
    }
  }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0); 
  }

 

  for (i=0;i<4;i++) {
    grid.min[i]=min[i];
    grid.max[i]=max[i];
  }
 

  if (channel==-1) sprintf(suffix,"grd");
  else if (channel==1) sprintf(suffix,"a.grd");
  else if (channel==2) sprintf(suffix,"b.grd");

  tsk=make_task_id(errlog_name);
  if (tsk==NULL) {
    perror("Unable to establish error handler.");
    exit(EXIT_FAILURE);
  }  

  if (getenv("SD_GRD_PATH") == NULL) {
    log_error(tsk,task_name,
      "Cannot find the env variable SD_GRD_PATH");
       exit(-1);
  }   

  path=getenv("SD_GRD_PATH");

  if (echo_register(TASK_NAME,echo_name,&echo_pass,1) != 0) {
    log_error(tsk,TASK_NAME,"Unable to attach to echo_data.");
    exit(EXIT_FAILURE);
  }

  sprintf(errbuf,"Started (version %s)",VSTRING);
  log_error(tsk,task_name,errbuf);  

  reset_scan(&scan);

  while (1) {
    reply_tid=Receive(0,&msg,sizeof(msg));
    rmsg=decode_msg(reply_tid,msg,ptr);
    Reply(reply_tid,&rmsg,sizeof(char));
    if (msg==TASK_RESET) {
       log_error(tsk,TASK_NAME,"Reset");
       if (grdname !=NULL) free(grdname);
       grdname=NULL;
    } else if (msg==TASK_QUIT) {
       log_error(tsk,TASK_NAME,"Stopped");
       if (grdname !=NULL) free(grdname);
       exit (0);
    } else if (msg==TASK_DATA) {
      for (i=0;i<ptr->block.num;i++) {
        if (ptr->block.data[i].type !=FIT_TYPE) continue;
        fdt=(struct fitdata *) (ptr->block.ptr[ptr->block.data[i].index]);

        if (channel !=-1) {
           if ((channel==1) && (fdt->prms.CHANNEL==2)) continue;
           if ((channel==2) && (fdt->prms.CHANNEL!=2)) continue;
	}

        if (channel !=-1) grid.chn=fdt->prms.CHANNEL;                                  else grid.chn=0;                                                        
      

        /* process the records in here */
        if ((fdt->prms.SCAN==1) && (cnt>0)) {
  	      /* process the first complete record */
               
          bound_type(&scan,grid.gsct);
          if (bflg==1) bound(15,&scan,grid.min,grid.max);
        
          if (bxcar==1) {
            for (j=1;j<nbox;j++) copy_scan(&buffer[j-1],&buffer[j]);
            copy_scan(&buffer[nbox-1],&scan);      
          } else for (j=0;j<nbox;j++) copy_scan(&buffer[j],&scan);
          cnt++;
          reset_scan(&scan);
          scan.stime=-1;
          scan.etime=-1;

          /* process the data records */ 
          
          if ((cnt>=nbox) && (limit==1) && (mode !=-1)) 
            status=check_ops(buffer)==-1;
          else status=0;
          if ((cnt>=nbox) && (status==0)) {   
            
            if (mode==-1) middle(buffer,&scan);
              else {
               filter_raw(mode,buffer,&scan,1+6*xtd);
	         }
            
            if (hdw==NULL) {
              int yr,mo,dy,hr,mt;
              double sc;
              epoch_time(scan.stime,&yr,&mo,&dy,&hr,&mt,&sc);
              hdw=get_site(hdt,yr,mo,dy,hr,mt,(int) sc,scan.st_id);
            }
            status=test_gtable_write(&grid,&scan,tlen);
         
            if (status==1) { /* record available for writing */ 
             
              if ((ptr->closetme !=0) && 
                  (grid.st_time>=ptr->closetme)) {
                 ptr->closetme=0;
                 log_error(tsk,TASK_NAME,"Closing file");
                 if (grdname !=NULL) free(grdname);
                 grdname=NULL;

              }                 
              if ((ptr->opentme !=0) && 
	              (grid.st_time>=ptr->opentme)) {
                 ptr->opentme=0;
                 log_error(tsk,TASK_NAME,"Opening file");
                 sfx=0;
                 grdname=open_file(path,
			     grid.st_time,
			     radar_code(rid,fdt->prms.ST_ID),suffix,0,&sfx,0);
              }                 

            
              if ((grdname !=NULL) && (grdfp=fopen(grdname,"a")) !=NULL) {
               
                fwrite_gtable(grdfp,&grid,NULL,xtd);
              
                fclose(grdfp);
              }
            }
            map_to_gtable(&grid,&scan,hdw,tlen,iflg);           
            reset_scan(&scan);
            scan.stime=-1;
            scan.etime=-1;
          }
        } else if (fdt->prms.SCAN==1) cnt++;
        process_beam(fdt,&scan);         
      }
      free(ptr->store);          
      ptr->store=NULL;
    }
  }    
} 
		
