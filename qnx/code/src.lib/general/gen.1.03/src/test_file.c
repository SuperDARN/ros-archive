/* test_file.c
   =========== */

/*
 $Log: test_file.c,v $
 Revision 1.1  1998/10/28 16:06:16  barnes
 Initial revision


 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

int test_file(char *fname) {
 int fildes;
  int rc;
  struct stat buf;
  fildes=open(fname,O_RDONLY);
  if (fildes==-1) return 0;
  rc=fstat(fildes,&buf);
  close(fildes);
  if (rc ==-1) return 0;
  if (buf.st_size != 0) {
    return 1;
  }
  return 0;
}




