/* freq.h
   ======
   Author: R.J.Barnes
*/


/*
 $License$
*/

#ifndef _FREQ_H
#define _FREQ_H


#define DEFAULT_FREQ 13000

struct FreqTable {
  int num;
  int dfrq;
  int *start;
  int *end;
};

struct FreqTable *FreqLoadTable(FILE *fp);
int FreqTest(struct FreqTable *ptr,int freq);
   	
#endif
