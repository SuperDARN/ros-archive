/* meteor_proc.c
   =============
   Author: Kevin O'Rourke, adapted by R.J.Barnes
*/




/* 
 $Log: meteor_proc.c,v $
 Revision 1.3  2002/01/21 18:29:55  barnes
 Modification to make code backwards compatible with ROS 1.01.

 Revision 1.2  2002/01/21 16:14:54  barnes
 Removed debugging code

 Revision 1.1  2002/01/21 16:09:45  barnes
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "math_constant.h"

#include "hardware.h"
#include "radar_name.h" 
#include "geo.h"

#include "print_info.h"
#include "rawdata.h"
#include "fitdata.h"
#include "fit_read.h"


#include "errstr.h"
#include "hlpstr.h"

#include "meteor.h"
#include "math.h"

#include "nrutil.h"

struct radar_id *rid=NULL;
struct radar_hardware *hdt=NULL;
struct radar_site *hdw;

struct fitdata fit;

int cnt=0;

struct metdata {
  int yr,mo,dy,hr,mt,sc;
  int bmnum;
  int frang,rsep;
  int max_gate;
  unsigned char flg[75];
  double vlos[75];
};

struct metdata *met=NULL;


double bm_total[16];
double bm_sdtmp[16];
int bm_count[16];
int num_avgs=0;

int beams=0;
double vlos[16];
double sdev[16];

double vx,vy;
double vm;
double sdvx,sdvy;
double lat,lon,rho,vmlat,vmlon;

/*
double x[16];
double y[16];
double sig[16];
double a[2];
double u[16*2];
double v[2*2];
double w[2];
*/

double *a;
double **u;
double **v;
double *w;
double *x;
double *y;
double *sig;

double coseps;
double chisq;
/*
double cvm[2*2];
*/

double **cvm;

double calc_coseps(double range) {
  double eps;
  eps = asin(METEOR_HEIGHT/range);
  return cos(eps);
};


double calc_azi(int bmnum) {
  double azi;
  azi = hdw->beam_sep*(bmnum-7.5)+hdw->boresite;
  return (azi*PI/180.0);
};

void cosfunc(double x, double afunc[], int ma) {
	afunc[1] = -cos(x);
	afunc[2] = sin(x);
};


