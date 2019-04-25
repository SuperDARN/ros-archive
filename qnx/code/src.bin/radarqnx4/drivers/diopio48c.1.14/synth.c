/* synth.c
   =======
   Author Julian Thornhill
*/


/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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


/* functions to send transmit frequency to the synth unit.
   There is a different funtion for the mono Hokkaido radar to
   the subsequent stereo radars such as Blackstone.
*/

#include <i86.h>
#include <conio.h>
#include <stdio.h>
#include <syslog.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/kernel.h>
#include <sys/types.h>
#include <sys/name.h>
#include <sys/sched.h>
#include <sys/qnxterm.h>
#include <unistd.h>
#include <sys/vc.h>

/*
 $Log: synth.c,v $
 Revision 1.4  2008/03/12 15:36:24  code
 The network node is now defined as a global variable.

 Revision 1.3  2007/11/13 18:18:43  code
 Removed redundant variable.

 Revision 1.2  2007/11/09 21:56:34  code
 Fixed unused variables.

 Revision 1.1  2007/11/09 16:12:05  code
 Initial revision

*/

#define CLOCK_MSG 33
#define PC104SYNTH_TASKNAME "/cutlass/pc104synth"

#define DEBUGTERM "/dev/con3"

int synthnode=4;


/* This structure used for mono Hokkaido radar */


struct command_clock_board {
  int message_id;	/* this should be CLOCK_MSG */
  int TX_freq;		/* TX freq in Hz, -1 no change */
};

/* This structure used for stereo radars with synth unit */

struct command_clock_board_stereo {
  int message_id;	/* this should be CLOCK_MSG */
  int TX_freq_A;        /* channel A transmit frequency */
  int TX_freq_B;  	/* channel B transmit frequency */
	};


/* This function for mono Hokkaido radar */


int txsynth(int freq) {

  int status;
  FILE *fp;
  pid_t pid;
  struct command_clock_board cmd,rcmd;


  /*  open debug terminal */

  fp=fopen(DEBUGTERM,"w");

  /* set up initial state of command block */

  cmd.message_id=CLOCK_MSG;
  cmd.TX_freq=-1;

  pid=qnx_vc_name_attach(synthnode,1024,PC104SYNTH_TASKNAME);
  if(pid==-1) {
    perror("Unable to locate pc104synth task:");
    return(-1);
  }


	
  cmd.TX_freq=freq*1000; /* change kHz to Hz; */

  fprintf(fp,"Frequency =%d kHz, cmd.TX_freq=%d Hz\n",freq,cmd.TX_freq);
  fclose(fp);
	
  status=Send(pid,&cmd,&rcmd,sizeof(cmd),sizeof(rcmd));
  if(status==-1){
    perror("Send failed: ");
    qnx_vc_detach(pid);
    return(-1);
  }
  qnx_vc_detach(pid);
  return(0);
}


/* This function for stereo radars */

int txsynth_s(int channel,int freq) {

  int status;
  FILE *fp;
  pid_t pid;
  struct command_clock_board_stereo cmd,rcmd;


  /* open debug terminal */

  fp=fopen(DEBUGTERM,"w");

  /* set up initial state of command block */

  cmd.message_id=CLOCK_MSG;
  cmd.TX_freq_A=-1; /* sending -1 means do not change frequency */
  cmd.TX_freq_B=-1;

  pid=qnx_vc_name_attach(synthnode,1024,PC104SYNTH_TASKNAME);
  if(pid==-1){
    perror("Unable to locate pc104synth task:");
    return(-1);
  }


  if(channel==0) {
    cmd.TX_freq_A=freq*1000; /* change kHz to Hz; */
  } else {
    cmd.TX_freq_B=freq*1000; /* change kHz to Hz; */
		}

  fprintf(fp,
   "Channel=%d Frequency =%d kHz, cmd.TX_freq_A=%d Hz, cmd.TX_freq_B=%d Hz\n",
   channel,freq,cmd.TX_freq_A,cmd.TX_freq_B);
  fclose(fp);
	
  status=Send(pid,&cmd,&rcmd,sizeof(cmd),sizeof(rcmd));
  if (status==-1) {
    perror("Send failed: ");
    qnx_vc_detach(pid);
    return(-1);
  }
  qnx_vc_detach(pid);
  return(0);
}

