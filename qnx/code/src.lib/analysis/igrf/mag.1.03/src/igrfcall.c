/* igrfcall.c
   ==========
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
 $Log: igrfcall.c,v $
 Revision 1.5  2001/06/27 20:43:53  barnes
 Added license tag

 Revision 1.4  2001/01/29 18:11:55  barnes
 Added Author Name

 Revision 1.3  1999/12/15 20:38:34  barnes
 Fixed problem with a filename.

 Revision 1.2  1999/12/15 18:44:06  barnes
 Modification for the 2000 co-oefficient set.

 Revision 1.1  1999/08/10 12:23:06  barnes
 Initial revision

 */


/* store the co-efficients up here */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "interpshc.h"
#include "extrapshc.h"
#include "getshc.h"
#include "shval3.h"

double igrf_date=-1;

static  char *filmod[] = {"dgrf45.dat","dgrf50.dat","dgrf55.dat","dgrf60.dat",
                      "dgrf65.dat","dgrf70.dat","dgrf75.dat","dgrf80.dat",
                      "dgrf85dat","dgrf90.dat","igrf95.dat","igrf2000.dat",
                      "igrf2000s.dat",0}; 

double dtemod[] = { 1945,1950,1955,1960,1965,1970,
                    1975,1980,1985,1990,1995,2000,2005 };

double gh1[120],gh2[120],gha[120],ghb[120];
double erad;
int nmax,nmax1,nmax2;

int igrfcall(double date, double flat, double flon, 
              double elev, double *x, double *y, double *z) {
 
    int i;
    char *envstr;
    char file1[256];
    char file2[256];
    double ext[3]={0.0,0.0,0.0};
    double a2 = 40680925.0;
    double b2 = 40408588.0;                       
	
    if (igrf_date !=date) {
      
      for (i=0;(dtemod[i] !=0) && (dtemod[i]<date);i++);
      if (dtemod[i]==0) return -1;
      i--;

      igrf_date=date;
      envstr=getenv("IGRF_PATH");
    
      sprintf(file1,"%s/%s",envstr,filmod[i]);
    
      getshc(file1, &nmax1, &erad, gh1);

      sprintf(file2,"%s/%s",envstr,filmod[i+1]);
   
      getshc(file2, &nmax2, &erad, gh2);
          
      if (i < 10) {
	  interpshc(date, dtemod[i], 
                    nmax1, gh1, dtemod[i+1], nmax2, gh2, &nmax, gha);
  	  interpshc(date+1,dtemod[i], 
                    nmax1, gh1, dtemod[i+1], nmax2, gh2, &nmax, ghb);
      } else {
	  extrapshc(date, dtemod[i], 
                    nmax1, gh1, nmax2, gh2, &nmax, gha);
  	  extrapshc(date+1,dtemod[i], 
                    nmax1, gh1, nmax2, gh2, &nmax, ghb);
      }
      

    }

    shval3(1, flat, flon, elev, erad, a2, b2, nmax, gha, 0,
	   ext, x, y, z);

    return 0;
} 



