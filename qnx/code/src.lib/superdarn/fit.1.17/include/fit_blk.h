/* fit_blk.h
   =========
   Author: R.J.Barnes
*/

#ifndef _FIT_BLK_H
#define _FIT_BLK_H

#ifndef _RADAR_LIMIT_H
#include "limit.h"
#endif

struct fit_prm {
  int channel; /* zero=mono 1 or 2 is stereo */
  int offset; /* used for stereo badlags */
  int cp;
  int xcf;
  int tfreq;
  int noise;
  int nrang;
  int smsep;
  int nave;
  int mplgs;
  int mpinc;
  int txpl;
  int lagfr;
  int mppul;
  int bmnum;
  int old;
  int lag[2][LAG_TAB_LEN];
  int pulse[PULSE_PAT_LEN];
  int pwr0[MAX_RANGE];
  double interfer_pos[3];
  double beam_sep;
  double phidiff;
  double tdiff;
  double vdir;
};

struct fit_block {
  struct fit_prm prm;
  struct complex *acfd[MAX_RANGE];
  struct complex *xcfd[MAX_RANGE];
};

struct elv { /* elevation angle derived from the cross correlation */
  double normal;
  double low;
  double high;
};

struct noise { /* noise statistics */
  double vel;
  double skynoise;
  double lag0;
};

struct range_data {  /* fitted parameters for a single range */
  double v;
  double v_err;
  double p_0;
  double p_l;
  double p_l_err; 
  double p_s;
  double p_s_err;
  double w_l;
  double w_l_err;
  double w_s;
  double w_s_err;
  double phi0;
  double phi0_err;
  double sdev_l;
  double sdev_s;
  double sdev_phi;
  int qflg,gsct;
  char nump;
};
      
#endif
