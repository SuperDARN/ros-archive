/* cat_raw.c
   ========= */

/*
 $Log: cat_raw.c,v $
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






















