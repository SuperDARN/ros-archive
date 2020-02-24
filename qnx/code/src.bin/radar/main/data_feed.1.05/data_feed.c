/* data_feed.c
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
 $Log: data_feed.c,v $
 Revision 1.17  2001/06/27 20:55:56  barnes
 Added license tag

 Revision 1.16  2001/01/29 18:12:08  barnes
 Added Author Name

 Revision 1.15  2000/09/20 13:53:54  barnes
 Modification to implement the new hardware scheme.

 Revision 1.14  2000/09/18 18:23:30  barnes
 Added missing header.

 Revision 1.13  1999/04/10 20:37:35  barnes
 Added help message system.

 Revision 1.12  1999/03/19 18:37:33  barnes
 Modifications to support the new file libraries.

 Revision 1.11  1999/01/21 18:19:56  barnes
 Removed debug code.

 Revision 1.10  1999/01/21 18:17:29  barnes
 Added the ability to force the feed to use the actual time, and so more closely behave like a simulated Radar.

 Revision 1.9  1998/11/09 23:00:03  barnes
 Incorporated rawdata header.

 Revision 1.8  1998/10/23 17:46:36  barnes
 Added extra checks to make sure the configuration files can be found.

 Revision 1.7  1998/07/30 21:38:47  root
 Added the ability to specify when files are opened and closed.

 Revision 1.6  1998/06/11 16:50:57  root
 Fixed problems with setting the station id.

 Revision 1.5  1998/06/11 16:05:25  root
 Added support for the new hardware and name functions.

 Revision 1.4  1998/06/09 18:40:27  root
 Added the ability to distribute data to more than one task.

 Revision 1.3  1998/06/07 18:48:37  root
 Modification to use the fitfp structure.

 Revision 1.2  1998/06/03 20:04:49  root
 Modified the version control system

 Revision 1.1  1998/06/01 11:58:43  root
 Initial revision

 */

#include <i86.h>
#include <stdio.h>
#include <stdlib.h>
#include "print_info.h"
#include "cnv_time.h"
#include "rawdata.h"
#include "fitdata.h"
#include "hardware.h"
#include "radar_name.h"
#include "read_clock.h"
#include "fitacf.h"
#include "raw_read.h"
#include "fit_read.h"

#include "task_write.h"
#include "message.h"
#include "task_msg.h"
#include "errstr.h"
#include "hlpstr.h"

struct rawdata raw;
struct fitdata fit;
struct fit_block fblk;
struct radar_site *hdblk;
 
struct task_id *tsk[32];
struct task_block msg;

char *tasklist[32];

struct radar_hardware *hdw;
struct radar_id *rid;

long int strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   

void wait_boundary(float bnd) {
   int stime=0;
   int yr,mo,dy,hr,mt,sc,ms,us;
   read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
   bnd=bnd*1000;
   stime=(mt*60+sc)*1000+ms;
   stime=(int) bnd - (stime % (int) bnd);
   if (stime<0) return;
   sleep(stime / 1000);
   delay(stime% 1000);
}   