main (int argc,char *argv[]) {
  int i,j;
  struct fitfp *ffp=NULL;
  FILE *fp;
  int fc=0,c;
  int verbose=0;
  int help=0;
 

  double max_vel = MAX_VEL;
  double min_sn = MIN_SN;
  double max_v_err = MAX_V_ERR;
  double max_w_l = MAX_W_L;
  int max_range = MAX_MET_RANGE;
  int vm_beam = VM_BEAM;
  int min_beams = MIN_BEAMS;
  int bm_type = BM_TYPE;
  int req_hour=0;
  int max_gate=0;
  int st_id=-1;
  int year,month,day;
  
  int smsep,lagfr,rxrise;
 
  int bc=0;

  char *envstr;

  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }
 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);
  
  envstr=getenv("SD_HARDWARE");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_HARDWARE' must be defined.\n");
    exit(1);
  }
  hdt=load_hdw(envstr,rid); 


  
  if (argc>1) {
    for (c=1;c<argc;c++) { 
      if (strcmp(argv[c],"--help")==0) help=1;
      else if (strcmp(argv[c],"-vb")==0) verbose=1; 
      else if (strcmp(argv[c],"-mv")==0) {
        max_vel=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-ms")==0) {
        min_sn=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-me")==0) {
        max_v_err=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-mw")==0) {
        max_w_l=atof(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-mr")==0) {
        max_range=atoi(argv[c+1]);
        c++;
      } else if (strcmp(argv[c],"-mr")==0) {
        max_range=atoi(argv[c+1]);
        c++;
     } else if (strcmp(argv[c],"-bm")==0) {
        vm_beam=atoi(argv[c+1]);
        c++;
     } else if (strcmp(argv[c],"-mb")==0) {
        min_beams=atoi(argv[c+1]);
        c++;
     }  else if (strcmp(argv[c],"-mz")==0) {
       if (tolower(argv[c+1][0])=='m') bm_type=0;
       else bm_type=1;
       c++;
     } else if (strcmp(argv[c],"-hr")==0) {
       req_hour=atoi(argv[c+1]);
       c++;
     } else break;
    } 
    fc=c;
  } else {
    print_info(stderr,errstr);
    exit(-1);
  }

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }

  for (c=fc;c<argc;c++) {
    ffp=fit_open(argv[c],NULL); 
    fprintf(stderr,"Opening file %s\n",argv[c]);
    if (ffp==NULL) {
      fprintf(stderr,"file %s not found\n",argv[c]);
      continue;
    }
 
    while (fit_read(ffp,&fit) !=-1) {

      if (hdw==NULL) {
        hdw=get_site(hdt,fit.prms.YEAR,fit.prms.MONTH,
                                  fit.prms.DAY,fit.prms.HOUR,fit.prms.MINUT,
                                  fit.prms.SEC,fit.prms.ST_ID);
        st_id=fit.prms.ST_ID;
        lagfr=fit.prms.LAGFR;
        smsep=fit.prms.RSEP;
        rxrise=fit.prms.RXRISE;
        if (rxrise==0) rxrise=hdw->rec_rise;
        rxrise=hdw->rec_rise;

      }
      /* select the data */

   
      if (fit.prms.HOUR !=req_hour) continue; 
      if (fit.prms.SCAN <0) continue;
      if (fit.prms.FRANG==0) continue;
      if (fit.prms.RSEP==0) continue;
      if (met==NULL) met=malloc(sizeof(struct metdata));
      else met=realloc(met,sizeof(struct metdata)*(cnt+1));
     
      
      met[cnt].yr=fit.prms.YEAR;
      met[cnt].mo=fit.prms.MONTH;
      met[cnt].dy=fit.prms.DAY;
      met[cnt].hr=fit.prms.HOUR;
      met[cnt].mt=fit.prms.MINUT;
      met[cnt].sc=fit.prms.SEC;
      met[cnt].bmnum=fit.prms.BMNUM;
      met[cnt].frang=fit.prms.FRANG;
      met[cnt].rsep=fit.prms.RSEP;
     
      max_gate=(max_range-fit.prms.FRANG)/fit.prms.RSEP;
      met[cnt].max_gate=max_gate;
      for (i=0;i<max_gate;i++) {
        met[cnt].flg[i]=0;
        if (fit.rng[i].qflg==0) continue;
        if (fabs(fit.rng[i].v) > max_vel) continue;
        if (fit.rng[i].p_l < min_sn) continue;
        if (fit.rng[i].v_err >= max_v_err) continue;
        if (fit.rng[i].w_l > max_w_l) continue;
        met[cnt].flg[i]=1;
        met[cnt].vlos[i]=fit.rng[i].v;
      }
      cnt++;
    } 
    fit_close(ffp);
  }

  /* now do the fitting */
  year=met[i].yr;
  month=met[i].mo;
  day=met[i].dy;

  for (i=0;i<cnt;i++) {
    for (j=0;j<met[i].max_gate;j++) {
      if (met[i].flg[j]==0) continue;
      bm_total[met[i].bmnum]+= met[i].vlos[j];
      bm_count[met[i].bmnum]++;
      num_avgs++;
    }
  }

  for (i=0;i<16;i++) {
    if (bm_count[i] > 0) {
      beams++;
      vlos[i] =bm_total[i]/bm_count[i];
     } else vlos[i]=0;
  };

  for (i=0;i<cnt;i++) {
    for (j=0;j<met[i].max_gate;j++) {
      if (met[i].flg[j]==0) continue;
      bm_sdtmp[met[i].bmnum]+=(met[i].vlos[j]-vlos[met[i].bmnum])*
	(met[i].vlos[j]-vlos[met[i].bmnum]);
      
    }
  }
  for (i=0;i<16;i++) {
    if (bm_count[i] > 1) {;
      sdev[i] =sqrt(bm_sdtmp[i]/(bm_count[i]-1));
     } else {
      sdev[i]=1;
      vlos[i]=0;
     }
  };
  if (beams<min_beams) {
    fprintf(stderr,"Not enough beams of data to generate a velocity vector\n");
    exit(0);
  }


  x = dvector(1,16);
  y = dvector(1,16);
  sig = dvector(1,16);
  a = dvector(1,2);
  u = dmatrix(1,16,1,2);
  v = dmatrix(1,2,1,2);
  w = dvector(1,2);

  coseps = calc_coseps(max_range/2.0);

  bc=0;
  for (i=0;i<16;i++) {
    if (bm_count[i]>1) {
      x[++bc]=calc_azi(i);   
      y[bc] =vlos[i]/coseps; /* mean velocity */
      sig[bc] = sdev[i];	
    }
  }

 
 
  fprintf(stderr,"Fitting %d of 16 beams\n",bc);
 

  dsvdfit(x, y, sig, bc, a, 2, u, v, w, &chisq, &cosfunc);

  
  vx=a[1];
  vy=a[2];
  cvm=dmatrix(1,2,1,2);
  dsvdvar(v, 2, w, cvm);

  sdvx = sqrt(cvm[1][1]);
  sdvy = sqrt(cvm[2][2]);
  
  vm=vlos[vm_beam]/coseps;
  
  geographic(0,7,3,hdw,lagfr,smsep,rxrise,METEOR_HEIGHT,&rho,
             &lat,&lon);

  geographic(0,vm_beam,3,hdw,lagfr,smsep,rxrise,METEOR_HEIGHT,&rho,
             &vmlat,&vmlon);


  fprintf(stdout, "# met file\n# Created by %s\n", "$Id: meteor_proc.c,v 1.3 2002/01/21 18:29:55 barnes Exp $");
  fprintf(stdout, "# Criteria\n");
  fprintf(stdout,"Vlos(max)=%.2f\nS/N(min)=%.2f\nrange(max)=%d\n", 
                  max_vel, min_sn, max_range);
  fprintf(stdout, "Verr(max)=%.2f\nnum_beams(min)=%d\n", max_v_err, min_beams);
  fprintf(stdout, "w_l(max)=%.2f\n", max_w_l);
  if (bm_type == 0) fprintf(stdout,
    "# Beam for meridional wind\nbeam_num=%d\nwind=meridional\n",vm_beam);
  else fprintf(stdout, 
    "# Beam for zonal wind\nbeam_num=%d\nwind=zonal\n",vm_beam);
  fprintf(stdout, "# Station ID\nst_id=%d\n", st_id);

  if (bm_type == 0) fprintf(stdout,
     "# year,month,day,hour,Vx,Vy,lat,long,Vm,Vm_lat,Vm_long,num_avgs\n");
  else fprintf(stdout, 
     "# year,month,day,hour,Vx,Vy,lat,long,Vz,Vz_lat,Vz_long,num_avgs\n");

  fprintf(stdout, "%4d,%02d,%02d,%02d,%.0f,%.0f,%.1f,%.1f,%.0f,%.1f,%.1f,%d\n",
	  year,month,day,req_hour,vx,vy,lat,lon,vm,vmlat,vmlon,num_avgs);

  fprintf(stdout, "# sdev_vx,sdev_vy\n");
  fprintf(stdout, "%.2f,%.2f\n", sdvx, sdvy);
  fprintf(stdout, "# used frang=%d and rsep=%d\n", met[0].frang, met[0].rsep);

  return 0;
} 


















