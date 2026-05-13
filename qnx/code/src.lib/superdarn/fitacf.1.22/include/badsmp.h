/* badsmp.h
   ========
   Author: R.J.Barnes
*/


/*
 $Log: badsmp.h,v $
 Revision 1.1  2004/07/12 22:17:25  barnes
 Initial revision

*/


#ifndef _BADSMP_H
#define _BADSMP_H

#define MAXBAD 50

struct FitACFBadSample {
  int nbad;
  int badsmp[MAXBAD];
};
      
#endif
