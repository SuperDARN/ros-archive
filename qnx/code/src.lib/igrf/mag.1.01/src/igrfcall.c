/* igrfcall.c
   ========== */

/*
 $Log: igrfcall.c,v $
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



