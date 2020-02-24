/* out_tsg_x.h
   ===========
   Author: R.J.Barnes
*/

#define X_MAX 10000

int out_tsg_x(unsigned char *tsg_xtd,
	      unsigned char *tsg_code,unsigned char *tsg_rep,
	      int tsg_length,unsigned char flag,
              unsigned char *fptr,
              int TSG_safe,
              int TSG_port,int TSG_mask,int TSG_state,
              int CLOCK_port,int CLOCK_mask,
	      int BEAM_port,int BEAM_mask,int BEAM_state,int *fadr);
