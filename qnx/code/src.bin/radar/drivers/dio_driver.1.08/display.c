/* display.c
   =========
   Author: R.J.Barnes
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

/*
 $Log: display.c,v $
 Revision 1.27  2001/06/27 20:55:38  barnes
 Added license tag

 Revision 1.26  2001/01/29 18:12:06  barnes
 Added Author Name

 Revision 1.25  2000/11/21 18:25:56  barnes
 Fixed bug in displaying the gain in stereo mode.

 Revision 1.24  2000/11/14 16:02:05  barnes
 Numerouse bug fixes to ensure the the local/remote condition of the
 synthesizer is correctly reported.

 Revision 1.23  2000/03/19 20:03:46  barnes
 Modifications to show both pulse sequences as they are transmitted

 Revision 1.22  1999/08/09 01:52:18  barnes
 Fixed bug in the low power and agc status flags.

 Revision 1.21  1999/04/14 14:58:50  barnes
 Modifications to prevent deadlock if we lose the clock pulse.

 Revision 1.20  1998/11/07 00:19:16  barnes
 Extensive changes for stereo operations.
 Moved all of the hard coded configuration information into a configuration
 file.

 Revision 1.19  1998/11/06 01:19:57  barnes
 Implemented the majority of the Stereo Radar code. The remaining sections
 involve selecting the correct PIO port based on the channel number and the
 implementation of the channel switching timing sequence.

 Revision 1.18  1998/10/28 22:07:50  barnes
 Changed the task name to "dio_driver"

 Revision 1.17  1998/10/23 17:39:58  barnes
 Modifications to support the extended timing sequence.

 Revision 1.16  1998/09/11 17:33:25  root
 Modifications to remove short integer types.

 Revision 1.15  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.14  1997/08/12 18:10:12  root
 Modified code for halley as they now use the leicester Rx design.

 Revision 1.13  1997/08/12 17:54:08  root
 Added #if to mask out filter switching on sites that don't have it.

 Revision 1.12  1997/08/11 15:07:23  root
 Tidied up the display of the filter mode.

 Revision 1.11  1997/08/07 16:12:48  root
 Added code for filter switch

 Revision 1.10  1997/06/10 20:17:13  root
 Moved sd_radar_id.h header.

 Revision 1.9  1997/04/16 10:59:46  root
 Debug mode can now be set on the command line.

 Revision 1.8  1997/02/19 17:07:40  root
 Fixed bug in Sanae code.

 * Revision 1.7  1997/02/19  17:05:45  root
 * Added code for Sanae
 *
 * Revision 1.6  1996/05/31  14:16:03  root
 * Added ability to set task name on the command line.
 *
 * Revision 1.5  1996/03/27  20:13:23  root
 * Made attenuation settings legible.
 *
 * Revision 1.4  1996/03/26  19:47:30  root
 * Added code for fclr
 *
 * Revision 1.3  1996/03/18  11:57:57  root
 * Removed #ifdef LEICESTER directive and replaced with SD_st_id
 * form.
 *
 * Revision 1.2  1996/03/14  10:13:41  root
 * Added DEBUG code.
 *
 * Revision 1.1  1996/03/13  15:13:31  root
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/qnxterm.h>

#include "version.h"
#include "main.h"
#include "dio_msg.h"
#include "port.h"

#define STAT_THRESH 3

extern int debug;
extern struct dio_hdw hdw;
extern struct dio_param param;
extern int AGC_stat_array[2][16][16];
extern int LOPWR_stat_array[2][16][16];
extern int stat_number[2][16][16];


/* String to hold the scrolling command bar */

extern char dio_name[];

char command[17]="----------------";
int count=0;

