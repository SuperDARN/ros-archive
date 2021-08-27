/* gtable.h 
   ======== */

#define MAX_RANGE 75

#define MAJOR_REVISION 0
#define MINOR_REVISION 90

struct gpnt {
  int cnt;
  int npix;
  int ref;
  double mlat;
  double mlon;
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
  
};

struct gtable {
  double st_time;
  double ed_time;
  int status;
  int st_id;
  int prog_id;
  int nscan;
  int npnt;
  int frang[16],rsep[16],rxrise[16];
  double freq;
  struct {
    double mean;
    double sd;
  } noise;
  int gsct; 
  double min[4],max[4];
 
  int ltable[MAX_RANGE*16];
  double azm[MAX_RANGE*16]; 
  int pnum;
  struct gpnt *pnt;
};

int write_gtable(int fp,struct gtable *ptr,char *log,int xtd);
int fwrite_gtable(FILE *fp,struct gtable *ptr,char *log,int xtd);

int test_gtable_write(struct gtable *ptr,struct radardata *scan,int tlen);
void map_to_gtable(struct gtable *ptr,struct radardata *scan,
                struct radar_site *pos,int tlen);
 