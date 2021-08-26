/* fit_write.c
   =========== */

/*
 $Log: fit_write.c,v $
 Revision 1.3  2000/10/30 15:20:50  barnes
 Fixed macro variable to _QNX.

 Revision 1.2  1999/04/11 17:01:09  barnes
 Added checks for null files.

 Revision 1.1  1999/03/16 14:36:30  barnes
 Initial revision

 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "rawdata.h"
#include "fitdata.h"
#include "cnv_time.h"
#include "cnv_data.h"

#include "fit_str.h"

#if defined _QNX
#define HSTRING "%s version %s (QNX)\n"
#elif defined HP_UX
#define HSTRING "%s version %s (UNX)\n"
#elif defined VMS
#define HSTRING "%s version %s (VAX)\n"
#elif defined DOS
#define HSTRING "%s version %s (DOS)\n"
#else
#define HSTRING "%s version %s (UNX)\n"
#endif

int fit_header_write(int fitfp, char *text, char *name, char *version) {

  unsigned char head_buf[1024];
  int n;
  int rcode;
  char hstring[80];
  long ctime;

  /* first, clear the header buffer */
  memset(head_buf, 0, FIT_RECL);
  short_cnv(FIT_RECL,head_buf);
  short_cnv(INX_RECL,head_buf+sizeof(int16));
 
  n = sprintf(hstring, HSTRING ,name,version);

  strcpy(head_buf+2*sizeof(int16), hstring);
  ctime = time((long *) 0);
  strcat(head_buf+2*sizeof(int16), asctime(gmtime(&ctime)));
  strcat(head_buf+2*sizeof(int16),text);
  strcat(head_buf+2*sizeof(int16),"\n");

  if ((fitfp !=-1) && 
     (write(fitfp,head_buf, FIT_RECL) != FIT_RECL)) return -1;
  return 0;
}

int fit_inx_close(int inxfp,struct fitdata *ptr,int irec) {

  int pat[]={4,4,0,0};
  int32 index_rec[4];
  long ctime;

  ctime = time_yrsec(ptr->prms.YEAR,
				   ptr->prms.MONTH,
				   ptr->prms.DAY,
				   ptr->prms.HOUR,
				   ptr->prms.MINUT,
				   ptr->prms.SEC);

  if (lseek(inxfp, 0, SEEK_SET) != 0) return -1;

  if (read(inxfp,index_rec, 4*sizeof(int32)) != (4*sizeof(int32))) return -1;
  cnv_block( (unsigned char *) index_rec,pat);
  index_rec[1] = ctime;
  index_rec[3] = irec - 1;	/* irec is pointing to the next record */
  cnv_block( (unsigned char *) index_rec,pat);
  if (lseek(inxfp, 0, SEEK_SET) !=0) return -1;
  
  if (write(inxfp,index_rec, 4*sizeof(int32)) !=(4*sizeof(int32))) return -1;
  return 0;
}
	
int fit_inx_header_write(int inxfp,struct fitdata *ptr) {
  int pat[]={4,4,0,0};
  long int ctime;
  int32 index_rec[4];

  ctime = time_yrsec(ptr->prms.YEAR,
				   ptr->prms.MONTH,
				   ptr->prms.DAY,
				   ptr->prms.HOUR,
				   ptr->prms.MINUT,
				   ptr->prms.SEC);

  index_rec[0]=ctime;
  index_rec[1]=0;
  index_rec[2]=2;
  index_rec[3]=0;
  cnv_block( (unsigned char *) index_rec,pat);			 	
  if (write(inxfp,index_rec, sizeof(int32)*4) !=(4*sizeof(int32))) return -1;
			
  return 0;
}

int fit_inx_write(int inxfp,int drec,int dnum,struct fitdata *ptr) {
  int pat[]={4,4,0,0};
  long int ctime;
  int32 index_rec[4];
  ctime = time_yrsec(ptr->prms.YEAR,
				   ptr->prms.MONTH,
				   ptr->prms.DAY,
				   ptr->prms.HOUR,
				   ptr->prms.MINUT,
				   ptr->prms.SEC);
  index_rec[0]=ctime;
  index_rec[1]=drec;
  index_rec[2]=dnum;
  index_rec[3]=(ptr->prms.XCF != 0);
  cnv_block( (unsigned char *) index_rec,pat);
  if (write(inxfp,index_rec, sizeof(int32)*4) !=(4*sizeof(int32))) return -1;
  return 0;
}

