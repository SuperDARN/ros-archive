/* display.c
   ========= */

/*
 $Log: display.c,v $
 Revision 2.6  1999/03/29 15:59:50  barnes
 Fixed bug in plotting fitted data as the quality flag now contains
 extra information

 Revision 2.5  1998/11/09 23:15:05  barnes
 Incorporated rawdata header.

 Revision 2.4  1998/11/03 23:36:48  barnes
 Modification to cope for possible floating point error if the noise is zero.

 Revision 2.3  1998/10/29 22:04:16  barnes
 Removed references to short types.

 Revision 2.2  1998/06/11 16:20:40  root
 Use the new radar_name function and changed the task name to 'terminal'.

 Revision 2.1  1998/06/05 11:18:35  root
 Modifications to use the improved message passing mechanism.

 Revision 1.17  1997/08/12 18:12:40  root
 Added code to take account of new Rx design at Halley.

 Revision 1.16  1997/06/10 19:09:19  root
 Removed redundant headers.

 Revision 1.15  1997/04/17 17:20:41  root
 Moved under echo_data distribution system.

 Revision 1.14  1997/02/19 17:09:16  root
 Added code for Sanae.

 * Revision 1.13  1996/09/23  14:32:18  root
 * Made display task independant from sd_radar_id.h
 *
 * Revision 1.12  1996/04/16  14:36:52  root
 * Corrected problem with slow screen updates over a modem.
 *
 * Revision 1.11  1996/04/16  14:07:05  root
 * Major modifications to the colour bars, scaling and keyboard controls.
 *
 * Revision 1.10  1996/04/12  17:22:31  root
 * Added code to change priority.
 *
 * Revision 1.9  1996/04/12  16:45:17  root
 * Corrected bug that prevented the correct
 * radar parameters being displayed for the raw data.
 *
 * Revision 1.8  1996/04/03  12:36:13  root
 * Altered the parameters displayed on the screen.
 *
 * Revision 1.7  1996/04/03  08:48:07  root
 * Modified velocity display to show ground scatter.
 *
 * Revision 1.6  1996/03/29  17:33:32  root
 * Added support for display running on an Xterm.
 *
 * Revision 1.5  1996/03/27  20:11:44  root
 * Added station name to ACF display.
 *
 * Revision 1.4  1996/03/27  20:09:52  root
 * Added station name to display.
 *
 * Revision 1.3  1996/03/19  17:00:38  root
 * Fixed bug that meant the initial banner was wrong.
 *
 * Revision 1.2  1996/03/14  11:56:31  root
 * Debug error code.
 *
 * Revision 1.1  1996/03/13  16:18:43  root
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/qnxterm.h>
#include <time.h>

#include "echo_util.h"
#include "id.h"
#include "radar_name.h"

#include "rawdata.h"
#include "fitdata.h"
#define MAX_BEAM 16

#define ACF_NORM_FACTOR 8
#define MAX_LAG_NUMBER 70
#define MAX_POWER_SCALE 30

extern struct term_state term_state;

double thrhold=2.0;
double vel_scale=1000.0;
double pwr_scale=40.0;
double wdt_scale=400.0; 

extern term_flag;

/* define the colour scheme */

#include "colours.h"

int convert_snr(int power,int noise) {

   /* calculate S/N ratio */
   int p_over_n;
   int snr;

   if (noise !=0) p_over_n = power/noise;
   else p_over_n=0;
   p_over_n = p_over_n >> 1;
   snr = 0;
   while((p_over_n > 0) && (snr < (MAX_POWER_SCALE-1))) {
      p_over_n = p_over_n >> 1;
      snr++;
   } 
   return snr;
} 


void plot(int x,int y,int scale,double value,double max_val) {
  
  /* plot a character on the screen using the colour bar system */

  double scl;
  int col;

  if (scale == -1) {
    term_type(x,y," ",1,TERM_WHITE);
    return;
  }

  if (term_flag==0) {
    /* running on a QNX graphics console */


    term_box_on();
    scl=value/max_val;
    if (scl > 1) scl=1;
    if (scl < -1) scl=-1;

    if (scale==-2) term_type(x,y,&gsct_key,1,TERM_WHITE);
    else if (scale !=0) {
      col=(VEL_COL_MAX+scl*VEL_COL_MAX)/2;
      term_type(x,y,&vel_key[col].chr,1,vel_key[col].attr);
    } else {
      col=PWR_COL_MAX*scl;
      term_type(x,y,&pwr_key[col].chr,1,pwr_key[col].attr);
    }
  } else {
    /* running on xterm */
    if (scale !=0) {
      char str[2];
      col=10*value/max_val;
      if (scale==-2) 
        term_type(x,1,"g",1,TERM_WHITE | TERM_HILIGHT);
      else if (col==0) { 
        term_box_on();
        term_type(x,y,"l",1,TERM_WHITE);
        term_box_off();  
      } else {
        sprintf(str,"%d",abs(col));
        if (col<0) term_type(x,y,str,1,TERM_WHITE);
        else term_type(x,y,str,1,TERM_WHITE | TERM_HILIGHT);
      }
    } else {
      char str[2];
      col=10*value/max_val;
      sprintf(str,"%d",col);
      term_type(x,y,str,1,TERM_WHITE | TERM_HILIGHT);
    }
  }    
}
 
