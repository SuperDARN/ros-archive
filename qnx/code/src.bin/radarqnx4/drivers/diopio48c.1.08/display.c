/* display.c
   =========
   Author: R.J.Barnes
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/qnxterm.h>

#include "version.h"
#include "main.h"
#include "dio_msg.h"
#include "port.h"

/*
 $Log: display.c,v $
 Revision 1.5  2005/06/16 15:03:35  barnes
 Modification to handle phase steered radars.

 Revision 1.4  2004/11/12 15:13:13  barnes
 Modifications to handle digital phased radars.

 Revision 1.3  2004/06/22 12:25:08  barnes
 QNX4 code audit.

 Revision 1.2  2004/05/11 14:14:42  barnes
 Changed version handling.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

 */


#define STAT_THRESH 3

extern char silent;
extern char debug;
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

  if (silent !=0) return;

  if (debug !=0) {
    sprintf(ver_str,"%d.%.2d debug",MAJOR_VERSION,MINOR_VERSION);
  } else {
    sprintf(ver_str,"%d.%.2d",MAJOR_VERSION,MINOR_VERSION);
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
   if (param.phased==0) {
     term_type(10+6*i,24-strlen("Beam :"),"Beam :",0,
   		     TERM_WHITE | TERM_HILIGHT);
     term_type(11+6*i,24-strlen("Antenna Mode :"),"Antenna Mode :",0,
 		     TERM_WHITE | TERM_HILIGHT);
   } else {
     term_type(10+6*i,24-strlen("Phase :"),"Phase :",0,
              TERM_WHITE | TERM_HILIGHT);
   }
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

  if (silent !=0) return;
  for (j=0;j<=param.stereo;j++) {
    if (param.channel==j) hilit=TERM_HILIGHT;
    else hilit=0;
    term_type(9+6*j,25,"        ",0,TERM_CYAN | hilit);
    sprintf(str,"%d",param.freq[j]);  
    term_type(9+6*j,25,str,0,TERM_CYAN | hilit);

    if (param.phased==0) {
      sprintf(str,"%d",param.beam[j]);  
      term_type(10+6*j,25,"    ",0,TERM_CYAN | hilit);
      term_type(10+6*j,25,str,0,TERM_CYAN | hilit);
     
      if (param.antenna_mode[j] == ANTENNA_AUTO) sprintf(str,"AUTO");
      else if (param.antenna_mode[j] == ANTENNA_BEAM) 
	    sprintf(str,"BEAM");
      else sprintf(str,"TEST (%2d)",param.antenna[j]);
  	   
      term_type(11+6*j,25,"           ",0,TERM_CYAN | hilit);
      term_type(11+6*j,25,str,0,TERM_CYAN | hilit);
    } else {
      sprintf(str,"%d (%d)",param.phase[j],param.beam[j]);
      term_type(10+6*j,25,"           ",0,TERM_CYAN | hilit);
      term_type(10+6*j,25,str,0,TERM_CYAN | hilit);
    }     
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
  if (silent !=0) return;
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
    case READ_PHASE:
        sprintf(str,"READ_PHASE");
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
    case SET_PHASE:
        sprintf(str,"SET_PHASE");
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

  if (silent !=0) return;
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
