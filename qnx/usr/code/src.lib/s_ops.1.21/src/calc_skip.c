/* calc_skip.c
   =========== */

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
 $Log: calc_skip.c,v $
 Revision 1.6  2002/03/26 16:55:05  barnes
 Fixed redundant headers.

 Revision 1.5  2001/06/27 21:10:46  barnes
 Added license tag

 Revision 1.4  2000/10/24 16:35:46  barnes
 Fixed header problem.

 Revision 1.3  2000/10/13 16:01:20  barnes
 Fixed return state of function.

 Revision 1.2  2000/10/13 15:56:03  barnes
 Fixed bug in calc skip.

 Revision 1.1  1999/03/31 19:57:19  barnes
 Initial revision

 */
#include <sys/types.h>
#include <time.h>
#include "tsg.h"
#include "limit.h"
#include "global_s.h"

int calc_skip(float bnd) {
  int skip;

  if (sec !=0) skip=((minut*60+sec) % (int) bnd + (intt+1)/2)/intt+2;
  if (skip>15) skip=0;
  return skip;
}

 
