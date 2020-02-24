/* file_lib.h
   ==========
   Author: R.J.Barnes
*/

#ifndef _INT_TYPES
#include "types.h"
#endif


#define LMAX 4096

#define FIELD_TXT 0
#define FIELD_CHAR 1
#define FIELD_DOUBLE 2
#define FIELD_FLOAT 3

#define FILE_STEP 64

struct file_index {
  double st_time,ed_time;
  int num;
  struct {
    double *start;
    double *end;
  } time;
  int *offset;
};


struct file_data {
  int type;
  union {
    char *tval;
    char cval;
    float fval;
    double dval;
    int32 ival;
    int16 sval;
    int32 lval;
  } data;
};


typedef int (*decoder)(char *,char *,char *,double,double,int,int,int,
                            struct file_data *,void *ptr);


int read_file(FILE *fp,int num,decoder *decode,void **ptr);
struct file_index *load_index(FILE *fp);
int seek_file(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,
              struct file_index *inx,
              double *aval);






