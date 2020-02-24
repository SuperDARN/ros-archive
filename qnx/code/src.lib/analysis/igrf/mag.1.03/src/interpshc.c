/* interpshc.c
   ===========
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
 $Log: interpshc.c,v $
 Revision 1.3  2001/06/27 20:43:53  barnes
 Added license tag

 Revision 1.2  2001/01/29 18:11:55  barnes
 Added Author Name

 Revision 1.1  1999/08/10 12:23:06  barnes
 Initial revision

 */

#include <math.h>
#include "math_constant.h"

int interpshc(double date, 
              double dte1, 
              int nmax1, double *gh1, 
              double dte2, double nmax2, 
              double *gh2, int *nmax, double *gh) {
    
  
    int i, k, l;
    double factor;
  
    --gh;
    --gh2;
    --gh1;

    factor = (date - dte1) / (dte2 - dte1);
    if (nmax1 == nmax2) {
	k = nmax1 * (nmax1 + 2);
	*nmax = nmax1;
    } else if (nmax1 > nmax2) {
	k = nmax2 * (nmax2 + 2);
	l = nmax1 * (nmax1 + 2);
    
	for (i = k + 1; i <= l; ++i) {
	    gh[i] = gh1[i] + factor * (-gh1[i]);
	}
	*nmax = nmax1;
    } else {
	k = nmax1 * (nmax1 + 2);
	l = nmax2 * (nmax2 + 2);
     
	for (i = k + 1; i <= l; ++i) {
	    gh[i] = factor * gh2[i];
	}
	*nmax = nmax2;
    }
  
    for (i = 1; i <= k; ++i) {
	gh[i] = gh1[i] + factor * (gh2[i] - gh1[i]);
    }
    return 0;
} 

