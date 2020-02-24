/* tsg.h
   =====
   Author: R.J.Barnes
*/


void init_tsg();
void zero_tsg();
int get_length(unsigned char seq_id);
void download(pid_t pid);
int send_seq_mono(int channel,
                  unsigned char seq_id,unsigned char flag,int *freq);
int send_seq_stereo(unsigned char *seq_id,unsigned int *delay,
                    unsigned char *flag,int **freq);
int send(pid_t pid);
