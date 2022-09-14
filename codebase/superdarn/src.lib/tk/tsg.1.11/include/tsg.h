/* tsg.h
   =====
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _TSG_H
#define _TSG_H

#define CLOCK_PERIOD	10		/* clock period in microseconds */

struct TSGbuf {
  unsigned char *code;
  unsigned char *rep;
  int len;
};

struct TSGprm {
  int nrang,          /* number of ranges */
      frang,          /* distance to first range */
      rsep,           /* range gate separation */
      smsep,          /* sample separation */
      lagfr,
      txpl,           /* length of pulse */
      mppul,          /* number of pulses in the sequence */
      mpinc,	      /* multi-pulse increment */
      mlag,           /* maximum lag in the sequence */
      nbaud,	      /* number of baud in the phase code */
      samples,	      /* number of samples generated by the sequence */
      smdelay,        /* sample delay */
      stdelay,        /* delay at front of sequence */
      gort;           /* gate or trigger with scope sync*/
  int rtoxmin;        /* delay between receiver off and pulse */
      int *pat,	      /* pointer to the pulse pattern */
	  *code;      /* pointer to the phase code table */
};

struct TSGtable {
  int num;
  int max;
  int *active;
  struct TSGprm *buf;
};

#define TSG_OK 0
#define TSG_INV_RSEP 1
#define TSG_NO_SMSEP 2
#define TSG_INV_MPPUL_SMSEP 3
#define TSG_INV_PAT 4
#define TSG_INV_MPINC_SMSEP 5
#define TSG_INV_LAGFR_SMSEP 6
#define TSG_INV_DUTY_CYCLE 7
#define TSG_INV_ODD_SMSEP 8
#define TSG_INV_TXPL_BAUD 9
#define TSG_INV_MEMORY 10
#define TSG_INV_PHASE_DELAY 11

#endif
