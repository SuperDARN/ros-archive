/* freq_lib.h
   ==========
   Author: R.J.Barnes
*/


#define DEFAULT_FREQ 13000

struct freq_table {
  int num;
  int dfrq;
  int *start;
  int *end;
};

struct freq_table *load_ftable(FILE *fp);
int test_freq(struct freq_table *ptr,int freq);
   	

