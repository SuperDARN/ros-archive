/* make_inx.c
   ========= */

/*
 $Log: make_inx.c,v $
 Revision 1.3  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.2  1999/05/19 17:34:41  barnes
 Fixed bug that meant that the last record in the index file was corrupt.

 Revision 1.1  1999/04/11 17:04:13  barnes
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

  fp=fit_open(argv[argc-2],NULL); 
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






















