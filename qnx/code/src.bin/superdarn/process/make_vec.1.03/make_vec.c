 /* make_vec.c
    ========== */

/*
 $Log: make_vec.c,v $
 Revision 1.9  2000/08/22 15:04:20  barnes
 Added return to main.

 Revision 1.8  2000/08/16 22:42:09  barnes
 Changed return type of main().

 Revision 1.7  1999/10/27 22:00:12  barnes
 Fixed bug in the linear regression algorithm.

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "print_info.h"
#include "cnv_time.h"
#include "radar_name.h"
#include "file_lib.h"
#include "griddata.h"
#include "math_constant.h"
#include "hlpstr.h"
struct radar_id *rid=NULL;

struct griddata rcd;

#define VSTRING "8.0"

int linreg(struct gvec *pnt,int npnt,
	   double *parvel,double *pervel,
	   double *parvelx,double *pervelx,
	   double *frms) {

  int k;
  double ferr[32];

  double sx2=0,cx2=0,ysx=0,ycx=0,cxsx=0;
  double sum=0,den;
  double xval,yval;
  double ds,d2sdv2;
  for (k=0;k<npnt;k++) {
     sx2=sx2+sind(pnt[k].azm)*sind(pnt[k].azm);
     cx2=cx2+cosd(pnt[k].azm)*cosd(pnt[k].azm);
     ysx=ysx-pnt[k].vel.median*sind(pnt[k].azm);
     ycx=ycx-pnt[k].vel.median*cosd(pnt[k].azm);
     cxsx=cxsx+sind(pnt[k].azm)*cosd(pnt[k].azm);
  }   

  den=sx2*cx2-cxsx*cxsx;
  
  if (den==0) return 1;
  
  *parvel=(sx2*ycx-cxsx*ysx)/den;
  *pervel=(cx2*ysx-cxsx*ycx)/den;

  for (k=0;k<npnt;k++) {
    xval=pnt[k].azm;
    yval=*parvel*cosd(xval)+*pervel*sind(xval);
    sum=sum+(pnt[k].vel.median-yval)*(pnt[k].vel.median-yval);
  }
  *frms=sqrt(sum/npnt);
  ds=25.0;

  d2sdv2=1/(*frms*npnt)*cx2;

  *parvelx=sqrt((2*ds)/d2sdv2);
  d2sdv2=1/(*frms*npnt)*sx2;
  *pervelx=sqrt((2*ds)/d2sdv2); 

  return 0;
}

int gvec_sort(const void *a,const void *b) {
  struct gvec *x;
  struct gvec *y;
  x=(struct gvec *) a;
  y=(struct gvec *) b;

  if (x->mlat < y->mlat) return -1;
  if (x->mlat > y->mlat) return 1;
  return 0;

}

int main(int argc,char *argv[]) {

  int verbose=0;  
  char *envstr;
  FILE *fp;   
  char *fname=NULL;
  double tme;
  int inx=0;
  int cnt=0;
  int st_id=-1;
  int help=0;
  int npnt;
  struct gvec *data=NULL;  
  int i,j,k;
  int rcnt=0;
  double parvel,pervel,parvelx,pervelx,frms,fac,qn,qr;
  int plat,plon,tflag,pvE,pvP,pvEx,pvPx,qlflag;
  int yr,mo,dy,hr,mt,yrsec;
  double sc;
  time_t ctime;
 


  envstr=getenv("SD_RADARNAME");
  if (envstr == NULL) {
    fprintf(stderr,"Environment variable 'SD_RADARNAME' must be defined.\n");
    exit(1);
  }
 
  fp=fopen(envstr,"r");
  rid=load_radar(fp);
  fclose(fp);

  if (argc>1) {
    for (k=1;k<argc;k++) {
      if (strcmp(argv[k],"-vb")==0) verbose=1;
      else if (strcmp(argv[k],"--help")==0) help=1;
      else if (argv[k][0]=='-') st_id=radar_id(rid,argv[k][1]);
      else fname=argv[k];
    }
  }   

  if (help==1) {
    print_info(stdout,hlpstr);
    exit(0);
  }
  if (fname==NULL) fp=stdin;
  else fp=fopen(fname,"r");
  if (fp==NULL) {
    fprintf(stderr,"File not found.\n");
    exit(-1);
  }

  if (read_grid(fp,&rcd)==-1) {
    fprintf(stderr,"Error reading file.\n");
    exit(1);
  }

  ctime = time(NULL);
  if (st_id !=-1) 
    fprintf(stdout,"vlptm %s (QNX) %s\n",VSTRING,radar_name(rid,st_id));
  else 
    fprintf(stdout,"vlptm %s (QNX) %s\n",VSTRING,
                    radar_name(rid,rcd.sdata[0].st_id));
  fprintf(stdout,"%s",asctime(gmtime(&ctime)));
  fprintf(stdout,"make_vec\n\n");

  fprintf(stdout,
    "   M.LAT   M.LONG TYP QFL    Veast    Vpole    VEerr    VPerr\n");

  do {
    epoch_time(rcd.st_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (verbose==1) fprintf(stderr,"%d-%d-%d %d:%d:%d",dy,mo,yr,hr,mt,sc);
    yrsec=time_yrsec(yr,mo,dy,hr,mt,sc);
    fprintf(stdout," %4d %4d %4d %4d %4d %4d    %8ld",yr,mo,dy,hr,mt,
	    (int) sc,yrsec);
    epoch_time(rcd.ed_time,&yr,&mo,&dy,&hr,&mt,&sc);
    if (verbose==1) fprintf(stderr,"-%d:%d:%d ",hr,mt,sc);
  
    yrsec=time_yrsec(yr,mo,dy,hr,mt,sc);
    fprintf(stdout," %8ld\n",yrsec);
    fprintf(stdout,"Vlos: ");
    for (k=0;k<16;k++) {
       fprintf(stdout,"%8.1f",0.0);
       if ((k+1) % 8==0) fprintf(stdout,"\n");
    }
    
 
    if (st_id !=-1) { 
      for (inx=0;(inx<rcd.npnt[0]) && (rcd.sdata[inx].st_id != st_id);inx++); 
      if (inx==rcd.npnt[0]) {
        if (verbose==1) fprintf(stderr,"npnt=%d\n",0);
        fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);
        continue;    
      }
    }
   


    if (rcd.sdata[inx].npnt<3)  {
      if (verbose==1) fprintf(stderr,"npnt=%d\n",0);
      fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);

      continue;
    }
    npnt=rcd.sdata[inx].npnt;
    data=malloc(sizeof(struct gvec)*npnt);
    j=0;

    for (i=0;i<rcd.npnt[1];i++) {     
      if ((st_id==-1) || (rcd.data[i].st_id==st_id)) {
        memcpy(&data[j],&rcd.data[i],sizeof(struct gvec));
        if (data[j].mlat<0)  data[j].azm=data[j].azm-90.0;
        else data[j].azm=90.0-data[j].azm;
        j++;
      }
    }
    

    /* okay best approach is to sort the vector table into latitude order */
    
    qsort(data,npnt,sizeof(struct gvec),gvec_sort);
    j=0;
    cnt=0;
    for (i=0;i<npnt;i++) {
      if (data[i].mlat !=data[j].mlat) {
        /* got the end of one row */
        
        if ((i-j) > 3) {
           if (linreg(&data[j],i-j,&parvel,&pervel,&parvelx,
                      &pervelx,&frms) !=0) continue;
           cnt++;
           plat=100*data[j].mlat;
           plon=0;
           for (k=j;k<i;k++) plon=plon+data[k].mlon;
           plon=100*plon/(i-j);
           tflag=3;
           fac=sqrt(parvel*parvel+pervel*pervel)/500.0;
           pvE=10*parvel;
           pvP=10*pervel;
           pvEx=10*parvelx;
           pvPx=10*pervelx;
           qn=1-(1.0/(16-5))*(i-j-5);
           qr=1-(1.0/(fac*100.0))*((fac*100.0)-frms);
           qlflag=3.0*qn+2.0*qr;           
         
           if (frms> fac*100.0) qlflag=5;
           fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
		   plat,plon,tflag,qlflag,pvE,pvP,pvEx,pvPx);

         
        }
        j=i;
      }
    }  
    
   
    
    if (verbose==1) fprintf(stderr,"npnt=%d\n",cnt);
    fprintf(stdout,"%8d %8d %3d %3d %8d %8d %8d %8d\n",
	    0,0,0,0,0,0,0,0);
    free(data);
  } while (read_grid(fp,&rcd) !=-1);

  if (fp !=stdout) fclose(fp);

  return 0;
}






















