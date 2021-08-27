/* make_fit.c
   ========== */

/*
 $Log: make_fit.c,v $
 Revision 1.7  2000/01/10 13:30:08  barnes
 Fixed bug in ordering of environment variables.

 Revision 1.6  1999/12/15 21:57:22  barnes
 Modification to use the distributed hardware files.

 Revision 1.5  1999/04/10 19:09:56  barnes
 Added help message system.

 Revision 1.4  1999/04/10 19:08:38  barnes
 Added help message system.

 Revision 1.3  1999/03/16 17:25:24  barnes
 Modification to use the raw and fit libraries.

 Revision 1.2  1998/11/09 22:56:21  barnes
 Incorporated rawdata header.

 Revision 1.1  1998/06/18 16:03:20  barnes
 Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "hardware.h"

#include "fitacf.h"
#include "raw_read.h"
#include "fit_write.h"
#include "hardware.h"
#include "radar_name.h"

#include "errstr.h"
#include "hlpstr.h"

struct rawdata raw;
struct fitdata fit;
struct fit_block fblk;
struct radar_site *hdblk;  
struct radar_hardware *hdw;
struct radar_id *rid;  

void main(int argc,char *argv[]) {
  char *envstr;
  int status;
  int verbose=0;
  struct rawfp *rawfp;
  FILE *fp;
  FILE *fitfp=NULL;
  FILE *inxfp=NULL;
  char tmp;
  int i;
  int irec=1;
  int drec=2;
  int dnum=0;
  int c=1;

  char *dname=NULL;
  char *fname=NULL;
  char *iname=NULL;


  envstr=getenv("SD_RADARNAME");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(-1);
  }

  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp); 

  envstr=getenv("SD_HARDWARE");
  if (envstr==NULL) {
    fprintf(stderr,"Environment variable 'SD_HARDWARE' must be defined.\n");
    exit(-1);
  }

 
  hdw=load_hdw(envstr,rid);
 


 

  if (argc<3) {
    if ((argc==2) && (strcmp(argv[1],"--help")==0)) {
      print_info(stdout,hlpstr);
      exit(-1);
    }
    print_info(stderr,errstr);
    exit(-1);
  } 
 
  if (strcmp(argv[1],"--help")==0) {
    print_info(stdout,hlpstr);
    exit(-1);
  }
 
  if (strcmp(argv[1],"-vb")==0) {
     verbose=1; 
     c=2;
  }
 
  dname=argv[c];
  fname=argv[c+1];
  if ((argc-c)>2) iname=argv[c+2];

  rawfp=raw_open(dname,NULL);
  if (rawfp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  raw_read(rawfp,&raw);
  hdblk=get_site(hdw,raw.PARMS.YEAR,raw.PARMS.MONTH,
         raw.PARMS.DAY,raw.PARMS.HOUR,raw.PARMS.MINUT,
            raw.PARMS.SEC,raw.PARMS.ST_ID);
  start_fit(hdblk,raw.PARMS.YEAR,&fblk);
 
  fitfp=fopen(fname,"w");
  if (iname !=NULL) inxfp=fopen(iname,"w");
  
  fitacf(&raw,&fblk,&fit);

  fit_header_fwrite(fitfp,"make_fit","fitacf","4.0");
  if (inxfp !=NULL) fit_inx_header_fwrite(inxfp,&fit);

  do {
    if (verbose) 
    fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",raw.PARMS.DAY,raw.PARMS.MONTH,
	    raw.PARMS.YEAR,raw.PARMS.HOUR,raw.PARMS.MINUT,raw.PARMS.SEC,
	    raw.PARMS.BMNUM);

    dnum=fit_fwrite(fitfp,&fit);
    if (inxfp !=NULL) fit_inx_fwrite(inxfp,drec,dnum,&fit);
    drec+=dnum;
    irec++;
    status=raw_read(rawfp,&raw);
    if (status==0) fitacf(&raw,&fblk,&fit); 
  } while (status==0);
  if (inxfp !=NULL) {
    fclose(inxfp);
    inxfp=fopen(iname,"r+");
    fit_inx_fclose(inxfp,&fit,irec-1);
    fclose(inxfp);
  }
  end_fit(&fblk);
  raw_close(rawfp);
  fclose(fitfp);
}