void draw_labels(char *str) {
  term_type(21,3,"beam:",0,TERM_WHITE);
  term_type(21,11,"freq:",0,TERM_WHITE);  
  term_type(23,25,"noise:",0,TERM_WHITE);
  term_type(21,54,"frang:",0,TERM_WHITE);
  term_type(21,66,"rsep:",0,TERM_WHITE); 
  term_type(22,3,"error code:",0,TERM_WHITE);   
  term_type(22,20,"station:",0,TERM_WHITE);
  term_type(22,50,"time:",0,TERM_WHITE);
  term_type(0,54,str,0,TERM_WHITE);
  term_type(1,60,"input:",0,TERM_WHITE);
  term_type(23,3,"attenuation:",0,TERM_WHITE);
  term_type(23,40,"nave:",0,TERM_WHITE);
  term_type(23,48,"AGC status:",0,TERM_WHITE);
  term_type(24,45,"LOWPWR status:",0,TERM_WHITE);
  term_flush();
}     

void draw_axis(struct rawdata *raw_data,int range) {/* plots the acf frame */
  int x,y,i,j;
  int max_lag_num;
  int acf_x_factor;
  int last_lag_index;
  int x_length,x_axis_tick;
  char str[16];
  
  last_lag_index=raw_data->PARMS.MPLGS-1;
  max_lag_num = raw_data->LAG_TABLE[1][last_lag_index] - 
                raw_data->LAG_TABLE[0][last_lag_index] + 1;

  acf_x_factor = MAX_LAG_NUMBER / max_lag_num ;

  if (range >=0) {
    sprintf(str,"%d",range);
    term_type(0,60,str,0,TERM_WHITE | TERM_HILIGHT);
  } else {
    term_type(0,60,"mx pwr",0,TERM_WHITE | TERM_HILIGHT);
  } 

  term_box_on();
  for (y=-ACF_NORM_FACTOR;y<ACF_NORM_FACTOR;y++) {
	if (y % 4 ==0)
	  term_type(11+y,10,&term_state.box_cross,1,TERM_WHITE | TERM_HILIGHT); 
	else term_type(11+y,10,&term_state.box_left_tee,1,
				   TERM_WHITE | TERM_HILIGHT);
  }
  term_box_off();

  x_length = (max_lag_num - 1) * acf_x_factor + 1;
  x_axis_tick = 2 * acf_x_factor;
  
  term_box_on();
  term_box(19,11, x_length,1,TERM_WHITE);
  term_axis(19,11,TERM_HOR_DN,
            TERM_NO_CAP,11,x_axis_tick,x_length,TERM_WHITE | TERM_HILIGHT);
  term_box_off(); 
  j=2;
  for(i= x_axis_tick;i <= x_length;i = i + x_axis_tick) {
    sprintf(str,"%3d",j);
    term_type(20,11+i,str,3,TERM_WHITE);
    j = j + 2;
  }  

/*
  term_box_on();
  for (x=0;x<raw_data->PARMS.NRANG;x++) {
	if (x % 10 ==0) 
	  term_type(1,x+3,&term_state.box_cross,1,TERM_WHITE | TERM_HILIGHT);	   
    else 
	  term_type(1,x+3,&term_state.box_top_tee,1,TERM_WHITE | TERM_HILIGHT);
  }
  term_box_off();
*/
  term_flush();
}


void draw_acf_banner(char *name,int range) {

  /* draw the ACF banner along the top of the screen */

  int x;
  char str[80];

  memset(str,' ',79);
  str[79]=0;
  term_type(0,0,str,80,TERM_WHITE);
  term_type(1,0,str,66,TERM_WHITE);
  
  if (term_flag==0) for (x=0;x<=PWR_COL_MAX;x++) plot(0,12+x,0,x,PWR_COL_MAX);		
  else for (x=0;x<10;x++) plot(0,30+x,0,x,10);
  term_type(0,3,"ACF",0,TERM_BLUE | TERM_HILIGHT);
  if (range >=0) {
    sprintf(str,"%d",range);
    term_type(0,60,str,0,TERM_WHITE | TERM_HILIGHT);
  } else {
    term_type(0,60,"mx pwr",0,TERM_WHITE | TERM_HILIGHT);
  } 
  term_type(0,68,name,0,TERM_RED | TERM_HILIGHT);  
  term_flush();
}

