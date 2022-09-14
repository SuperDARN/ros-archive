/* average.c
   =========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "limit.h"



int ACFAverage(float *pwr0,float *acfd,
		 float *xcfd,int *lagsum,int nave,int nrang,int mplgs) {
   int range;
   int lag,sum;
   
   float *acfdptr=NULL;
   float *xcfdptr=NULL;
   float *pwr0ptr=NULL;
 

   pwr0ptr=pwr0;
  
   
   for (range = 0; range < nrang ; range++) {
     acfdptr = &acfd[range*(2*mplgs)];
     if (xcfd !=NULL) xcfdptr = &xcfd[range*(2*mplgs)];
	      
	 for(lag=0;lag < mplgs ; lag++) {

           if (lagsum !=NULL) sum=lagsum[lag];
           else sum=1;
           if (sum==0) sum=1;

       *acfdptr = *acfdptr / (sum*nave);   
	   acfdptr++;
       *acfdptr = *acfdptr / (sum*nave);   
       acfdptr++; 
       if(xcfdptr !=NULL) {
         *xcfdptr = *xcfdptr / (sum*nave);  /* real */
         xcfdptr++;
         *xcfdptr = *xcfdptr / (sum*nave);   /* imag */
         xcfdptr++; 
        }
      } 
      *pwr0ptr = *pwr0ptr / nave;
      pwr0ptr++;
   } 
   return 0;
}


