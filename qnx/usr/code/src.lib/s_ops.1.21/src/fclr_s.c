/* fclr_s.c
   ======== */

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
 $Log: fclr_s.c,v $
 Revision 1.4  2002/03/26 16:33:43  barnes
 Fixed bug in naming of variables.

 Revision 1.3  2002/03/25 14:30:02  barnes
 Fixed bug of passing int pointer to do_fclr_s rather than float.

 Revision 1.2  2001/06/27 21:10:46  barnes
 Added license tag

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
  float fnoiseA,fnoiseB;

  *tfreqA=-1;
  *tfreqB=-1;
  
  do_fclr_s(tsg_table,fclr_id,debug,
            bufadr_s,bufsze_s,frq_table,
            dio_id_s,ad_id_s,reset_id_s,
            &tick,delay_s,

            start_freqA,end_freqA,step_freqA,
            mxpwr_A,max_atten_A,&atten_A,&fnoiseA,&noise_stat_A,&flgA,tfreqA,
            start_freqB,end_freqB,step_freqB,
            mxpwr_B,max_atten_B,&atten_B,&fnoiseB,&noise_stat_B,&flgB,tfreqB);

  noise_A=fnoiseA;
  noise_B=fnoiseB;	            
}
