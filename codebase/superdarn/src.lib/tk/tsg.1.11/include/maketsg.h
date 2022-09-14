/* maketsg.h
   ==========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/



#ifndef _MAKETSG_H
#define _MAKETSG_H

void TSGFree(struct TSGbuf *ptr);
struct TSGbuf *TSGMake(struct TSGprm *tsg,int *flg);


#endif