void main(int argc,char *argv[]) {
  int status;
  char *envstr;
  FILE *fp;
  struct rawfp *rawfp;
  struct fitfp *fitfp;
  int tnum=0;
  char *str={"data_read"};
  int otime=0;
  double stime;
  double ctime;
  
  int i=0,j;
  float intt=7;
  float bnd=120;
  int st_id=-1;
  int rawflg=0;
  int repflg=0;
  int aflg=0; /* use the actual time rather than that from the file */
  char tmp;
  int help=0; 

  int yr,mo,dy,hr,mt,sc,ms,us;

  envstr=getenv("SD_RADARNAME");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");
  if (fp==NULL) {
    fprintf(stderr,"Could not locate radar identity file.\n");
    exit(-1);
  }

  rid=load_radar(fp);
  fclose(fp); 

  envstr=getenv("SD_HARDWARE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HARDWARE' must be defined.\n");
    exit(-1);
  }

  hdw=load_hdw(envstr,rid);

  fclose(fp); 

  if (argc>1) {
    int c;
    for (c=1;c<argc;c++) { 
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-o")==0) {
        otime=strtime(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-d")==0) rawflg=1;
        else if (strcmp(argv[c],"-r")==0) repflg=1;
        else if (strcmp(argv[c],"-a")==0) aflg=1;
        else if (strcmp(argv[c],"-b")==0) {
        bnd=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-s")==0) {
        st_id=radar_id(rid,argv[c+1][0]);
        c++;
      } else if (strcmp(argv[c],"-i")==0) {
        intt=atof(argv[c+1]);
        c++;
      } else {
	    tasklist[tnum]=argv[c];
        tnum++;
      } 
    }
  } else {
   print_info(stderr,errstr);
   exit(1);
  }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }


  if (tnum==0) {
    fprintf(stderr,"No tasks were specified.\n");
    exit(-1);
  }

  if (rawflg==1) {
    rawfp=raw_open(argv[argc-1],NULL);
    if (rawfp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  } else {
    fitfp=fit_open(argv[argc-1],NULL);
    if (fitfp==NULL) {
      fprintf(stderr,"File not found.\n");
      exit(-1);
    }
  }

  for (i=0;i<tnum;i++) tsk[i]=make_task_id(tasklist[i]);

  if (rawflg ==1) { /* using a dat file */
    raw_read(rawfp,&raw);
    if (aflg==1) {
      if (raw.PARMS.SCAN==1) {
        fprintf(stderr,"Start of new scan\n");
        wait_boundary(bnd);
      }
      read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
      raw.PARMS.YEAR=yr;
      raw.PARMS.MONTH=mo;
      raw.PARMS.DAY=dy;
      raw.PARMS.HOUR=hr;
      raw.PARMS.MINUT=mt;
      raw.PARMS.SEC=sc;
      raw.PARMS.INTT=intt;
    }  

    for (i=0;i<tnum;i++) task_open(tsk[i],strlen(str),str,
            raw.PARMS.YEAR,raw.PARMS.MONTH,
			raw.PARMS.DAY,raw.PARMS.HOUR,raw.PARMS.MINUT,
            raw.PARMS.SEC);

    if (st_id !=-1) raw.PARMS.ST_ID=st_id;
    hdblk=get_site(hdw,raw.PARMS.YEAR,raw.PARMS.MONTH,
			raw.PARMS.DAY,raw.PARMS.HOUR,raw.PARMS.MINUT,
            raw.PARMS.SEC,raw.PARMS.ST_ID);
    start_fit(hdblk,raw.PARMS.YEAR,&fblk);
    stime=time_epoch(raw.PARMS.YEAR,raw.PARMS.MONTH,raw.PARMS.DAY,
	                 raw.PARMS.HOUR,raw.PARMS.MINUT,raw.PARMS.SEC);
  } else {
    fit_read(fitfp,&fit);
    if (aflg==1) {
      if (fit.prms.SCAN==1) {
        fprintf(stderr,"Start of new scan\n");
        wait_boundary(bnd);

      }
      read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
      fit.prms.YEAR=yr;
      fit.prms.MONTH=mo;
      fit.prms.DAY=dy;
      fit.prms.HOUR=hr;
      fit.prms.MINUT=mt;
      fit.prms.SEC=sc;
      fit.prms.INTT=intt;
    }  

    for (i=0;i<tnum;i++) task_open(tsk[i],strlen(str),str,
		    fit.prms.YEAR,fit.prms.MONTH,
			fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,
            fit.prms.SEC);
    stime=time_epoch(fit.prms.YEAR,fit.prms.MONTH,fit.prms.DAY,
	                 fit.prms.HOUR,fit.prms.MINUT,fit.prms.SEC);
  }
  do {
    msg.num=0;
    msg.tsize=0;
    if (rawflg==1) {
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",raw.PARMS.DAY,raw.PARMS.MONTH,
			raw.PARMS.YEAR,raw.PARMS.HOUR,raw.PARMS.MINUT,raw.PARMS.SEC);
      if (st_id !=-1) raw.PARMS.ST_ID=st_id;
      fitacf(&raw,&fblk,&fit);
      task_add(&msg,sizeof(struct rawdata),(unsigned char *) &raw,0,
			   raw.PARMS.YEAR,raw.PARMS.MONTH,
			   raw.PARMS.DAY,raw.PARMS.HOUR,raw.PARMS.MINUT,raw.PARMS.SEC);
    } else {
      fprintf(stderr,"%d-%d-%d %d:%d:%d\n",fit.prms.DAY,fit.prms.MONTH,
			fit.prms.YEAR,fit.prms.HOUR,fit.prms.MINUT,fit.prms.SEC);
    }
    ctime=time_epoch(fit.prms.YEAR,fit.prms.MONTH,fit.prms.DAY,
	                 fit.prms.HOUR,fit.prms.MINUT,fit.prms.SEC);
    if ((otime !=0) && ((ctime-stime)>=otime)) {
      fprintf(stderr,"Closing files.\n");
      for (i=0;i<tnum;i++) task_close(tsk[i],
        fit.prms.YEAR,fit.prms.MONTH,
		fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,
        fit.prms.SEC);
      fprintf(stderr,"Opening files.\n");
      for (i=0;i<tnum;i++) task_open(tsk[i],strlen(str),str,
          fit.prms.YEAR,fit.prms.MONTH,
		  fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,
          fit.prms.SEC);
      stime=ctime;
    }

    task_add(&msg,sizeof(struct fitdata),(unsigned char *) &fit,1,
			 fit.prms.YEAR,fit.prms.MONTH,
			 fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,fit.prms.SEC);
    for (i=0;i<tnum;i++) task_send(tsk[i],&msg);
    delay(intt*1000.0);
    
    if (rawflg==1) {
      status=raw_read(rawfp,&raw);
      if (aflg==1) {
        if (raw.PARMS.SCAN==1) {
          fprintf(stderr,"Start of new scan\n");
          wait_boundary(bnd);
        }
        read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
        raw.PARMS.YEAR=yr;
        raw.PARMS.MONTH=mo;
        raw.PARMS.DAY=dy;
        raw.PARMS.HOUR=hr;
        raw.PARMS.MINUT=mt;
        raw.PARMS.SEC=sc;
        raw.PARMS.INTT=intt;
      }  
    } else {
      status=fit_read(fitfp,&fit);
      if (aflg==1) {
        if (fit.prms.SCAN==1) {
          fprintf(stderr,"Start of new scan\n");
          wait_boundary(bnd);
        }
        read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
        fit.prms.YEAR=yr;
        fit.prms.MONTH=mo;
        fit.prms.DAY=dy;
        fit.prms.HOUR=hr;
        fit.prms.MINUT=mt;
        fit.prms.SEC=sc;
        fit.prms.INTT=intt;
      }  
      if (st_id !=-1) fit.prms.ST_ID=st_id;
    }
    if ((status !=0) && (repflg==1)) {
      status=0; 
      if (rawflg==1) {
        raw_close(rawfp);
        rawfp=raw_open(argv[argc-1],NULL);
        status=raw_read(rawfp,&raw);
        if (aflg==1) {
          if (raw.PARMS.SCAN==1) {
            fprintf(stderr,"Start of new scan\n");
            wait_boundary(bnd);
          }
          read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
          raw.PARMS.YEAR=yr;
          raw.PARMS.MONTH=mo;
          raw.PARMS.DAY=dy;
          raw.PARMS.HOUR=hr;
          raw.PARMS.MINUT=mt;
          raw.PARMS.SEC=sc;
          raw.PARMS.INTT=intt;
        }  

        for (i=0;i<tnum;i++) task_close(tsk[i],
            raw.PARMS.YEAR,raw.PARMS.MONTH,
			raw.PARMS.DAY,raw.PARMS.HOUR,raw.PARMS.MINUT,
            raw.PARMS.SEC);
        for (i=0;i<tnum;i++) task_open(tsk[i],
           strlen(str),str,raw.PARMS.YEAR,raw.PARMS.MONTH,
			raw.PARMS.DAY,raw.PARMS.HOUR,raw.PARMS.MINUT,
            raw.PARMS.SEC);
        stime=time_epoch(raw.PARMS.YEAR,raw.PARMS.MONTH,raw.PARMS.DAY,
	                 raw.PARMS.HOUR,raw.PARMS.MINUT,raw.PARMS.SEC);
      } else {
        fit_close(fitfp);
        fitfp=fit_open(argv[argc-1],NULL);        
        status=fit_read(fitfp,&fit);
        if (aflg==1) {
          if (fit.prms.SCAN==1) {
            fprintf(stderr,"Start of new scan\n");
            wait_boundary(bnd);
          }
          read_clock(&yr,&mo,&dy,&hr,&mt,&sc,&ms,&us);
          fit.prms.YEAR=yr;
          fit.prms.MONTH=mo;
          fit.prms.DAY=dy;
          fit.prms.HOUR=hr;
          fit.prms.MINUT=mt;
          fit.prms.SEC=sc;
          fit.prms.INTT=intt;
        }   
        for (i=0;i<tnum;i++) task_close(tsk[i],
		    fit.prms.YEAR,fit.prms.MONTH,
			fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,
            fit.prms.SEC);
        for (i=0;i<tnum;i++) task_open(tsk[i],
	        strlen(str),str,fit.prms.YEAR,fit.prms.MONTH,
			fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,
            fit.prms.SEC);
        stime=time_epoch(fit.prms.YEAR,fit.prms.MONTH,fit.prms.DAY,
	                 fit.prms.HOUR,fit.prms.MINUT,fit.prms.SEC);

      } 
    }
  } while (status==0);
  for (i=0;i<tnum;i++) task_quit(tsk[i]);
  if (rawflg==1) end_fit(&fblk);
}
