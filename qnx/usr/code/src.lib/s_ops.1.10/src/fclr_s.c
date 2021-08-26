/* fclr_s.c
   ======== */

/*
 $Log: fclr_s.c,v $
 Revision 1.1  2000/03/19 20:33:19  barnes
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
#include "do_fclr_s.h"

#include "global_s.h"

struct freq_table *frq_table=NULL;
int fclr_id[2]={0,0};

extern clock_t tick;

void fclr_s(int debug,
         int start_freqA,
         int end_freqA,
         int step_freqA,
         int start_freqB,
         int end_freqB,
         int step_freqB,
         int *tfreqA,
         int *tfreqB) {
 
  int flgA,flgB;

  *tfreqA=-1;
  *tfreqB=-1;
  
  do_fclr_s(tsg_table,fclr_id,debug,
            bufadr_s,bufsze_s,frq_table,
            dio_id_s,ad_id_s,reset_id_s,
            &tick,delay_s,

            start_freqA,end_freqA,step_freqA,
            mxpwr_A,max_atten_A,&atten_A,&noise_A,&noise_stat_A,&flgA,tfreqA,
            start_freqB,end_freqB,step_freqB,
            mxpwr_B,max_atten_B,&atten_B,&noise_B,&noise_stat_B,&flgB,tfreqB);

       
	            
}



 
