/* getshc.c
   ======== */

/* 
 $Log: getshc.c,v $
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


