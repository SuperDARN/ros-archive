/* middle.c
   ======== */

/*
 $Log: middle.c,v $
 Revision 1.8  2000/03/21 20:16:00  barnes
 Several bug fixes to do with making sure the output of the filter agrees
 with the inputs.

 Revision 1.7  1999/10/14 14:17:17  barnes
 Added attenuation to record.

 Revision 1.6  1999/10/08 22:13:00  barnes
 Modifications for the extended format.

 Revision 1.5  1999/07/23 20:29:44  barnes
 Greatly improved speed by only re-allocating memory as it is needed.

 Revision 1.4  1999/01/28 14:25:10  barnes
 Modification to cope with changes in radardata structure.

 Revision 1.3  1998/06/07 22:33:04  barnes
 Modifications to use the radardata structure and to use epoch time.

 Revision 1.2  1998/06/04 23:36:23  barnes
 Fixed missing return value.

 Revision 1.1  1998/02/03 23:11:58  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include "radar_scan.h"


/* take the middle scan */


int middle(struct radardata *input,
	   struct radardata *output) {
  int i,j;
  output->scnt=input[1].scnt;
  output->st_id=input[1].st_id;
  output->ftype=input[1].ftype;
  output->stored=input[1].stored;
  output->major_rev=input[1].major_rev;
  output->minor_rev=input[1].minor_rev;
  output->stime=input[1].stime;
  output->etime=input[1].etime;

  for (i=0;i<16;i++) {      
     output->bmcnt[i]=0;
     if (input[1].bmcnt[i] !=0) {
       output->bmcnt[i]=1;
       if (output->cpid[i]==NULL) output->cpid[i]=malloc(sizeof(int));
       if (output->intt[i]==NULL) output->intt[i]=malloc(sizeof(int));
       if (output->frang[i]==NULL) output->frang[i]=malloc(sizeof(int));
       if (output->rsep[i]==NULL) output->rsep[i]=malloc(sizeof(int));
       if (output->rxrise[i]==NULL) output->rxrise[i]=malloc(sizeof(int));

       if (output->freq[i]==NULL) output->freq[i]=malloc(sizeof(int));
       if (output->noise[i]==NULL) output->noise[i]=malloc(sizeof(int));
       if (output->atten[i]==NULL) output->atten[i]=malloc(sizeof(int));
       if (output->time[i]==NULL) output->time[i]=malloc(sizeof(double));

       output->frang[i][0]=input[1].frang[i][0];
       output->rsep[i][0]=input[1].rsep[i][0];
       output->rxrise[i][0]=input[1].rxrise[i][0];
       output->freq[i][0]=input[1].freq[i][0]; 
       output->noise[i][0]=input[1].noise[i][0];
       output->atten[i][0]=input[1].atten[i][0];
       output->intt[i][0]=input[1].intt[i][0];
       output->cpid[i][0]=input[1].cpid[i][0];
       output->time[i][0]=input[1].time[i][0]; 
       for (j=0;j<MAX_RANGE;j++) {
         if (output->sct[i][j]==NULL) output->sct[i][j]=malloc(sizeof(char));
         if (output->rng[i][j]==NULL) output->rng[i][j]=
             malloc(sizeof(struct datapoint));
         output->sct[i][j][0]=input[1].sct[i][j][0];       
         memcpy(&output->rng[i][j][0],&input[1].rng[i][j][0],
	      sizeof(struct datapoint));
       }

     }
   }
  return 0;
}
 