void draw_display(int ticker) {

  /* draw the background to the display */
  int i;
  char ver_str[16];

  if (debug !=0) {
    sprintf(ver_str,"%s debug",VSTRING);
  } else {
    sprintf(ver_str,"%s",VSTRING);
  }

 term_box(1,0,80,22,TERM_WHITE);

 term_type(2,24-strlen("Configuration :"),"Configuration :",0,
 		   TERM_WHITE | TERM_HILIGHT);
 term_type(2,25,hdw.name,0,TERM_CYAN | TERM_HILIGHT);


 term_type(4,24-strlen("Registered Name :"),"Registered Name :",0,
 		   TERM_WHITE | TERM_HILIGHT);
 term_type(4,25,dio_name,0,TERM_CYAN | TERM_HILIGHT);
 
 term_type(5,24-strlen("Version :"),"Version :",0,
 		   TERM_WHITE | TERM_HILIGHT);
 term_type(5,25,ver_str,0,TERM_CYAN | TERM_HILIGHT);
 if (param.stereo==1) {
    term_type(4,56-strlen("Stereo Channel :"),"Stereo Channel :",0,
           TERM_WHITE | TERM_HILIGHT);
    term_type(8,4,"Channel A:",0,TERM_RED | TERM_HILIGHT);
    term_type(14,4,"Channel B:",0,TERM_RED | TERM_HILIGHT);

  }

 for (i=0;i<=param.stereo;i++) {
   term_type(9+6*i,24-strlen("Freqency :"),"Freqency :",0,
 		   TERM_WHITE | TERM_HILIGHT);
   term_type(10+6*i,24-strlen("Beam :"),"Beam :",0,
 		   TERM_WHITE | TERM_HILIGHT);
   term_type(11+6*i,24-strlen("Antenna Mode :"),"Antenna Mode :",0,
 		   TERM_WHITE | TERM_HILIGHT);

   term_type(9+6*i,56-strlen("AGC status :"),"AGC status :",0,
 		   TERM_WHITE | TERM_HILIGHT);
   term_type(10+6*i,56-strlen("LOPWR status :"),"LOPWR status :",0,
 		   TERM_WHITE | TERM_HILIGHT);		   
   term_type(11+6*i,56-strlen("Attenuation :"),"Attenuation :",0,
 		   TERM_WHITE | TERM_HILIGHT);
   term_type(12+6*i,56-strlen("Test Mode :"),"Test Mode :",0,
 		   TERM_WHITE | TERM_HILIGHT);
   if ((hdw.FILTER_port[0] !=-1) || (hdw.FILTER_port[1] !=-1)) 
      term_type(12+6*i,24-strlen("Filter Mode :"),"Filter Mode :",0,
           TERM_WHITE | TERM_HILIGHT);
   term_type(13+6*i,24-strlen("Synthesizer Status :"),"Synthesizer Status :",0,
 		   TERM_WHITE | TERM_HILIGHT);

  }
   
  term_type(5,56-strlen("Sequence No. :"),"Sequence No. :",0,
 		   TERM_WHITE | TERM_HILIGHT);		   
  term_type(6,56-strlen("Sequence Id. :"),"Sequence Id. :",0,
 		   TERM_WHITE | TERM_HILIGHT);		   		   		   		   
  term_type(7,56-strlen("Sequence Stat. :"),"Sequence Stat. :",0,
 		   TERM_WHITE | TERM_HILIGHT);		   		   		   		   

  if (ticker==1)
  term_type(20,40-strlen("Command :"),"Command :",0,
 		   TERM_WHITE | TERM_HILIGHT);

}


