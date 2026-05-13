/* fclr.h
   ======
   Author: R.J.Barnes
*/


#ifndef _FCLR_H
#define _FCLR_H

#define FCLR_BUF_OVF 0x01
#define FCLR_FREQ_FAIL 0x02
#define FCLR_GAIN_FAIL 0x04
#define FCLR_AD_FAIL 0x08
#define FCLR_TSG_FAIL 0x10
#define FCLR_FCLR_FAIL 0x20
#define FCLR_SCAN_FAIL 0x40
#define FCLR_LOCAL_MODE 0x80

#define FCLR_NRANG_BASE 50
#define FCLR_NRANG_FINE 200

#define MIN_SMSEP_STEP 60
#define NUM_FCLR_FREQ 5
#define ATTEN_OFF 0

#define MAX_LONG 2147483647

int FCLRGetDCOffset(int tsgid,int samples,int cfreq,
		    int numchn,int maxatten, void **bufadr,
		    ipcid_t dioid,ipcid_t adid,ipcid_t resetid,
		    float *dco,int *flg);

int FCLRSet(ipcid_t dioid,struct TSGtable *ptr,int *tsgid);
int FCLRDoBase(int tsgid,int samples,int start,
              int end,
              int step,
              void **bufadr,int bufsize,
              struct FreqTable *fptr,
              int *lowfrq,float *statistic,int *flg,float *dco,
	      int numchn,ipcid_t dioid,ipcid_t adid,ipcid_t resetid);
int FCLRDoFine(int tsgid,int samples,int *lowfrq,
             int start,int numchn,float mxpwr,int maxatten,
             void **bufadr,
             ipcid_t dioid,ipcid_t adid,ipcid_t resetid,
	       int *atten,float *noise,int *flg,float *dco);


int FCLR(ipcid_t dioid,ipcid_t adid,ipcid_t resetid,
         struct TSGtable *tab,int *tsgid,struct FreqTable *fptr,
         int start,
	 int end,int step,void **bufadr,int bufsize,
	 float mxpwr,int maxatten,
	 int *atten,float *noise,
         float *statistic,float *dco,int *flg);

#endif
