/* print_config.c
   ============== */

/*
 $Log: print_config.c,v $
 Revision 1.3  2000/11/14 16:02:05  barnes
 Numerouse bug fixes to ensure the the local/remote condition of the
 synthesizer is correctly reported.

 Revision 1.2  2000/02/03 12:53:23  barnes
 Attempt to fix problem with masking out unused bits in the
 timing sequence control word.

 Revision 1.1  1998/11/08 19:11:58  barnes
 Initial revision

 */

#include <stdlib.h>
#include <stdio.h>

#include "port.h"

extern struct dio_hdw hdw;

void print_config(FILE *fp,int stereo) {
  int i;
  char *cstr={"AB"};
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

    fprintf(fp,"\n");
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
    fprintf(fp,"\n");
  }

  fprintf(fp,"maximum attenuation=%d\n",hdw.Atten_max);
  fprintf(fp,"local oscillator=%d\n",hdw.Local_Oscillator);
  if (hdw.BEAM_inv !=0) fprintf(fp,"beam logic inverted\n");
  if (hdw.FREQ_inv !=0) fprintf(fp,"frequency logic inverted\n");
  if (hdw.FSTAT_inv !=0) fprintf(fp,"frequency status logic is inverted\n");

  if (hdw.AGCstatus_inv !=0) fprintf(fp,"AGC status logic inverted\n");
  if (hdw.LOPWRstatus_inv !=0) fprintf(fp,"LOPWR status logic inverted\n");
}