void draw_banner(char *name,int key) {
  
  /* draw the fitted data banner along the top of the screen */

  char str[80];
  int x;

  memset(str,' ',79);
  str[79]=0;
  term_type(0,0,str,80,TERM_WHITE);
  term_type(1,0,str,66,TERM_WHITE);
 
 if (term_flag ==0) {
    if  (key !=1) {
      for (x=0;x<=PWR_COL_MAX;x++) plot(0,12+x,0,x,PWR_COL_MAX);		
    } else {
      for (x=-VEL_COL_MAX-1;x<=VEL_COL_MAX;x+=2) 
         plot(0,32+(x/2),1,x,VEL_COL_MAX);
    }
  } else {
    if (key==1) {
      plot(0,35,1,0,10); 
      for (x=1;x<10;x++) { 
        plot(0,35+x,1,x,10);
        plot(0,35-x,1,-x,10);
      }  
    } else {
      for (x=0;x<10;x++) plot(0,30+x,0,x,10);
    }	
  }
 
  if (key==0) term_type(0,3,"Lbd pwr",0,TERM_BLUE | TERM_HILIGHT);
  else if (key==1) term_type(0,3,"Vel",0,TERM_BLUE | TERM_HILIGHT);
  else if (key==2) term_type(0,3,"Lbd wdt",0,TERM_BLUE | TERM_HILIGHT);
  if (key==0) sprintf(str,"%g",pwr_scale);
  if (key==1) sprintf(str,"%g",vel_scale);
  if (key==2) sprintf(str,"%g",wdt_scale);  
  term_type(0,60,str,0,TERM_WHITE | TERM_HILIGHT);
  term_type(0,68,name,0,TERM_RED | TERM_HILIGHT);  
  if (term_flag==0) {
     double y; 
     if (key==1) {
        y=-vel_scale;
        for (x=0;x<=(VEL_COL_MAX+1);x+=10) { 
          sprintf(str,"%g",y);
          term_type(1,12+x,str,0,TERM_WHITE);
          y+=20.0*vel_scale/(VEL_COL_MAX+1);
        }
      } else {
       y=0;
        for (x=0;x<=(PWR_COL_MAX+1);x+=5) { 
          sprintf(str,"%g",y);
          term_type(1,12+x,str,0,TERM_WHITE);
          if (key==0) y+=5.0*pwr_scale/(PWR_COL_MAX+1);
          if (key==2) y+=5.0*wdt_scale/(PWR_COL_MAX+1);
        }
      }
  }
  term_flush();
}

void draw_frame(int max_range) {

  /* draw a frame around the beams */

  int x,y;
  char str[16]; 
  
  /* label beam */

  for (y=0;y<MAX_BEAM;y++) {
    sprintf(str,"%d",y);
    term_type(3+y,0,str,0,TERM_WHITE);
    term_box_on();
	if (y % 4 ==0)
	  term_type(3+y,2,&term_state.box_cross,1,TERM_WHITE | TERM_HILIGHT); 
	else term_type(3+y,2,&term_state.box_left_tee,1,
				   TERM_WHITE | TERM_HILIGHT);
    term_box_off();
  }

  term_box_on();
  term_type(2,2,&term_state.box_top_left,1,TERM_WHITE | TERM_HILIGHT);
  term_type(3+MAX_BEAM,2,&term_state.box_bot_left,1,
		    TERM_WHITE | TERM_HILIGHT);
  term_box_off();
  for (x=0;x<max_range;x++) {
	if (x % 10 ==0) {
      sprintf(str,"%d",x);
      term_type(4+MAX_BEAM,x+2,str,0,TERM_WHITE);
      term_box_on();
	  term_type(2,x+3,&term_state.box_top_tee,1,TERM_WHITE | TERM_HILIGHT);
      term_type(3+MAX_BEAM,x+3,&term_state.box_cross,1,
				TERM_WHITE | TERM_HILIGHT);
      term_box_off();	   
	} else {
      term_box_on();
	  term_type(2,x+3,&term_state.box_horizontal,1,TERM_WHITE | TERM_HILIGHT);
	  term_type(3+MAX_BEAM,x+3,&term_state.box_horizontal,1,
				TERM_WHITE | TERM_HILIGHT);
      term_box_off();
    }
  }
  term_flush();
}


