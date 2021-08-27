/* file_lib.h
   ========== */

#define LMAX 256

struct file_data {
  int type;
  union {
    char *tval;
    char cval;
    double dval;
    int ival;
  } data;
};


typedef int (*decoder)(char *,char *,char *,double,double,int,int,int,
                            struct file_data *,void *ptr);


int read_file(FILE *fp,int num,decoder *decode,void **ptr);


 






