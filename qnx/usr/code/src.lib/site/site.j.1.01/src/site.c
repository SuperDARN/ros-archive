/* site.c
   ====== */


/* 
 $Log: site.c,v $
 Revision 1.1  2000/07/10 17:31:25  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_time.h"
#include "types.h"
#include "option.h"
#include "limit.h"
#include "rawdata.h"
#include "fitdata.h"

#include "hardware.h"
#include "radar_name.h"
#include "fitacf.h"
#include "raw_read.h"

#include "task_msg.h"
#include "radar_shell.h"

#include "global.h"

extern struct vtable var_table[];
extern struct optstr cmdlne;

void start_program() {

  /* insert stuff to be done when the program first starts here */
  backward=1;
  start_beam=15;
  end_beam=0;
  xcount=4;
  day_start_hr=9;
  night_start_hr=19;
  day_start_freq=12300;
  night_start_freq=12300;
  freq_range=300;
}


int start_scan() {

  /* insert stuff for start of scan here.
   * return 1 if you wish the scan to proceed or zero otherwise.
   */

  return 1;
}


void integrate() {

  /* insert stuff to be done before intergrating here */

}


void end_program() {

  /* insert stuff to be done at the end of the program here */

}
