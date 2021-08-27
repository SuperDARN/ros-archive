/* out_tsg_sx.h
   =========== */

#define X_MAX 10000

int out_tsg_sx(unsigned char *tsg_xtd_A,unsigned char *tsg_xtd_B,
               unsigned char *tsg_code_A,unsigned char *tsg_code_B,
               unsigned char *tsg_rep_A,unsigned char *tsg_rep_B,
	       int tsg_length_A,int tsg_length_B,
               int tsg_delay_A,int tsg_delay_B,
               char flag_A,char flag_B,
               unsigned char *fptr_A,unsigned char *fptr_B,
               int TSG_safe_A,int TSG_safe_B,  
               int TSG_port_A,int TSG_port_B,
               int TSG_mask_A,int TSG_mask_B,
               int TSG_state_A,int TSG_state_B,
               int BEAM_port_A,int BEAM_mask_A,int BEAM_state_A,
               int BEAM_port_B,int BEAM_mask_B,int BEAM_state_B,
               int CLOCK_port,int CLOCK_mask,
	       int *fadr_A,int *fadr_B);
