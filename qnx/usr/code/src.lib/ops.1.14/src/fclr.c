/* fclr.c
   ====== */

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
 $Log: fclr.c,v $
 Revision 1.4  2001/10/15 16:06:26  barnes
 Moved to floating point internals.

 Revision 1.3  2001/06/27 21:10:39  barnes
 Added license tag

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
  int status;
  int flg;
  float fnoise;
  status=do_fclr(tsg_table,fclr_id,debug,start_freq,end_freq,step_freq,
		  bufadr[0],bufsze,mxpwr,max_atten,frq_table,dio_id,
                  ad_id,reset_id,
		  &atten,&fnoise,&noise_stat,&tick,&flg);  

  noise=fnoise;
  return status;
}

 
