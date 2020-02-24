/* out_tsg.h
   =========
   Author: R.J.Barnes
*/

#define X_MAX 10000

int out_tsg(unsigned char *tsg_code,unsigned char *tsg_rep,
	    int tsg_length,int safe,
            int TSG_port,int TSG_mask,int TSG_state,
            int CLOCK_port,int CLOCK_mask);