int fit_write(int fitfp,struct fitdata *ptr) {
  int r1_pat[]={4,2,1,2,2,17,4,2,2,14,4,4,2,4,
                2,PULSE_PAT_LEN,2,2*LAG_TAB_LEN,1,COMBF_SIZE,4,3,
		2,2*MAX_RANGE,1,MAX_RANGE,0,0};

  int r2_pat[]={4,3,1,25,1,25,2,475,0,0};
  int dnum=0;
  union fit_out r;		
  int slist[MAX_RANGE];
  int i,j, k=0, hlength;
  int ctime;
  long int rrn;

  memset(&r,0,sizeof(union fit_out));
 
  ctime = time_yrsec(ptr->prms.YEAR,
				   ptr->prms.MONTH,
				   ptr->prms.DAY,
				   ptr->prms.HOUR,
				   ptr->prms.MINUT,
				   ptr->prms.SEC);
   
  /* transfer the parameter list, the pulse and lag tables, the lag-0 pwrs
	and the selection list to the first record (rel_rec_no 0) */
  r.r1.rrn = 0;
  hlength = sizeof(struct radar_parms) + 
            sizeof(int16)*(PULSE_PAT_LEN + 
            2*LAG_TAB_LEN) + COMBF_SIZE;
  r.r1.r_time = ctime;
  memcpy(&(r.r1.plist[0]),ptr, hlength);

  /*  Now transfer the noise parameters */

  r.r1.r_noise_lev = (long) ptr->noise.skynoise;
  r.r1.r_noise_lag0 = ptr->noise.lag0;
  r.r1.r_noise_vel = 10.0 * ptr->noise.vel;

  /* Now transfer the lag0 powers and the selection list */

  memset(slist,0,sizeof(int)*MAX_RANGE);

  for (i=0; i<ptr->prms.NRANG; i++) {
    if ((ptr->rng[i].qflg ==1) ||
       (ptr->xrng[i].qflg ==1)) {
      slist[k]=i+1;
      k++;
    }
  }

  for (i=0; i<ptr->prms.NRANG; i++) {
	r.r1.r_pwr0[i] = (int16) 100.0*ptr->rng[i].p_0;
    r.r1.r_slist[i] = slist[i];
    if (slist[i] > 0) r.r1.r_numlags[i]=ptr->rng[slist[i]-1].nump; 
  }

  cnv_block( (unsigned char *) &r,r1_pat);
  if ((fitfp !=-1) && 
     (write(fitfp,&r,sizeof(union fit_out)) !=sizeof(union fit_out))) 
     return -1;

  dnum++;

 
  /* Now start transferring the main data block */

  rrn=1;
  j = 0;

  /* first check and make sure that there is something to transfer.
	if not, return */
  if (slist[0]==0) return dnum;
  
  for (i=0; i<MAX_RANGE; i++) {
        r.r2.r_time = ctime;
        r.r2.r_xflag = 0;
        r.r2.rrn=rrn;
        r.r2.range[j] = slist[i];
        k=slist[i]-1;
	r.r2.r_qflag[j] = ptr->rng[k].qflg;
	r.r2.r_pwr_l[j] = 100*ptr->rng[k].p_l;
	r.r2.r_pwr_l_err[j] = 100*ptr->rng[k].p_l_err;
        r.r2.r_pwr_s[j] = 100*ptr->rng[k].p_s;
	r.r2.r_pwr_s_err[j] = 100*ptr->rng[k].p_s_err;
	r.r2.r_vel[j] = 10*ptr->rng[k].v;
	r.r2.r_vel_err[j] = 10*ptr->rng[k].v_err;
	r.r2.r_w_l[j] = 10*ptr->rng[k].w_l;
	r.r2.r_w_l_err[j] = 10*ptr->rng[k].w_l_err;
	r.r2.r_w_s[j] = 10*ptr->rng[k].w_s;
	r.r2.r_w_s_err[j] = 10*ptr->rng[k].w_s_err;
	r.r2.r_sdev_l[j] = 1000*ptr->rng[k].sdev_l;
	r.r2.r_sdev_s[j] = 1000*ptr->rng[k].sdev_s;
	r.r2.r_sdev_phi[j] = (32767. < 100*ptr->rng[k].sdev_phi) ?
			32767 : 100*ptr->rng[k].sdev_phi;
	r.r2.r_gscat[j] = ptr->rng[k].gsct;
	j++;
	if (j == 25) {
          cnv_block( (unsigned char *) &r,r2_pat);
          if ((fitfp !=-1) && 
          (write(fitfp,&r,sizeof(union fit_out)) 
           !=sizeof(union fit_out))) return -1;
          dnum++;			
	  rrn++;
	  j=0;
	  if ( i >= (MAX_RANGE-1)) break;
	  if (slist[i+1] == 0) break;
	}	
  }
  
  /*  Now write out the xcf data if there is any */
 
  j = 0;
  for (i=0; (i<MAX_RANGE) && (ptr->prms.XCF != 0); i++) {
        r.r2.r_time = ctime;
        r.r2.r_xflag = 1;
        r.r2.rrn=rrn;
	r.r2.range[j] = slist[i];
        k=slist[i]-1;
	r.r2.r_qflag[j] = ptr->xrng[k].qflg;
	r.r2.r_pwr_l[j] = 100*ptr->xrng[k].p_l;
	r.r2.r_pwr_l_err[j] = 100*ptr->xrng[k].p_l_err;
        r.r2.r_pwr_s[j] = 100*ptr->xrng[k].p_s;
	r.r2.r_pwr_s_err[j] = 100*ptr->xrng[k].p_s_err;
	r.r2.r_vel[j] = 10*ptr->xrng[k].v;
	r.r2.r_vel_err[j] = 10*ptr->xrng[k].v_err;
	r.r2.r_w_l[j] = 10*ptr->xrng[k].w_l;
	r.r2.r_w_l_err[j] = 10*ptr->xrng[k].w_l_err;
	r.r2.r_w_s[j] = 10*ptr->xrng[k].w_s;
	r.r2.r_w_s_err[j] = 10*ptr->xrng[k].w_s_err;

	r.r2.r_phi0[j] = 100*ptr->xrng[k].phi0;
	r.r2.r_phi0_err[j] = 100*ptr->xrng[k].phi0_err;
	r.r2.r_elev[j] = 100*ptr->elev[k].normal;
	r.r2.r_elev_low[j] = 100*ptr->elev[k].low;
	r.r2.r_elev_high[j] = 100*ptr->elev[k].high;

	r.r2.r_sdev_l[j] = 1000*ptr->xrng[k].sdev_l;
	r.r2.r_sdev_s[j] = 1000*ptr->xrng[k].sdev_s;
	r.r2.r_sdev_phi[j] = (32767. < 100*ptr->xrng[k].sdev_phi) ?
			32767 : 100*ptr->xrng[k].sdev_phi;
	r.r2.r_gscat[j] = ptr->rng[k].gsct;

	j++;
	if (j == 25) {
          cnv_block( (unsigned char *) &r,r2_pat);
          if ((fitfp !=-1) && 
          (write(fitfp,&r,sizeof(union fit_out)) 
           !=sizeof(union fit_out))) return -1;
          dnum++;
          rrn++;
	  j=0;
	  if ( i >= MAX_RANGE) break;
	  if (slist[i+1] == 0) break;
    }
  }

  return dnum;

}




int fit_fwrite(FILE *fitfp,struct fitdata *ptr) {
  if (fitfp==NULL) return fit_write(-1,ptr);
  return fit_write(fileno(fitfp),ptr);
}
   
int fit_header_fwrite(FILE *fitfp, char *text, char *name, char *version) {
  if (fitfp==NULL) return fit_header_write(-1,text,name,version);
  return fit_header_write(fileno(fitfp),text,name,version);
}

int fit_inx_header_fwrite(FILE *inxfp,struct fitdata *ptr) {
  if (inxfp==NULL) return -1;
  return fit_inx_header_write(fileno(inxfp),ptr);
}


int fit_inx_fclose(FILE *inxfp,struct fitdata *ptr,int irec) {
  if (inxfp==NULL) return -1;
  return fit_inx_close(fileno(inxfp),ptr,irec);
}


int fit_inx_fwrite(FILE *inxfp,int drec,int dnum,struct fitdata *ptr) {
  if (inxfp==NULL) return -1;
  return fit_inx_write(fileno(inxfp),drec,dnum,ptr);
}



