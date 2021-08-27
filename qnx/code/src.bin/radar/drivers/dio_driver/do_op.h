/* program the DIO board 
   ==================== */

int put_filter(int channel,unsigned char filter);
int put_beam(int channel,unsigned char bmnum);
int put_antenna(int channel,unsigned char ant);
int put_Hz(int channel,int freq);
int get_freq_mode(int channel);
int put_gain(int channel,unsigned char atten);
int put_test_mode(int channel,unsigned char mode);
int read_agcstatus(int channel);
int read_lopwrstatus(int channel);

  



