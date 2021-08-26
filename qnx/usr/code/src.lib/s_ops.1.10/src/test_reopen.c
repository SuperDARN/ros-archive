/* test_reopen.c
   ============= */

/*
 $Log: test_reopen.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "limit.h"
#include "global_s.h"

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

 