void plot_acf(struct rawdata *raw_data,int range) {

  /* plot ACF data on the screen */

  int rng;
  int mplgs,last_lag_index;
  int lag0_value,lag;
  int norm_unit;
  int max_lag_num,l_index;
  int acf_x_factor,acf_norm;
  int x,y,i,x_length;
  
  char str[16];
  
  term_box_on();
  term_box_fill(3,11,69,16,TERM_WHITE,TERM_BOX_NO_FRAME,' ');
  term_box_off();
  
  if (range < 0) {
    int min_power;
    min_power=0L;
	for (i=0;i<raw_data->PARMS.NRANG;i++) {
	  if (raw_data->pwr0[i]>min_power) {
	     min_power=raw_data->pwr0[i];
		 range=i;
	  } 
	}
	sprintf(str,"(%d)   ",range);
	term_type(0,66,str,0,TERM_WHITE | TERM_HILIGHT);
  }	
  
  mplgs = raw_data->PARMS.MPLGS;
  last_lag_index = mplgs - 1;

  lag0_value = raw_data->acfd[range][0][0];
  norm_unit = lag0_value / ACF_NORM_FACTOR;

  max_lag_num = raw_data->LAG_TABLE[1][last_lag_index] - 
                raw_data->LAG_TABLE[0][last_lag_index] + 1;

  acf_x_factor = MAX_LAG_NUMBER / max_lag_num ;

  if(acf_x_factor < 1) acf_x_factor = 1;

  x_length = (max_lag_num - 1) * acf_x_factor + 1;
  term_box_on();
  for (x=0;x<x_length;x++) 
    term_type(11,11+x,&term_state.box_horizontal,1,
			  TERM_WHITE | TERM_HILIGHT);
  term_box_off();
  for(l_index=0; l_index < mplgs; l_index++) {
    lag = raw_data->LAG_TABLE[1][l_index] - 
		  raw_data->LAG_TABLE[0][l_index];
    x = (lag * acf_x_factor) + 11;

    for(i=0; i < 2;i++) {
	  if (norm_unit !=0) { 
	    acf_norm =  raw_data->acfd[range][l_index][i] / norm_unit;  
        if (acf_norm > ACF_NORM_FACTOR) acf_norm = ACF_NORM_FACTOR-1;
        else if (acf_norm < -ACF_NORM_FACTOR) acf_norm = -ACF_NORM_FACTOR; 
	    y = 11 - acf_norm;	  
	    if (i==0) term_type(y,x,"+",0,TERM_RED);
		  else term_type(y,x,"-",0,TERM_BLUE);
	  }
    }	
  }
  sprintf(str,"%8ld",lag0_value);
  term_type(11-ACF_NORM_FACTOR,0,str,0,TERM_WHITE | TERM_HILIGHT);

  for (rng=0;rng<raw_data->PARMS.NRANG;rng++) {
    int snr;
	snr=convert_snr(raw_data->pwr0[rng],raw_data->PARMS.NOISE);
    plot(2,3+rng,0,snr,MAX_POWER_SCALE); 	
  }
  term_flush();
}
  

void plot_velocity(struct fitdata *fit_data) {

  /* plot velocity data on the screen */

  int rng;
  
  for (rng=0;rng<fit_data->prms.NRANG;rng++) {
    if ( (fit_data->rng[rng].p_l > thrhold) &&
         (fit_data->rng[rng].gsct ==0) &&
         (fit_data->rng[rng].qflg==1) ) 
      plot(3+fit_data->prms.BMNUM,3+rng,1,fit_data->rng[rng].v,vel_scale); 
    else if ( (fit_data->rng[rng].p_l > thrhold) &&
              (fit_data->rng[rng].gsct !=0))
      plot(3+fit_data->prms.BMNUM,3+rng,-2,0,0); 
	else plot(3+fit_data->prms.BMNUM,3+rng,-1,0,0); 	
  }
  term_flush();

}

void plot_power(struct fitdata *fit_data) {

  /* plot power data on the screen */

  int rng;
  
  for (rng=0;rng<fit_data->prms.NRANG;rng++) {
    if (fit_data->rng[rng].p_l > 0) plot(3+fit_data->prms.BMNUM,
	    3+rng,0,fit_data->rng[rng].p_l,pwr_scale); 
	else plot(3+fit_data->prms.BMNUM,3+rng,-1,0,0); 	
  }
  term_flush();
}

