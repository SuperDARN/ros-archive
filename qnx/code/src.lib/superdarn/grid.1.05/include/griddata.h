/* grid.h
   ====== */

#ifndef _GRIDDATA_H
#define _GRIDDATA_H


struct svec {
  int st_id;
  int npnt;
  double freq0;
  char major_revision;
  char minor_revision;
  int prog_id;
  char gsct;

  struct {
    double mean;
    double sd;
  } noise;

  struct {
    double min;
    double max;
  } vel;

  struct {
    double min;
    double max;
  } pwr;

  struct {
    double min;
    double max;
  } wdt;

  struct {
    double min;
    double max;
  } verr;


};

struct gvec {
  double mlat,mlon;
  double azm;

  struct {
   double median;
   double sd;
  } vel;

  struct {
   double median;
   double sd;
  } pwr;

  struct {
   double median;
   double sd;
  } wdt;

  int st_id;
  int index;
 
};

struct griddata {
  double st_time;
  double ed_time;
 

  int nprm[2];
  int npnt[2];  
  char *name[2];
  char *type[2];
  char *unit[2];

  /* for now hardcode in the structure pointers */

  struct svec *sdata;
  struct gvec *data;
};

int locate_cell(int npnt,struct gvec *ptr,int index);


int decode_grid_one(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr);

int decode_grid_two(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr);

int decode_grid_three(char *name,char *unit,char *type,
                    double st_time,double ed_time,
                    int npnt,int nprm,int pnt,
                    struct file_data *data,void *ptr);


int read_grid(FILE *fp,struct griddata *);

int grid_seek(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,
              struct file_index *inx,
              double *aval);

void merge_grid(struct griddata *,struct griddata *);
void avg_grid(struct griddata *,struct griddata *,int flg);

void copy_grid(struct griddata *a,struct griddata *b);
void add_grid(struct griddata *a,struct griddata *b,int recnum);
void sort_grid(struct griddata *ptr);
void integrate_grid(struct griddata *a,struct griddata *b,double *err);


#endif



