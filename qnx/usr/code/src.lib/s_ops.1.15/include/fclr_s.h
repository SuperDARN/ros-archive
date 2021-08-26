/* fclr_s.h
   ====== */

extern int fclr_id[2];
extern struct freq_table *frq_table;
void fclr_s(int debug,
           int start_freqA,
           int end_freqA,
           int step_freqA,
           int start_freqB,
           int end_freqB,
           int step_freqB,
           int *tfreqA,int *tfreqB);
 