void show_values() {

  /* show the current parameter values */

  char str[32];
  int i,j;
  int hilit=0;
  for (j=0;j<=param.stereo;j++) {
    if (param.channel==j) hilit=TERM_HILIGHT;
    else hilit=0;
    term_type(9+6*j,25,"        ",0,TERM_CYAN | hilit);
    sprintf(str,"%d",param.freq[j]);  
    term_type(9+6*j,25,str,0,TERM_CYAN | hilit);
    sprintf(str,"%d",param.beam[j]);  
    term_type(10+6*j,25,"    ",0,TERM_CYAN | hilit);
    term_type(10+6*j,25,str,0,TERM_CYAN | hilit);
    
    if (param.antenna_mode[j] == ANTENNA_AUTO) sprintf(str,"AUTO");
    else if (param.antenna_mode[j] == ANTENNA_BEAM) 
	  sprintf(str,"BEAM");
    else sprintf(str,"TEST (%2d)",param.antenna[j]);
  	   
    term_type(11+6*j,25,"           ",0,TERM_CYAN | hilit);
    term_type(11+6*j,25,str,0,TERM_CYAN | hilit);

    if (hdw.Atten_max >3) {
      sprintf(str,"00000000");
      for (i=0;i<param.gain[j];i++) str[7-i]='1';
    } else {
      switch (param.gain[j]) {
        case 0 : sprintf(str,"000");
	      break;
        case 1 : sprintf(str,"001");
	      break;
        case 2 : sprintf(str,"011");
	      break;
        default : sprintf(str,"111");
	      break;
      }
    }
		
    term_type(11+6*j,57,str,0,TERM_RED | hilit);
    if ((hdw.FILTER_port[0] !=-1) || (hdw.FILTER_port[1] !=-1)) {
      if (param.filter[j] !=0) sprintf(str,"WIDE BAND   ");
      else sprintf(str,"NARROW BAND");
      term_type(12+6*j,25,str,0,TERM_CYAN | hilit);
    }
    if (param.test_mode[j] !=0) sprintf(str,"ON ");
      else sprintf(str,"OFF");
    term_type(12+6*j,57,str,0,TERM_CYAN | hilit);

    if (param.fstatus[j] ==0) sprintf(str,"REMOTE");
      else sprintf(str,"LOCAL ");
    term_type(13+6*j,25,str,0,TERM_CYAN | hilit);

  }

  if (param.stereo==1) {
     if (param.channel==0) sprintf(str,"A");
     else sprintf(str,"B");
     term_type(4,57,str,0,TERM_CYAN | TERM_HILIGHT);
  }

  
  term_flush();
}

void show_command(char msg) {
  
  /* show the last command to be executed */

  char str[20];
  term_type(20,41,"                     ",0,TERM_WHITE | TERM_HILIGHT);
  switch (msg) {
    case SET_GAIN:
	    sprintf(str,"SET_GAIN"); 
		break;
	case SET_FREQ:
	    sprintf(str,"SET_FREQ");
		break;
    case SET_BEAM:
	    sprintf(str,"SET_BEAM");    
		break;
    case SET_FILTER:
	    sprintf(str,"SET_FILTER");    
		break;
    case READ_GAIN:
	    sprintf(str,"READ_GAIN");
		break;
	case READ_FREQ:
	    sprintf(str,"READ_FREQ");
		break;
	case READ_BEAM:
	    sprintf(str,"READ_BEAM");
		break;
	case READ_FILTER:
	    sprintf(str,"READ_FILTER");
		break;
	case RESET_XT:
	    sprintf(str,"RESET_XT");
	    break;
	case SET_ANTENNA:
	    sprintf(str,"SET_ANTENNA");
	    break;
	case TEST_MODE:
	    sprintf(str,"TEST_MODE");
	    break;	
	case SET_TSG:
	    sprintf(str,"SET_TSG");
	    break;	
	case SEND_TSG:
	    sprintf(str,"SEND_TSG");
	    break;	 
    case SEND_FCLR:
        sprintf(str,"SEND_FCLR");
        break;	   
	case VALID_ID:
	    sprintf(str,"VALID_ID");
		break;
	case GET_FREQ_MODE:
	    sprintf(str,"GET_FREQ_MODE");
		break;
	case GET_STATUS_NOFULL_NOCLR:
	    sprintf(str,"GET_STATUS (--)");
		break;
	case GET_STATUS_FULL_NOCLR:
	    sprintf(str,"GET_STATUS (f-)");
		break;
	case GET_STATUS_NOFULL_CLR:
	    sprintf(str,"GET_STATUS (-c)");
		break;
	case GET_STATUS_FULL_CLR:
	    sprintf(str,"GET_STATUS (fc)");
		break;		
    case SET_CHANNEL:
        sprintf(str,"SET_CHANNEL");
        break;
    case READ_CHANNEL:
        sprintf(str,"READ_CHANNEL");
        break;
    default :
        sprintf(str,"Unknown (%c)",msg);
        break;  
  }
  

  /* Draw the scrolling command bar */

  term_type(20,41,str,0,TERM_GREEN | TERM_HILIGHT);
  
  if (count < 16) command[count]=msg;
  else {
    memmove(command,command+1,15);
    command[15]=msg;
  }	
  count++;
  if (count>15) count=16;  	

  term_type(21,32,command,0,TERM_GREEN);
  term_flush();	
}

