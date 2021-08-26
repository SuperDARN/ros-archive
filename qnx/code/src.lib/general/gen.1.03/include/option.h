/* option.h 
   ======== */

#ifndef _OPTION_H
#define _OPTION_H

struct option {
 char *optname;
 char type;
 int set;
 void *ptr;
};


struct optstr {
  int num;
  struct option *ptr;
};


int opt_process(int argc,char *argv[],
               struct option *opt,void (*opterr)(char *str));

void opt_file(FILE *fp,
               struct option *opt,void (*opterr)(char *str));


int option_process(int argc,char *argv[],
               struct optstr *opt,void (*opterr)(char *str));

void option_file(FILE *fp,
               struct optstr *opt,void (*opterr)(char *str));

int option_add(struct optstr *ptr,char *name,char type,void *data);
void option_free(struct optstr *ptr);
struct option *option_read(struct optstr *ptr,int num);


#endif

