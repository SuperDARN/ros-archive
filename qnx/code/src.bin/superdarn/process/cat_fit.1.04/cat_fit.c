/* cat_fit.c
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
 $Log: cat_fit.c,v $
 Revision 1.8  2001/06/27 21:06:45  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.6  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.5  1999/04/10 19:12:05  barnes
 Fixed problem with help message.

 Revision 1.4  1999/04/10 19:01:16  barnes
 Added help message system.

 Revision 1.3  1999/03/16 15:12:28  barnes
 Modifications to support the new fit library.

 Revision 1.2  1998/11/09 22:54:25  barnes
 Incorporated rawdata header.

 Revision 1.1  1998/07/14 16:44:48  barnes
 Initial revision

*/


/* concatenates a set of fit files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
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
  } else if (strcmp(argv[1],"-i")==0) {
    inx=1;
    c=2;
  } 
       	  
  fitfp=fopen(argv[argc-1-inx],"w");
  if (fitfp==NULL) {
    fprintf(stderr,"could not create fit file.\n");
    exit(-1);
  }
  fit_header_fwrite(fitfp,"cat_fit","fitacf","4.0");

  if (inx !=0) {
    inxfp=fopen(argv[argc-1],"w");
    if (inxfp==NULL) fprintf(stderr,"could not create index file.\n");
  }
 
  for (i=c;i<argc-1-inx;i++) {
    fp=fit_open(argv[i],NULL); 
    if (fp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[i]);
      continue;
    }
    while (fit_read(fp,&fbuf[fnum]) !=-1) {
      fptr=&fbuf[fnum];
      fnum=(fnum+1) % 2;
      if ((inxfp !=NULL) && (irec==1)) fit_inx_header_fwrite(inxfp,fptr);
      dnum=fit_fwrite(fitfp,fptr);
      if (inxfp !=NULL) fit_inx_fwrite(inxfp,drec,dnum,fptr);
      drec+=dnum;
      irec++;
    } 
    fit_close(fp);
  }
  if (inxfp !=NULL) {
    fclose(inxfp);
    inxfp=fopen(argv[argc-1],"r+");
    fit_inx_fclose(inxfp,fptr,irec-1);
    fclose(inxfp);
  }
  fclose(fitfp);
  return 0;
} 






















