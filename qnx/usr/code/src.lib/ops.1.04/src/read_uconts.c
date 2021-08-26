/* read_uconts.c 
   ============= */

/* 
 $Log: read_uconts.c,v $
 Revision 1.1  1999/03/31 19:58:35  barnes
 Initial revision

 */

#include <sys/name.h>
#include <sys/proxy.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

void read_uconts(pid_t proxy) {
  if (proxy != -1) Trigger(proxy);
}	


