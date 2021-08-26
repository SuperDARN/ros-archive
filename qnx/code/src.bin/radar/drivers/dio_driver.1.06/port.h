/* port.h 
   ====== */

struct dio_hdw {
  char name[64];
  int Fstatus_port[2];	
  int FKHz_port[2];
  int FMKHz_port[2];
  int FMHz10_port[2];
  int Atten_port[2];
  int BEAM_port[2];
  int ANTENNA_port[2];
  int FILTER_port[2];
  int TSG_port[2];
  int TSG_safe[2];
  int CLOCK_port[2];
  int TEST_port[2];
  int AGCstatus_port[2];
  int LOPWRstatus_port[2];
  int micro_clock[2];
  int micro_clock_cw[2];

  int one_shot[2];
  int one_shot_cw[2];

  int FKHz_mask[2];
  int FMKHz_mask[2];
  int FMHz10_mask[2];
  int Atten_mask[2];
  int BEAM_mask[2];
  int ANTENNA_mask[2];
  int FILTER_mask[2];
  int CLOCK_mask[2];
  int TEST_mask[2];
  int TSG_mask[2];
  int AGCstatus_mask[2];
  int LOPWRstatus_mask[2];
  int Fstatus_bit[2];

  int control_word[4];

  int Atten_max;
  int BEAM_inv;
  int FREQ_inv;
  int FSTAT_inv;
  int AGCstatus_inv;
  int LOPWRstatus_inv;
  int Local_Oscillator;
}; 
