/* site.c
   ====== */


/* 
 $Log: site.c,v $
 Revision 1.1  1999/09/17 02:45:03  barnes
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

int wibble=0;

extern struct vtable var_table[];
extern struct optstr cmdlne;

void start_program() {

  /* insert stuff to be done when the program first starts here */

  

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
