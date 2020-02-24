/* trim_raw.c
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
 $Log: trim_raw.c,v $
 Revision 1.6  2001/06/27 21:07:57  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:57  barnes
 Added Author Name

 Revision 1.4  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.3  1999/11/02 13:51:27  barnes
 Added the ability to set the start and end dates.

 Revision 1.2  1999/08/12 13:42:37  barnes
 Added verbose option and fixed end time.

 Revision 1.1  1999/05/18 18:04:09  barnes
 Initial revision

*/


/* concatenates a set of raw files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "cnv_time.h"
#include "print_info.h"
#include "rawdata.h"
#include "version.h"

#include "raw_read.h"
#include "raw_write.h"

#include "errstr.h"
#include "hlpstr.h"
struct rawdata raw;


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
  return (double) hr*3600L+mn*60L;
}   
 
main (int argc,char *argv[]) {
  int c=1,i;
  int recnum=0;
  int thresh=0;
  int status=0;
  double atime;

  double stime=-1;
  double etime=-1;
  double extime=0;

  double sdate=-1;
  double edate=-1;

  int yr,mo,dy,hr,mt;
  double sc;
  int vb=0;


  struct rawfp *infp=NULL;
  FILE *outfp=NULL;
 
  if (argc<3) {
    if ((argc==2) && (strcmp(argv[1],"--help")==0)) {
      print_info(stdout,hlpstr);
      exit(0);
    }
    print_info(stderr,errstr);
    exit(-1);
  } 

  if (strcmp(argv[1],"--help")==0) {
     print_info(stdout,hlpstr);
     exit(0);
  } else  {
      for (c=1;c<argc-2;c++) { 
      if (strcmp(argv[c],"-t")==0) {
        thresh=atoi(argv[c+1]);
        c++;
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

  infp=raw_open(argv[argc-2],NULL);
  if (infp==NULL) {
    fprintf(stderr,"Could not open file %s.\n",argv[argc-2]);
    exit(-1);
  }

  outfp=fopen(argv[argc-1],"w");
  if (outfp==NULL) { 
    fprintf(stderr,"Could not open output file.\n");
    exit(-1);
  }
  if (raw_header_fwrite("rawwrite",VSTRING,thresh,            
		       "trimmed with trim_raw",outfp) !=0) {
    fprintf(stderr,"Could not write header.\n");
    exit(-1);
  }
    
  if (raw_read(infp,&raw)==-1)  {
     fprintf(stderr,"Error reading file\n");
     exit(-1);
   }

   atime=time_epoch(raw.PARMS.YEAR,
		    raw.PARMS.MONTH,
                    raw.PARMS.DAY,
                    raw.PARMS.HOUR,
		    raw.PARMS.MINUT,
                    raw.PARMS.SEC);

   /* skip here */

    if ((stime !=-1) || (sdate !=-1)) { 
      /* we must skip the start of the files */
      int yr,mo,dy,hr,mt;
      double sc; 
      if (stime==-1) stime= ( (int) atime % (24*3600));
      if (sdate==-1) stime+=atime - ( (int) atime % (24*3600));
      else stime+=sdate;

      epoch_time(stime,&yr,&mo,&dy,&hr,&mt,&sc);
      status=raw_seek(infp,yr,mo,dy,hr,mt,sc,NULL);
      if (status ==-1) {
        fprintf(stderr,"File does not contain the requested interval.\n");
        exit(-1);
      }

      if (raw_read(infp,&raw)==-1)  {
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
    recnum++;
    raw_fwrite(outfp,"rawwrite",&raw,thresh,recnum);
    atime=time_epoch(raw.PARMS.YEAR,
		    raw.PARMS.MONTH,
                    raw.PARMS.DAY,
                    raw.PARMS.HOUR,
		    raw.PARMS.MINUT,
                    raw.PARMS.SEC);
   epoch_time(atime,&yr,&mo,&dy,&hr,&mt,&sc);
   if (vb==1) fprintf(stderr,"%d-%d-%d %d:%d:%d\n",yr,mo,dy,hr,mt,(int) sc);

    if ((etime !=-1) && (atime>=etime)) break;
  } while (raw_read(infp,&raw) !=-1);  

  raw_close(infp);
  fclose(outfp);
  return 0;


}






















