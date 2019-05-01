/* watchdog.c
   ==========
   Author: J.D. Thornhill & R.J.Barnes
*/

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

#include <conio.h>
#include "watchdog.h"



/*
 $Log: watchdog.c,v $
 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

 */

/*  set of routines and task to kick the watchdog card
 *
 * Expects to use the AR-B7010 watchdog card
 *
 * First written 13 July 1995 by J D Thornhill
 *
 * Note: any task using these functions must be linked at privity level 1 
 * since hardware is being accessed
 *
 */


/**************************************************************************/
/* Kicks the watchdog. After a hardware reset the watchdog is disabled    */
/* and won't timeout. After this function has been called the first time  */
/* after reset the watchdog must be written to (kicked) at least every    */
/* timeout seconds otherwise a reset will be generated.	                  */
/* Note: the value of time out can be changed between calls.	          */
/*				        				  */
/* int timeout can have the following values in seconds:    		  */
/* 3, 6, 12, 18, 24, 30, 36, 42 - any other values will cause timeout to  */
/* be set to 42. 		                                          */
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
/* reset.	  						          */
/**************************************************************************/
void disable_watchdog(int watchdog_port) {
  if (watchdog_port==0) return;
  outp(watchdog_port,0);
}
