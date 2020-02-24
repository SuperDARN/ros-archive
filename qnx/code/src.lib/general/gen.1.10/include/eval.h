/* eval.h
   ====== */

struct stack {
  int num;
  unsigned char *buf;
};

struct postfix {
  int num;
  char *type;
  int  *object;

  int vnum;
  double *value;
};

int eval(char *sum,double *ptr,
         int (*dvar)(char *ptr,double *val,void *data),
         void *vdata,
         int (*dfun)(char *ptr,int argnum,double *argptr,
                     double *val,void *data),
         void *fdata);

