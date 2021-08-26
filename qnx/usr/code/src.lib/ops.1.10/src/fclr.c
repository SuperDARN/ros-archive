/* fclr.c
   ====== */

/*
 $Log: fclr.c,v $
 Revision 1.2  1999/10/08 17:25:46  barnes
 Added the ability to calculate some noise statistics.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 Revision 1.1  1999/03/25 20:04:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "limit.h"

#include "message.h"

#include "a_d_drive.h"
#include "dio.h"
#include "tsg.h"

#include "freq_lib.h"

#include "fclr_set.h"
#include "do_fclr.h"

#include "global.h"

struct freq_table *frq_table=NULL;
int fclr_id[2]={0,0};

extern clock_t tick;

extern pid_t dio_id;
extern pid_t ad_id;
extern pid_t reset_id;

extern int bufnum;
extern int bufsze;
extern void *bufadr[16];

int fclr(int debug,int start_freq,int end_freq,int step_freq) {

  int flg;
  return do_fclr(tsg_table,fclr_id,debug,start_freq,end_freq,step_freq,
		  bufadr[0],bufsze,mxpwr,max_atten,frq_table,dio_id,ad_id,reset_id,
		  &atten,&noise,&noise_stat,&tick,&flg);  
}

 