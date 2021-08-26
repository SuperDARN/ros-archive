/* make_cmp.c
   ========== */


/* 
 $Log: make_cmp.c,v $
 Revision 1.12  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.11  2000/02/16 20:16:48  barnes
 Changed upper limits to prevent saturation

 Revision 1.10  1999/05/15 14:36:38  barnes
 Fixed bug in decoding command line arguments.

 Revision 1.9  1999/04/10 19:07:01  barnes
 Added help message system.

 Revision 1.8  1999/03/18 14:45:16  barnes
 Modifications to the internals of the cmp file.

 Revision 1.7  1999/03/16 19:56:21  barnes
 Fixed bug in decoding command line parameters.

 Revision 1.6  1999/03/16 18:08:32  barnes
 Modifications to use the new fit and cmp libraries.

 Revision 1.5  1999/01/27 16:06:09  barnes
 Modifications to cope with variations in frang and rsep.

 Revision 1.4  1998/11/09 22:55:19  barnes
 Incorporated rawdata header.

 Revision 1.3  1998/07/16 12:20:11  barnes
 Added the ability to concatenate multiple files.

 Revision 1.2  1998/06/18 18:00:52  barnes
 Changed the power specification to -p and the threshold to -t.

 Revision 1.1  1998/06/18 15:06:22  barnes
 Initial revision

 */

/* This task is designed to compress a fit file*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "cmpdata.h"

#include "fit_read.h"
#include "fit_cmp.h"
#include "cmp_write.h"

#include "errstr.h"
#include "hlpstr.h"

struct fitdata fit;

struct cmpdata cmp;
struct cmphdr cmp_hdr;


main (int argc,char *argv[]) {
  int i;
  struct fitfp *fp=NULL;
  double min_pwr=3.0;
  int sflag=0x08;
  int bmnum=-1;
  int fc=0,c;
  int verbose=0;
  char prm[8]={""};
  int rnum=0;  
  int rflg=0;
  int help=0;
 
  double pmax=60.0,vmax=2000.0,wmax=1000.0;

  
  if (argc>1) {
    int c;
    for (c=1;c<argc;c++) { 
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-a")==0) sflag |=0x07;
      else if (strcmp(argv[c],"-vb")==0) verbose=1; 
      else if (strcmp(argv[c],"-p")==0) {
        sflag=sflag | 0x01;
        pmax=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-v")==0) {
        sflag=sflag | 0x02;    
        vmax=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-w")==0) {
        sflag=sflag | 0x04;
        wmax=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-t")==0) {
        min_pwr=atof(argv[c+1]);
        c++;
        if (min_pwr<0) min_pwr=0;
        if (min_pwr>30) min_pwr=30;
      } else if (strcmp(argv[c],"-b")==0) {
        bmnum=atoi(argv[c+1]);
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

  if (sflag & 0x01) strcpy(prm,"p");
  if (sflag & 0x02) strcat(prm,"v");
  if (sflag & 0x03) strcat(prm,"w");


  for (c=fc;c<argc;c++) {
    fp=fit_open(argv[c],NULL); 
    fprintf(stderr,"Opening file %s\n",argv[c]);
    if (fp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[c]);
      continue;
    }
 
    while (fit_read(fp,&fit) !=-1) {

      if (rnum==0) {
        fit_cmphdr(pmax,vmax,wmax,&fit,&cmp_hdr);
        cmp_header_fwrite(stdout,&cmp_hdr);
        cmp.frang=cmp_hdr.frang;
        cmp.rsep=cmp_hdr.rsep;
    
      }

      if ((bmnum !=-1) && (fit.prms.BMNUM != bmnum)) continue; 
      if (verbose) fprintf(stderr,"%d-%d-%d %d:%d:%d beam=%d recording:%s\n",
	  		   fit.prms.DAY,
			   fit.prms.MONTH,fit.prms.YEAR,fit.prms.HOUR,
			   fit.prms.MINUT,fit.prms.SEC,fit.prms.BMNUM,prm);
      if (bmnum==-1) rflg=fit_cmp(sflag,min_pwr,pmax,vmax,wmax,&fit,&cmp,0);
      else rflg=fit_cmp(sflag,min_pwr,pmax,vmax,wmax,&fit,&cmp,1);  
      cmp_fwrite(stdout,&cmp,1 | (rflg<<1));
      rnum++;
    } 
    fit_close(fp);
  }

  return 0;
} 






















