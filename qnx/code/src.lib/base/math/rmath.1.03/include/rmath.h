/* rmath.h
   =======
   Author: R.J.Barnes
*/


/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
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
 $Log: rmath.h,v $
 Revision 1.1  2004/05/04 22:37:42  barnes
 Initial revision

*/


#ifndef _RMATH_H
#define _RMATH_H

#ifndef _COMPLEX
struct complex {
    double x,y;
};
#define cabs(a) sqrt((a.x*a.x)+(a.y*a.y))
#endif

#define C 2.997e+8
#define LN_TO_LOG 4.342944819

#ifndef HUGE_VAL
#define HUGE_VAL 1.0e36                /* default value of HUGE */
#endif
 
#ifndef PI
#define PI 3.14159265358979323846
#endif

#define asind(x) asin(x)*180/PI
#define acosd(x) acos(x)*180/PI
#define cosd(x) cos(PI*(x)/180.0)
#define sind(x) sin(PI*(x)/180.0)
#define tand(x) tan(PI*(x)/180.0)
#define atand(x) atan(x)*180.0/PI
#define atan2d(x,y) atan2(x,y)*180.0/PI
#endif













