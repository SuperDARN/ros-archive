/* cat_raw.c
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
 $Log: cat_raw.c,v $
 Revision 1.8  2001/06/27 21:06:49  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:11:56  barnes
 Added Author Name

 Revision 1.6  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.5  1999/04/10 19:11:03  barnes
 Fixed problem with printing the help message.

 Revision 1.4  1999/04/10 19:02:32  barnes
 Added help message system.

 Revision 1.3  1999/03/16 17:10:50  barnes
 Modifications to use the new raw library.

 Revision 1.2  1998/11/09 22:54:57  barnes
 Incorporated rawdata header.

 Revision 1.1  1998/07/15 18:37:03  barnes
 Initial revision

*/


/* concatenates a set of raw files*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include "print_info.h"
#include "rawdata.h"
#include "version.h"

#include "raw_read.h"
#include "raw_write.h"

#include "errstr.h"
#include "hlpstr.h"
struct rawdata raw;

main (int argc,char *argv[]) {
  int c=1,i;
  int recnum=0;
  int thresh=0;
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
  } else  if (strcmp(argv[1],"-i")==0) {
    thresh=atoi(argv[2]);
    c=3;
  }

  outfp=fopen(argv[argc-1],"w");
  if (outfp==NULL) { 
    fprintf(stderr,"Could not open output file.\n");
    exit(-1);
  }
  if (raw_header_fwrite("rawwrite",VSTRING,thresh,            
		       "concatenated with cat_raw",outfp) !=0) {
    fprintf(stderr,"Could not write header.\n");
    exit(-1);
  }
    
  for (i=c;i<argc-1;i++) {
    infp=raw_open(argv[i],NULL);
    if (infp==NULL) {
      fprintf(stderr,"Could not open file %s.\n",argv[i]);
      continue;
    }
    while (raw_read(infp,&raw) !=-1) {
      recnum++;
      raw_fwrite(outfp,"rawwrite",&raw,thresh,recnum);
    }    
    raw_close(infp);
  }
  fclose(outfp);

  return 0;
}






















