/* normalize_baud.c 
   ================ */

/*
$Log: normalize_baud.c,v $
Revision 1.1  1999/03/30 23:55:58  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "limit.h"
#include "tsg.h"
#include "do_phase.h"

int normalize_baud(struct tsg_parms *prm,int *c_n) {
  
  int cd,k;
  for (cd=0;cd<prm->mppul;cd++) {
     c_n[cd]=0;
     for (k=0;k<prm->nbaud;k++) {
       c_n[cd]=c_n[cd]+prm->code[cd*prm->nbaud+k];
     }
     c_n[cd]=c_n[cd]*c_n[cd];
  }
  return 0;
}
