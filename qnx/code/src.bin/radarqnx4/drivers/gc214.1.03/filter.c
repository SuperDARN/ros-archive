/* filter.c 
   ====== 
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

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

/*
 $Log: filter.c,v $
 Revision 1.2  2004/06/22 12:38:15  barnes
 QNX4 Code audit.

 Revision 1.1  2004/05/10 15:03:40  barnes
 Initial revision

*/

float setCFIR(int *cfircoeffs,int Fsamp,int Fpass,int Fstop,int decimation){

  
  int pi=3.14159;
  float freq_in;

  float hamm[]={.0800,	.1025,	.1679,	.2696,	.3979,
		.5400,	.6821,	.8104,	.9121,	.9775,
		1};

 
  float CFIRgain;
  double wp,ws,wc;
  int n;
    
  freq_in=((float)Fsamp)/((float)decimation);	
  wp=pi*((float)Fpass/(float)freq_in);
  ws=pi*((float)Fstop/(float)freq_in);
  wc=(wp+ws)/2;
  CFIRgain=0;
  for(n=0;n<11;n++){
    cfircoeffs[n]=(int) (32767*hamm[n]*(pi/wc)*
                          sin(wc*(n-10))/(pi*(n-10))+.49999);
    cfircoeffs[10]=32767;
    CFIRgain=CFIRgain+cfircoeffs[n];
  }
  CFIRgain=CFIRgain/65536;
  return CFIRgain;
}


float setPFIR(int *pfircoeffs,int Fsamp,int Fpass,int Fstop,int decimation){

  
  int pi=3.14159;
  float freq_in;
	
  float hamm[]={.0800,	.0824,	.0894,	.1011,	.1173,	.1378,	.1624,	.1910,
		.2231,	.2584,	.2967,	.3374,	.3802,	.4247,	.4703,	.5167,
		.5633,	.6097,	.6553,	.6998,	.7426,	.7833,	.8216,	.8569,
		.8890,	.9176,	.9422,	.9627,	.9789,	.9906,	.9976,	1.000};
  
  float PFIRgain;
  double wp,ws,wc;
  int n;

  freq_in=(float)Fsamp/(2*(float)decimation);
  wp=pi*(float)Fpass/(float)freq_in;
  ws=pi*(float)Fstop/(float)freq_in;
  wc=(wp+ws)/2;
  PFIRgain=0;
  for(n=0;n<32;n++){
    pfircoeffs[n]=(int)(32767*hamm[n]*(pi/wc)*sin(wc*(n-31))/
                        (pi*(n-31))+.49999);
    pfircoeffs[31]=32767;
    PFIRgain=PFIRgain+pfircoeffs[n];
  }
  PFIRgain=PFIRgain/65536;
  return PFIRgain;
}
