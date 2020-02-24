/* make_smr.c
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
 $Log: make_smr.c,v $
 Revision 1.7  2001/07/24 12:48:53  barnes
 Modification to take input from standard input.

 Revision 1.6  2001/06/27 21:07:22  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.4  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.3  2000/04/25 20:06:59  barnes
 Moved "opening file message" to verbose only mode.

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
    if (strcmp(argv[c],"stdin")==0) fp=fit_stream(stdin);
    else fp=fit_open(argv[c],NULL); 
    if (verbose) fprintf(stderr,"Opening file %s\n",argv[c]);
    if (fp==NULL) {
      if (verbose) fprintf(stderr,"file %s not found\n",argv[c]);
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

  return 0;
} 






















