/* test_reopen.c
   ============= */

/*
 $Log: test_reopen.c,v $
 Revision 1.2  1999/04/08 18:28:57  barnes
 Removed debuggin code.

 Revision 1.1  1999/03/31 19:58:35  barnes
 Initial revision

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "limit.h"
#include "global.h"

static int then=-1;


int test_reopen(int hbnd,int mtbnd,int scbnd) {

  int bndsec;
  int nowsec;
  int now;

  bndsec=hbnd*3600+60*mtbnd+scbnd;
  nowsec=hour*3600+60*minut+sec;
  
  now=nowsec/bndsec;
  if (then==-1) then=now;
  if (then !=now) {
    then=now;
    return 1;
  }
  return 0;
}

 