/* trim_fit.c
   ========== */

/*
 $Log: trim_fit.c,v $
 Revision 1.7  2000/09/18 18:26:01  barnes
 Changed file header to trim_fit.

 Revision 1.6  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.5  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.4  1999/11/02 13:57:51  barnes
 Added the ability to set the start and end dates.

 Revision 1.3  1999/08/12 13:42:57  barnes
 Added verbose option and fixed end time.

 Revision 1.2  1999/05/18 18:05:41  barnes
 Fixed problem with setting the start time of the file is not specified
 on the command line.

 Revision 1.1  1999/05/18 17:29:11  barnes
 Initial revision

*/


/* concatenates a set of fit files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "cnv_time.h"
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "fit_write.h"

#include "errstr.h"
#include "hlpstr.h"

struct fitdata fbuf[2];
struct fitdata *fptr;
int fnum=0;

double strdate(char *text) {
  double tme;
  int val;
  int yr,mo,dy;
  val=atoi(text);
  dy=val % 100;
  mo=(val / 100) % 100;
  yr=(val / 10000);
  if (yr<1970) yr+=1900;  
  tme=time_epoch(yr,mo,dy,0,0,0);

  return tme;
}

double strtime(char *text) {
  int hr,mn;
  int i;
  for (i=0;(text[i] !=':') && (text[i] !=0);i++);
  if (text[i]==0) return atoi(text)*3600L;
  text[i]=0;
  hr=atoi(text);
  mn=atoi(text+i+1);
  return hr*3600L+mn*60L;
}   
 

main (int argc,char *argv[]) {
  int i;
  int status=0;
  struct fitfp *fp=NULL;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  int irec=1;
  int drec=2;
  int c=1;
  int inx=0; 
  int dnum=0;
  int yr,mo,dy,hr,mt;
  double sc;
  int vb=0;

  double atime;
  double stime=-1;
  double etime=-1;
  double extime=0;
 
  double sdate=-1;
  double edate=-1;

  if (argc<3) {
    if ((argc>1) && (strcmp(argv[1],"--help")==0)) {
       print_info(stdout,hlpstr);
       exit(0);
    }
    print_info(stderr,errstr);
    exit(-1);
  } 

  if (strcmp(argv[1],"--help")==0) {
    print_info(stdout,hlpstr);
    exit(0);
  } else {
    for (c=1;c<argc-1;c++) { 
      if (strcmp(argv[c],"-i")==0) {
        inx=1;
      } else if (strcmp(argv[c],"-ex")==0) {
        extime=strtime(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-st")==0) {
        stime=strtime(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-et")==0) {
        etime=strtime(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-sd")==0) {
        sdate=strdate(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-ed")==0) {
        edate=strdate(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-vb")==0) vb=1;
      else break;
    }
  } 
      	  
   
   if ((argc-1-inx-c) >1)  fp=fit_open(argv[c],argv[c+1]);
   else fp=fit_open(argv[c],NULL);


   if (fp==NULL) {
     fprintf(stderr,"file %s not found\n",argv[c]);
     exit(-1);
   }

   fitfp=fopen(argv[argc-1-inx],"w");
   if (fitfp==NULL) {
     fprintf(stderr,"could not create fit file.\n");
     exit(-1);
   }
   fit_header_fwrite(fitfp,"trim_fit","fitacf","4.0");

   if (inx !=0) {
     inxfp=fopen(argv[argc-1],"w");
     if (inxfp==NULL) fprintf(stderr,"could not create index file.\n");
   }

   if (fit_read(fp,&fbuf[fnum])==-1) {
     fprintf(stderr,"Error reading file\n");
     exit(-1);
   }
   atime=time_epoch(fbuf[fnum].prms.YEAR,
		    fbuf[fnum].prms.MONTH,
                    fbuf[fnum].prms.DAY,
                    fbuf[fnum].prms.HOUR,
		    fbuf[fnum].prms.MINUT,
                    fbuf[fnum].prms.SEC);


   /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
   
      int yr,mo,dy,hr,mt;
      double sc;  
      
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;


      epoch_time(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=fit_seek(fp,yr,mo,dy,hr,mt,sc,NULL);
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }

      if (fit_read(fp,&fbuf[fnum])==-1) {
        fprintf(stderr,"Error reading file\n");
        exit(-1);
      }

    } else stime=atime;
   
    if (etime !=-1) {
       if (edate==-1) etime+=atime - ( (int) atime % (24*3600));
       else etime+=edate;
    }

    if (extime !=0) etime=stime+extime;

  
  do {
    fptr=&fbuf[fnum];
    fnum=(fnum+1) % 2;
    if ((inxfp !=NULL) && (irec==1)) fit_inx_header_fwrite(inxfp,fptr);
    dnum=fit_fwrite(fitfp,fptr);
    if (inxfp !=NULL) fit_inx_fwrite(inxfp,drec,dnum,fptr);
 
    drec+=dnum;
    irec++;
    atime=time_epoch(fbuf[fnum].prms.YEAR,
		    fbuf[fnum].prms.MONTH,
                    fbuf[fnum].prms.DAY,
                    fbuf[fnum].prms.HOUR,
		    fbuf[fnum].prms.MINUT,
                    fbuf[fnum].prms.SEC);
    epoch_time(atime,&yr,&mo,&dy,&hr,&mt,&sc);
    if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d %d\n",yr,mo,dy,hr,mt,(int) sc,fbuf[fnum].prms.BMNUM);
   

    if ((etime !=-1) && (atime>=etime)) break;
 
  } while (fit_read(fp,&fbuf[fnum]) !=-1);
  fit_close(fp);
  if (inxfp !=NULL) {
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    fit_inx_fclose(inxfp,fptr,irec-1);
    fclose(inxfp);
  }
  fclose(fitfp);

  return 0; 
} 






















