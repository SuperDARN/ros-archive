/* watchdog.c
  =========== */

/*  set of routines and task to kick the watchdog card
 *
 * Expects to use the AR-B7010 watchdog card

 * First written 13 July 1995 by J D Thornhill

 * Note: any task using these functions must be linked at privity level 1 
 * since hardware is being accessed
 *
 */

/*
 $Log: watchdog.c,v $
 Revision 1.3  1998/11/07 00:19:16  barnes
 Extensive changes for stereo operations.
 Moved all of the hard coded configuration information into a configuration
 file.

 Revision 1.2  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.1  1997/01/23 22:01:42  root
 Initial revision

 */


#include <conio.h>
#include "watchdog.h"

/**************************************************************************/
/* Kicks the watchdog. After a hardware reset the watchdog is disabled    */
/* and won't timeout. After this function has been called the first time  */
/* after reset the watchdog must be written to (kicked) at least every    */
/* timeout seconds otherwise a reset will be generated.					  */
/* Note: the value of time out can be changed between calls.			  */
/*																		  */
/* int timeout can have the following values in seconds:				  */
/* 3, 6, 12, 18, 24, 30, 36, 42 - any other values will cause timeout to  */
/* be set to 42.														  */
/**************************************************************************/

int watch_port=0;

void kick_watchdog(int watchdog_port,int timeout){
	int arm_byte;
    if (watchdog_port==0) return;
	switch(timeout){
		case 3:
			arm_byte=0x80;	//3 seconds
		case 6:
			arm_byte=0x81;	//6 seconds
		case 12:
			arm_byte=0x82;	//12 seconds
		case 18:
			arm_byte=0x83;	//18 seconds
		case 24:
			arm_byte=0x84;	//24 seconds
		case 30:
			arm_byte=0x85;	//30 seconds
		case 36:
			arm_byte=0x86;	//36 seconds
		case 42:
			arm_byte=0x87;	//42 seconds
			break; 	//value of timeout ok
		default:
			arm_byte=0x87;	//42 seconds
			break;
		}
	outp(watchdog_port,arm_byte);
	}


/**************************************************************************/
/* Disables the watchdog, preventing it from timing out and generating a  */
/* reset.																  */
/**************************************************************************/
void disable_watchdog(int watchdog_port) {
  if (watchdog_port==0) return;
  outp(watchdog_port,0);
}
