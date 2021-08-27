/* dio.h 
   ===== */

#ifndef _DIO_DRIVER
#define _DIO_DRIVER
      
#define FREQ_LOCAL 2
#define UNKNOWN_TYPE 0x7f
#define DIO_OK 0
#define DIO_ERR 'R'
#define SET_GAIN 'G'
#define SET_BEAM 'B'
#define SET_FREQ 'F'
#define RESET_XT 'r'
#define SEND_FCLR 'c'
#define SEND_FILTER 'X'
#define SET_ANTENNA 'A'
#define TEST_MODE 'T' 

#define SET_CHANNEL 'l'

#define SET_TSG 'S'
#define GET_TSTATUS 's'

#define SEND_TSG 'o'

#define VALID_ID 'V'

#define GET_STATUS_NOFULL_NOCLR '0'
#define GET_STATUS_FULL_NOCLR '1'
#define GET_STATUS_NOFULL_CLR '2'
#define GET_STATUS_FULL_CLR '3'
#define GET_FREQ_MODE 'm'

#define PULSE_ATTEN 0x08
#define PULSE_TX 0x04
#define PULSE_RX 0x02
#define PULSE_SAMPLE 0x01
#define PULSE_PHASE 0x10

int set_gain(pid_t task_id,unsigned char atten);
int set_filter(pid_t task_id,unsigned char filter);
int set_beam(pid_t task_id,unsigned char beam);
int set_freq(pid_t task_id,int freq);
int reset_dio(pid_t task_id);
int set_tsg(pid_t task_id,unsigned char id,int length,char *code_byte,
			   char *rep_byte,char *xtd_byte);
int send_tsg(pid_t task_id,int stereo,unsigned char *id,
		     unsigned char *xtd_flag,unsigned char *st_flag,int *delay,
			 int **freq);
int verify_id(pid_t task_id,unsigned char id);
int send_fclr(pid_t task_id,int stereo,unsigned char *id,
              int *delay,int *freq_num,
              int **freq_table);
int set_antenna(pid_t task_id,unsigned char mode,unsigned char bmnum);
int set_test_mode(pid_t task_id,unsigned char mode);
int get_dio_status(pid_t task_id,int clear,int full,int *lstat,
				       int *astat);
#endif