void plot_width(struct fitdata *fit_data) {

  /* plot spectral width data on the screen */

  int rng;
  
  for (rng=0;rng<fit_data->prms.NRANG;rng++) {
    if (fit_data->rng[rng].w_l > 0) plot(3+fit_data->prms.BMNUM,
	    3+rng,0,fit_data->rng[rng].w_l,wdt_scale); 
	else plot(3+fit_data->prms.BMNUM,3+rng,-1,0,0); 	
  }
  term_flush();
}

void plot_param(struct radar_id *rid,struct radar_parms *parms,char *text,
                char *data,char *program) {

  /* plot the radar parameters on the screen */

  char str[28];
  struct tm qnx_tm;
  int stat_bits,i;

  /* blank entries */
  term_type(21,8,"  ",0,TERM_WHITE);
  term_type(21,17,"        ",0,TERM_WHITE);
  term_type(23,31,"        ",0,TERM_WHITE);
  term_type(21,60,"    ",0,TERM_WHITE);
  term_type(21,71,"    ",0,TERM_WHITE);
  term_type(22,15,"   ",0,TERM_WHITE);
  term_type(24,3,"                             ",0,TERM_WHITE);
  term_type(23,45,"   ",0,TERM_WHITE);  

  sprintf(str,"%d",parms->BMNUM);
  term_type(21,8,str,0,TERM_WHITE | TERM_HILIGHT);
  sprintf(str,"%d",parms->TFREQ);
  term_type(21,17,str,0,TERM_WHITE | TERM_HILIGHT);
  sprintf(str,"%ld",parms->NOISE);
  term_type(23,31,str,0,TERM_WHITE | TERM_HILIGHT);
  sprintf(str,"%d",parms->FRANG);
  term_type(21,60,str,0,TERM_WHITE | TERM_HILIGHT);
  sprintf(str,"%d",parms->RSEP);
  term_type(21,71,str,0,TERM_WHITE | TERM_HILIGHT);
  sprintf(str,"%d",parms->ERCOD);
  term_type(22,15,str,0,TERM_WHITE | TERM_HILIGHT);
  sprintf(str,"%d",parms->NAVE);
  term_type(23,45,str,0,TERM_WHITE | TERM_HILIGHT);
  
  qnx_tm.tm_sec = parms->SEC;
  qnx_tm.tm_min = parms->MINUT;
  qnx_tm.tm_hour = parms->HOUR;
  qnx_tm.tm_mday = parms->DAY;
  qnx_tm.tm_mon = parms->MONTH - 1;
  qnx_tm.tm_year = parms->YEAR - 1900;
  qnx_tm.tm_wday = 0;
  qnx_tm.tm_yday = 0;
  _asctime(&qnx_tm,str);
  str[24]=0;
  term_type(22,55,str+4,0,TERM_WHITE | TERM_HILIGHT); 
  term_type(22,29,radar_name(rid,parms->ST_ID),0,TERM_WHITE | TERM_HILIGHT);
  if ((parms->ST_ID == ICEEAST) || (parms->ST_ID ==ICEWEST)
    || (parms->ST_ID ==FINLAND) || (parms->ST_ID ==SANAE)
    || (parms->ST_ID == HALLEY)) {
    sprintf(str,"00000000");
    for (i=0;i<parms->ATTEN;i++) str[7-i]='1';
  } else {
    switch (parms->ATTEN) {
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
  term_type(23,15,str,0,TERM_WHITE | TERM_HILIGHT);
  stat_bits=parms->AGC_STAT;
  sprintf(str,"----------------");
  for (i=0;i<16;i++) {
    if ((stat_bits & 1) !=0) str[i]='*';
	stat_bits=stat_bits>>1;
  }
  term_type(23,59,str,0,TERM_WHITE | TERM_HILIGHT);
  stat_bits=parms->LOPWR_STAT;
  sprintf(str,"----------------");
  for (i=0;i<16;i++) {
    if ((stat_bits & 1) !=0) str[i]='*';
	stat_bits=stat_bits>>1;
  }
  term_type(24,59,str,0,TERM_WHITE | TERM_HILIGHT);
  if (text !=NULL) {
    term_type(24,3,text,0,TERM_WHITE);
    term_type(24,3+strlen(text),data,0,TERM_WHITE | TERM_HILIGHT);
  }
  if (program !=NULL) {
    term_type(21,25,"`",0,TERM_WHITE | TERM_HILIGHT);
    term_type(21,26,program,0,TERM_WHITE);
    term_type(21,26+strlen(program),"'",0,TERM_WHITE | TERM_HILIGHT);
  }
  term_flush();
}  