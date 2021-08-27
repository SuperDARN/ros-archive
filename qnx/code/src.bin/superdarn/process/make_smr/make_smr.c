/* make_smr.c
   ========== */


/* 
 $Log: make_smr.c,v $
 Revision 1.2  1999/04/10 19:16:17  barnes
 Added help message system.

 Revision 1.1  1999/03/18 16:35:12  barnes
 Initial revision

 */

/* This task is designed to make a summary file*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"

#include "fit_read.h"
#include "smr_write.h"

#include "errstr.h"
#include "hlpstr.h"

struct fitdata fit;

main (int argc,char *argv[]) {
  int i;
  int fc=0,c=1;
  char *bstr;
  struct fitfp *fp=NULL;
  double min_pwr=3.0;
  char bflg[16];
  int bnum=0,b;
  int verbose=0;
  int help=0;

  for (i=0;i<16;i++) bflg[i]=0;
  bnum=0;

  if (argc>1) {
    for (c=1;c<argc;c++) { 
      if (strcmp(argv[c],"--help")==0) help=1;
      if (strcmp(argv[c],"-vb")==0) verbose=1; 
      if (strcmp(argv[c],"-t")==0) {
        min_pwr=atof(argv[c+1]);
        c++;
        if (min_pwr<0) min_pwr=0;
        if (min_pwr>30) min_pwr=30;
      } else if (strcmp(argv[c],"-b")==0) {
        bstr=strtok(argv[c+1],",");
        do {
          b=atoi(bstr);
          if (bflg[b]==0) {
            bflg[b]=1;
            bnum++;
	  }
        } while ((bstr=strtok(NULL,",")) !=NULL);
        c++;
      } else if (fc==0) fc=c; 
    }
  } else {
    print_info(stderr,errstr);
    exit(-1);
  }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }

  if (bnum==0) {
     bflg[8]=1;
     bnum=1;
  }
  
  for (c=fc;c<argc;c++) {
    fp=fit_open(argv[c],NULL); 
    fprintf(stderr,"Opening file %s\n",argv[c]);
    if (fp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[c]);
      continue;
    }
 
    smr_header_fwrite(stdout,"1.00","make_smr","make_smr");
    

    while (fit_read(fp,&fit) !=-1) {

      if (bflg[fit.prms.BMNUM]==0) continue;
 
      if (verbose) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d\n",
	  		   fit.prms.DAY,
			   fit.prms.MONTH,fit.prms.YEAR,fit.prms.HOUR,
			   fit.prms.MINUT,fit.prms.SEC,fit.prms.BMNUM);
      smr_fwrite(stdout,&fit,min_pwr);            
    } 
    fit_close(fp);
  }
} 






















