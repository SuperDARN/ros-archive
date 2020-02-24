// For Iceland East Radar
// ======================

/* site.c
   ====== */


/* 
 $Log: site.c,v $
 Revision 1.1  2000/07/10 17:30:48  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/name.h>
#include <sys/proxy.h>
#include <sys/types.h>
#include <sys/kernel.h>
#include <ctype.h>

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

#define UCONT_NAME "ucont_moni"

void init_freq_bands(void);
//void init_proxy(void);
//void read_uconts(void);

extern struct vtable var_table[];
extern struct optstr cmdlne;

int lsfreq[42];
int lfreq_range[42];

static pid_t ucont_proxy;

void start_program(void)
{

/* insert stuff to be done when the program first starts here */

	backward = 0;

	start_beam = 0;
	end_beam = 15;

	xcount = 1;

	day_start_hr   = 8;
	night_start_hr = 18;

// For CUTLASS the variables day_start_freq and night_start_freq
// contain band numbers NOT frequencies.

	day_start_freq   = 27;
	night_start_freq = 23;

	freq_range = 300;
	max_atten = 7;

// set up Iceland East frequency bands

	init_freq_bands();	

// create a proxy

	ucont_proxy = init_proxy("ucont_moni");
}


int start_scan(void)
{

  /* insert stuff for start of scan here.
   * return 1 if you wish the scan to proceed or zero otherwise.
   */

  return 1;
}


void integrate(void)
{

	//For CUTLASS the variables day_start_freq and night_start_freq
	//are contain band numbers NOT frequencies.

	if(day_time()==1){	//day time
		start_freq=lsfreq[day_start_freq];
		freq_range=lfreq_range[day_start_freq];
		}
	else{				//night time
		start_freq=lsfreq[night_start_freq];
		freq_range=lfreq_range[night_start_freq];
		}

	read_uconts(ucont_proxy);	//get the microcontroller status
}


void end_program(void)
{

  /* insert stuff to be done at the end of the program here */

}


//===============================================================
//sets up the licenced frequency bands for Iceland

void init_freq_bands(void)
{

/* start freq of each band */
   lsfreq[20] = 8000;
   lsfreq[21] = 8430;
   lsfreq[22] = 8985;
   lsfreq[23] = 10155;
   lsfreq[24] = 10655;
   lsfreq[25] = 11290;
   lsfreq[26] = 11475;
   lsfreq[27] = 12105;
   lsfreq[28] = 12305;
   lsfreq[29] = 12590;
   lsfreq[30] = 13360;
   lsfreq[31] = 13875;
   lsfreq[32] = 14400;
   lsfreq[33] = 15805;
   lsfreq[34] = 16500;
   lsfreq[35] = 16820;
   lsfreq[36] = 18175;
   lsfreq[37] = 18835;
   lsfreq[38] = 19910;
   lsfreq[39] = 10155;

/* width of each band */

   lfreq_range[20] = 195;
   lfreq_range[21] = 420;
   lfreq_range[22] = 410;
   lfreq_range[23] = 500;
   lfreq_range[24] = 520;
   lfreq_range[25] = 160;
   lfreq_range[26] = 120;
   lfreq_range[27] = 130;
   lfreq_range[28] = 205;
   lfreq_range[29] = 690;
   lfreq_range[30] = 205;
   lfreq_range[31] = 120;
   lfreq_range[32] = 615;
   lfreq_range[33] = 560;
   lfreq_range[34] = 185;
   lfreq_range[35] = 655;
   lfreq_range[36] = 595;
   lfreq_range[37] = 50;
   lfreq_range[38] = 90;
   lfreq_range[39] = 1020;
}

// Creates a proxy that is triggered every beam

#if 0

void init_proxy()
{
	pid_t proxy_target_pid;
	extern struct task_id **etsk;
	extern char program_name[];

// locate leicester monitor task

  	proxy_target_pid = qnx_name_locate(0,UCONT_NAME,0,NULL); 
	if(proxy_target_pid == (-1)) {
	   perror("can't locate UCONT_MONITOR task!\n");
       exit(1);
	}

/* attach the proxy */
   	ucont_proxy = qnx_proxy_attach(proxy_target_pid,
                  "interp",strlen("interp"),-1); 
	if(ucont_proxy == -1) {
	   perror("unable to attach proxy\n");
	   log_error(etsk,program_name,"Unable to attach proxy");
		exit(1);
		}	
	}

#endif

//
//Sends a proxy message to the microcontroller monitoring task 
//every beam.

#if 0

void read_uconts(){
	if(ucont_proxy!=0) Trigger(ucont_proxy);
	}

#endif
