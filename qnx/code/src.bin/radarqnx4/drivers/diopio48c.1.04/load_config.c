/* load_config.c
   =============
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "port.h"

/*
 $Log: load_config.c,v $
 Revision 1.2  2004/06/22 14:56:40  barnes
 Added the majority of Sessai's modifications.

 Revision 1.1  2004/05/10 14:27:15  barnes
 Initial revision

*/

extern struct dio_hdw hdw;
extern struct dio_param param;

int load_config(FILE *fp) {
   int i;
   int channel=0;
   char *tkn;
   char *cmd,*op;
   char delim[]={' ','\t','\n','=',','};
   char line[1024];

   while (fgets(line,1024,fp) !=0) {
    for (i=0; (line[i] !=0) && 
              ((line[i] ==' ') || (line[i]=='\n'));i++);

    /* ignore comments or empty lines */

    if ((line[i]==0) || (line[i]=='#')) continue;
    tkn=line+i; 
    cmd=strtok(tkn,delim);
    if (strcmp(cmd,"name")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) strncpy(hdw.name,op,63);
    } else if (strcmp(cmd,"channel")==0) {
      op=strtok(NULL,"\n");
      if ((op !=NULL) && (op[0]=='b')) channel=1;
      else if (op !=NULL) channel=0;
    } else if (strcmp(cmd,"beam_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.BEAM_port[channel]);
        else hdw.BEAM_port[channel]=-1;
      }
    } else if (strcmp(cmd,"beam_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.BEAM_mask[channel]);
        else hdw.BEAM_mask[channel]=0;
      }
    } else if (strcmp(cmd,"atten_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.Atten_port[channel]);
        else hdw.Atten_port[channel]=-1;
      }
    } else if (strcmp(cmd,"atten_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.Atten_mask[channel]);
        else hdw.Atten_mask[channel]=0;
      }
    } else if (strcmp(cmd,"antenna_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.ANTENNA_port[channel]);
        else hdw.ANTENNA_port[channel]=-1;
      }
    } else if (strcmp(cmd,"antenna_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.ANTENNA_mask[channel]);
        else hdw.ANTENNA_mask[channel]=0;
      }
    } else if (strcmp(cmd,"filter_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FILTER_port[channel]);
        else hdw.FILTER_port[channel]=-1;
      }
    } else if (strcmp(cmd,"filter_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FILTER_mask[channel]);
        else hdw.FILTER_mask[channel]=0;
      }
    } else if (strcmp(cmd,"tsg_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.TSG_port[channel]);
        else hdw.TSG_port[channel]=-1;
      }
    } else if (strcmp(cmd,"tsg_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.TSG_mask[channel]);
        else hdw.TSG_mask[channel]=-1;
      }
     } else if (strcmp(cmd,"tsg_safe")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.TSG_safe[channel]);
        else hdw.TSG_safe[channel]=-1;
      }
    } else if (strcmp(cmd,"clock_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.CLOCK_port[channel]);
        else hdw.CLOCK_port[channel]=-1;
      }
    } else if (strcmp(cmd,"clock_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.CLOCK_mask[channel]);
        else hdw.CLOCK_mask[channel]=-1;
      }
    } else if (strcmp(cmd,"test_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.TEST_port[channel]);
        else hdw.TEST_port[channel]=-1;
      }
    } else if (strcmp(cmd,"test_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.TEST_mask[channel]);
        else hdw.TEST_mask[channel]=0;
      }
    } else if (strcmp(cmd,"agc_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.AGCstatus_port[channel]);
        else hdw.AGCstatus_port[channel]=-1;
      }
    } else if (strcmp(cmd,"agc_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.AGCstatus_mask[channel]);
        else hdw.AGCstatus_mask[channel]=0;
      }
    } else if (strcmp(cmd,"lopwr_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.LOPWRstatus_port[channel]);
        else hdw.LOPWRstatus_port[channel]=-1;
      }
    } else if (strcmp(cmd,"lopwr_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.LOPWRstatus_mask[channel]);
        else hdw.LOPWRstatus_mask[channel]=0;
      }
    } else if (strcmp(cmd,"fstatus_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.Fstatus_port[channel]);
        else hdw.Fstatus_port[channel]=-1;
      }
    } else if (strcmp(cmd,"fstatus_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.Fstatus_bit[channel]);
        else hdw.Fstatus_bit[channel]=0;
      }
    } else if (strcmp(cmd,"10mhz_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FMHz10_port[channel]);
        else hdw.FMHz10_port[channel]=-1;
      }
    } else if (strcmp(cmd,"10mhz_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FMHz10_mask[channel]);
        else hdw.FMHz10_mask[channel]=0;
      }
    } else if (strcmp(cmd,"khzmhz_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FMKHz_port[channel]);
        else hdw.FMHz10_port[channel]=-1;
      }
    } else if (strcmp(cmd,"khzmhz_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FMKHz_mask[channel]);
        else hdw.FMKHz_mask[channel]=0;
      }
    } else if (strcmp(cmd,"khz_port")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FKHz_port[channel]);
        else hdw.FKHz_port[channel]=-1;
      }
    } else if (strcmp(cmd,"khz_mask")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.FKHz_mask[channel]);
        else hdw.FKHz_mask[channel]=0;
      }
    } else if (strcmp(cmd,"micro_clock")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.micro_clock[channel]);
        else hdw.micro_clock[channel]=-1;
      }
    } else if (strcmp(cmd,"micro_control")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.micro_clock_cw[channel]);
        else hdw.micro_clock_cw[channel]=0;
      }
    } else if (strcmp(cmd,"oneshot_clock")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.one_shot[channel]);
        else hdw.one_shot[channel]=-1;
      }
    } else if (strcmp(cmd,"oneshot_control")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.one_shot_cw[channel]);
        else hdw.one_shot_cw[channel]=0;
      }
    } else if (strcmp(cmd,"atten_max")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) hdw.Atten_max=atoi(op);
      if ((hdw.Atten_max<0) || (hdw.Atten_max>7)) hdw.Atten_max=3;
    } else if (strcmp(cmd,"beam_inv")==0) hdw.BEAM_inv=1;
      else if (strcmp(cmd,"freq_inv")==0) hdw.FREQ_inv=1;
      else if (strcmp(cmd,"fstat_inv")==0) hdw.FSTAT_inv=1;
      else if (strcmp(cmd,"fstatus_inv")==0) hdw.FSTAT_inv=1;
      else if (strcmp(cmd,"agc_inv")==0) hdw.AGCstatus_inv=1;
      else if (strcmp(cmd,"lopwr_inv")==0) hdw.LOPWRstatus_inv=1;
      else if (strcmp(cmd,"oscillator")==0) {
      op=strtok(NULL,"\n");
      if (op !=NULL) hdw.Local_Oscillator=atoi(op);
    } else if (strcmp(cmd,"control_word")==0) {
      int word;
      op=strtok(NULL,delim);
      if (op !=NULL) word=atoi(op);
      else continue;
      if ((word<0) || (word>3)) continue;
      op=strtok(NULL,delim);
      if (op !=NULL) {
        if (op[0] !='x') sscanf(op,"%x",&hdw.control_word[word]);
        else hdw.control_word[word]=0;
      }
    }      
  }
  return 0;
}