long int simplify_stat(int stat_array[16][16],
					   int tot_array[16][16]) {

  /* Reduce the status array to a single long integer */

  int nn[16], ss[16], stat;
  int i, j;

  for (i=0; i < 16; i++) {
	nn[i] = 0;
	ss[i] = 0;
	for (j=0; j < 16; j++) {
      nn[i] += tot_array[j][i];
	  ss[i] += stat_array[j][i];
	}
  }
	
  for (i=0; i <16; i++) {
	if ( 10*ss[i] > STAT_THRESH*nn[i] ) ss[i] = 1; 
	   else ss[i] = 0;
  }
		
  stat = 0;
  for (i=15; i > -1; --i) {
	stat = stat << 1;
	stat = stat + ss[i];
  }	
  return stat;
}

void display_status() {

  /* Display the status information */

  char str[32];
  int i,j;
  int hilit;
  int AGCstat_bits,LOPWRstat_bits;
  sprintf(str,"%d",param.seq_no);
  term_type(5,57,"      ",0,TERM_CYAN | TERM_HILIGHT);
  term_type(5,57,str,0,TERM_CYAN | TERM_HILIGHT);
  if (param.stereo !=0) {
     char tmp[16];
     if (param.seq_id[0] !=-1) sprintf(str,"%d,",param.seq_id[0]);
     else sprintf(str,"*,");
     if (param.seq_id[1] !=-1) sprintf(tmp,"%d",param.seq_id[1]);
     else sprintf(tmp,"*");
     strcat(str,tmp);  
  } else
     sprintf(str,"%d",param.seq_id[0]);

  term_type(6,57,"   ",0,TERM_CYAN | TERM_HILIGHT);
  term_type(6,57,str,0,TERM_CYAN | TERM_HILIGHT);
  if (param.status==0) sprintf(str,"Ok");
  else sprintf(str,"Fail");
  term_type(7,57,"     ",0,TERM_CYAN | TERM_HILIGHT);
  term_type(7,57,str,0,TERM_CYAN | TERM_HILIGHT);


  for (j=0;j<=param.stereo;j++) {
    if (param.channel==j) hilit=TERM_HILIGHT;
    else hilit=0;
    AGCstat_bits=simplify_stat(AGC_stat_array[j],stat_number);
    LOPWRstat_bits=simplify_stat(LOPWR_stat_array[j],stat_number);
  
    sprintf(str,"----------------");

    for (i=0;i<16;i++) {
      if ((AGCstat_bits & 1) !=0) str[i]='*';
	  AGCstat_bits=AGCstat_bits>>1;
    }
  
  
    term_type(9+6*j,57,str,0,TERM_YELLOW | hilit);
    sprintf(str,"----------------");  

    for (i=0;i<16;i++) {
      if ((LOPWRstat_bits & 1) !=0) str[i]='*';
	  LOPWRstat_bits=LOPWRstat_bits>>1;
    }

  
    term_type(10+6*j,57,str,0,TERM_YELLOW | hilit);
  }  
  term_flush();
}
