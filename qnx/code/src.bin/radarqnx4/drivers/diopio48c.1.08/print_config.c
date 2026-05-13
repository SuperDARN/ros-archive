/* print_config.c
   ==============
   Author: R.J.Barnes & A. Sessai Yukimatu
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

#include <stdlib.h>
#include <stdio.h>
#include "freq.h"
#include "port.h"


/*
 $Log: print_config.c,v $
 Revision 1.3  2004/11/12 15:13:13  barnes
 Modifications to handle digital phased radars.

 Revision 1.2  2004/06/22 14:56:40  barnes
 Added the majority of Sessai's modifications.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

 */

extern struct dio_hdw hdw;
extern char *frq_name;				
extern char *cnf_name;				
extern struct FreqTable *fptr;			
extern int dfrq;			      

void print_config(FILE *fp,int stereo) {
  int i;
  char *cstr={"AB"};


  fprintf(fp,"\n");			 
  if(cnf_name!=NULL) fprintf(fp,"Configuration file name : %s\n",cnf_name);
  else	fprintf(fp,"No special conf file loaded\n");
  fprintf(fp,"Configuration Name:%s\n\n",hdw.name);

  for (i=0;i<(1+stereo);i++) {
    fprintf(fp,"Channel:%c\n\n",cstr[i]);
    if (hdw.BEAM_port[i] !=-1) 
      fprintf(fp,"beam_port[%d]=%.2x,mask=%.2x\n",i,hdw.BEAM_port[i],
				    hdw.BEAM_mask[i]);
      else fprintf(fp,"beam_port[%d]=undefined\n",i);

    if (hdw.Atten_port[i] !=-1) 
      fprintf(fp,"atten_port[%d]=%.2x,mask=%.2x\n",i,hdw.Atten_port[i],
				    hdw.Atten_mask[i]);
      else fprintf(fp,"atten_port[%d]=undefined\n",i);
    if (hdw.ANTENNA_port[i] !=-1) 
      fprintf(fp,"antenna_port[%d]=%.2x,mask=%.2x\n",i,hdw.ANTENNA_port[i],
				    hdw.ANTENNA_mask[i]);
      else fprintf(fp,"antenna_port[%d]=undefined\n",i);
    if (hdw.PHASE_port[i] !=-1) 
      fprintf(fp,"phase_port[%d]=%.2x,mask=%.2x\n",i,hdw.PHASE_port[i],
				    hdw.PHASE_mask[i]);
      else fprintf(fp,"phase_port[%d]=undefined\n",i);
    if (hdw.FILTER_port[i] !=-1) 
      fprintf(fp,"filter_port[%d]=%.2x,mask=%.2x\n",i,hdw.FILTER_port[i],
				    hdw.FILTER_mask[i]);
    else fprintf(fp,"filter_port[%d]=undefined\n",i); 
    if (hdw.TSG_port[i] !=-1) 
      fprintf(fp,"tsg_port[%d]=%.2x,mask=%.2x, safe=%.2x\n",i,hdw.TSG_port[i],
                                                   hdw.TSG_mask[i],
                                                   hdw.TSG_safe[i]);
    else fprintf(fp,"tsg_port[%d]=undefined\n",i); 
    if (hdw.CLOCK_port[i] !=-1) 
      fprintf(fp,"clock_port[%d]=%.2x,mask=%.2x\n",i,
                                                   hdw.CLOCK_port[i],
                                                   hdw.CLOCK_mask[i]);
    else fprintf(fp,"clock_port[%d]=undefined\n",i); 

    if (hdw.TEST_port[i] !=-1) 
      fprintf(fp,"test_port[%d]=%.2x,mask=%.2x\n",i,hdw.TEST_port[i],
				    hdw.TEST_mask[i]);
    else fprintf(fp,"test_port[%d]=undefined\n",i); 
    if (hdw.AGCstatus_port[i] !=-1) 
      fprintf(fp,"agc_port[%d]=%.2x,mask=%.2x\n",i,hdw.AGCstatus_port[i],
				    hdw.AGCstatus_mask[i]);
    else fprintf(fp,"agc_port[%d]=undefined\n",i); 

    if (hdw.LOPWRstatus_port[i] !=-1) 
      fprintf(fp,"lopwr_port[%d]=%.2x,mask=%.2x\n",i,hdw.LOPWRstatus_port[i],
				    hdw.LOPWRstatus_mask[i]);
    else fprintf(fp,"lopwr_port[%d]=undefined\n",i); 

    if (hdw.Fstatus_port[i] !=-1) 
      fprintf(fp,"fstatus_port[%d]=%.2x,mask=%.2x\n",i,hdw.Fstatus_port[i],
				    hdw.Fstatus_bit[i]);
    else fprintf(fp,"fstatus_port[%d]=undefined\n",i); 
    if (hdw.FMHz10_port[i] !=-1) 
    fprintf(fp,"10mhz_port[%d]=%.2x,mask=%.2x\n",i,hdw.FMHz10_port[i],
				    hdw.FMHz10_mask[i]);
    else fprintf(fp,"10mhz_port[%d]=undefined\n",i); 
    if (hdw.FMKHz_port[i] !=-1) 
    fprintf(fp,"khzmhz_port[%d]=%.2x,mask=%.2x\n",i,hdw.FMKHz_port[i],
				    hdw.FMKHz_mask[i]);
    else fprintf(fp,"khzmhz_port[%d]=undefined\n",i); 
    if (hdw.FKHz_port[i] !=-1) 
    fprintf(fp,"khz_port[%d]=%.2x,mask=%.2x\n",i,hdw.FKHz_port[i],
				    hdw.FKHz_mask[i]);
    else fprintf(fp,"khz_port[%d]=undefined\n",i); 

    if (hdw.CLOCK_port[i] !=-1)
    fprintf(fp,"clock_port[%d]=%.2x,mask=%.2x\n",i,hdw.CLOCK_port[i],  
				    hdw.CLOCK_mask[i]);
    else fprintf(fp,"clock_port[%d]=undefined\n",i); 	

    if (hdw.micro_clock[i] !=-1) 
      fprintf(fp,"micro_clock[%d]=%.2x\n",i,hdw.micro_clock[i]);
    else fprintf(fp,"micro_clock[%d]=undefined\n",i); 

    if (hdw.micro_clock_cw[i] !=-1) 
      fprintf(fp,"micro_control[%d]=%.2x\n",i,hdw.micro_clock_cw[i]);
    else fprintf(fp,"micro_control[%d]=undefined\n",i); 
    
    if (hdw.one_shot[i] !=-1) 
      fprintf(fp,"oneshot_clock[%d]=%.2x\n",i,hdw.one_shot[i]);
    else fprintf(fp,"oneshot_clock[%d]=undefined\n",i); 
    if (hdw.one_shot_cw[i] !=-1) 
      fprintf(fp,"oneshot_control[%d]=%.2x\n",i,hdw.one_shot_cw[i]);
    else fprintf(fp,"oneshot_control[%d]=undefined\n",i); 
  }

  for (i=0;i<=1;i++) fprintf(fp,"control_word[%d,%d]=0x%02x,0x%02x\n",
                     i*2,i*2+1,hdw.control_word[i*2],hdw.control_word[i*2+1]); 

  fprintf(fp,"\n");


 fprintf(fp,"maximum attenuation=%d\n",hdw.Atten_max);
  fprintf(fp,"local oscillator=%d\n",hdw.Local_Oscillator);

  if (hdw.BEAM_inv !=0) fprintf(fp,"beam logic : inverted\n");
  else fprintf(fp,"beam logic : normal\n");

  if (hdw.FREQ_inv !=0) { 
    fprintf(fp,"true_freq calc way : Syowa style ");
    fprintf(fp,"( true_freq = (Local_Oscillator - freq)/2 )\n");
  } else {
     fprintf(fp,"true_freq calc way : normal style ");	       
     fprintf(fp,"( true_freq = Local_Oscillator + freq )\n");	
  }								    

  if (hdw.FSTAT_inv !=0) fprintf(fp,"fstatus logic : inverted\n");
  else fprintf(fp,"fstatus logic : normal\n");

  if (hdw.AGCstatus_inv !=0) fprintf(fp,"AGC status logic : inverted\n");
  else fprintf(fp,"AGC status logic : normal\n");

  if (hdw.LOPWRstatus_inv !=0) fprintf(fp,"LOPWR status logic : inverted\n");
  else fprintf(fp,"LOPWR status logic : normal\n");   

  fprintf(fp,"\n");						

  if(fptr!=0){
    fprintf(fp,"loaded restrict freq table file name : %s\n",frq_name);
    fprintf(fp,"default freq = %d\n",dfrq);
    fprintf(fp,"default freq = %d\n",fptr->dfrq);
    fprintf(fp,"entry num. in  ftable = %d\n",fptr->num);
  } else { 
    fprintf(fp,"No restrict freq table loaded\n");
    fprintf(fp,"default freq = %d\n",dfrq);
  }

}


