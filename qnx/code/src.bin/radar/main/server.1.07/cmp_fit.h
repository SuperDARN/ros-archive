/* cmp_fit.h
   =========
   Author: R.J.Barnes
*/

#define read_bit(t,n) (((t)[n/8] & (1<<(n%8))) !=0) 
#define set_bit(t,n)  (t)[n/8]=(t)[n/8] | (1<<(n%8))

struct frame_data {
  char msg;
  struct radar_parms prms;
  char gsct[10];
  char dflg[10];
  double store[75*4]; /* this is the theoretical maximum */
};

int  compress_fit(struct fitdata *fit_data,struct frame_data *frame);
int  compress_xcf(struct fitdata *fit_data,struct frame_data *frame);
