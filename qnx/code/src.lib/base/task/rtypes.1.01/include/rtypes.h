/* rtypes.h
   ========
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
 $Log: rtypes.h,v $
 Revision 1.1  2004/05/04 17:11:23  barnes
 Initial revision

*/

#ifndef _RTYPES_H
#define _RTYPES_H

#ifdef _QNX4
typedef pid_t ipcid_t;
#else
typedef int ipcid_t;
#endif


/* This defines the int16, int32, and int64 types */

#include <limits.h>
#ifdef WORD_BIT
#if WORD_BIT == 16
typedef int int16;
typedef long int32;
#endif
#if WORD_BIT == 32
typedef short int16;
typedef int int32;
#endif
#elif defined LONG_BIT
#if LONG_BIT == 32
typedef short int16;
typedef long int32;
#endif
#if LONG_BIT == 64
typedef short int16;
typedef int int32;
#endif
#else
typedef short int16;
typedef long int32;
#endif
#ifdef LONG_BIT
#if LONG_BIT == 64
typedef long int64;
#endif
#endif
#endif
