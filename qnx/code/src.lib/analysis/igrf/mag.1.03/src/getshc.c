/* getshc.c
   ========
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
 $Log: getshc.c,v $
 Revision 1.4  2001/06/27 20:43:53  barnes
 Added license tag

 Revision 1.3  2001/01/29 18:11:55  barnes
 Added Author Name

 Revision 1.2  1999/09/08 20:36:37  barnes
 Fixed missing return value.

 Revision 1.1  1999/08/10 12:23:06  barnes
 Initial revision
 
 */

#include <stdio.h>
#include <stdlib.h>


int getshc(char *fname,int *nmax,double *erad,double *gh) {

  char dummy[256];
  int nn,mm,n,m;
  double g,h;
  double yr;
  FILE *fp;
  int stat;
  int i=0;
  fp=fopen(fname,"r");
  if (fp==NULL) return -1;
   
   stat=(fgets(dummy,256,fp)==NULL);
   if (stat !=0) {
     fclose(fp);
     return -1;
   }
    
   stat=(fscanf(fp,"%d %lg %lg",nmax,erad,&yr) !=3);
   if (stat !=0) {
     fclose(fp);
     return -1;
   }
   
   for (nn=1;nn<=*nmax;nn++) {
     for (mm=0;mm<=nn;mm++) {
    
       stat=(fscanf(fp,"%d %d  %lg %lg ",&n,&m,&g,&h) !=4);
       if (stat !=0) {
         fclose(fp);
         return -1;
       }
       gh[i]=g;
       i++;
       if (m !=0) {
         gh[i]=h;
         i++; 
       } 
     }
   }
   fclose(fp);
   return 0;
}


