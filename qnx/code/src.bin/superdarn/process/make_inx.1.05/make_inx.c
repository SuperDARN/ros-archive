/* make_inx.c
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
 $Log: make_inx.c,v $
 Revision 1.6  2001/07/24 12:55:11  barnes
 Modification to allow file to be read from standard input.

 Revision 1.5  2001/06/27 21:07:17  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.3  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.2  1999/05/19 17:34:41  barnes
 Fixed bug that meant that the last record in the index file was corrupt.

 Revision 1.1  1999/04/11 17:04:13  barnes
 Initial revision

*/


/* generates an index for a set of fit files*/

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
  FILE *inxfp=NULL;

  int irec=1;
  int drec=2;
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
  } 
       	  
  inxfp=fopen(argv[argc-1],"w");
  if (inxfp==NULL) {
    fprintf(stderr,"could not create index file.\n");
    exit(-1);  
  }

  if (strcmp(argv[argc-2],"stdin")==0) fp=fit_stream(stdin);
  else fp=fit_open(argv[argc-2],NULL); 
  if (fp==NULL) {
    fprintf(stderr,"file %s not found\n",argv[i]);
    exit(-1);
  }

 
  while (fit_read(fp,&fbuf[fnum]) !=-1) {
    fptr=&fbuf[fnum];
    fnum=(fnum+1) % 2;
    if (irec==1) fit_inx_header_fwrite(inxfp,fptr);
    dnum=fit_fwrite(NULL,fptr);
    fit_inx_fwrite(inxfp,drec,dnum,fptr);
    drec+=dnum;
    irec++;
  } 
  fit_close(fp);
  
  fclose(inxfp);
  inxfp=fopen(argv[argc-1],"r+");
  fit_inx_fclose(inxfp,fptr,irec-1);
  fclose(inxfp);

  return 0;
} 






















