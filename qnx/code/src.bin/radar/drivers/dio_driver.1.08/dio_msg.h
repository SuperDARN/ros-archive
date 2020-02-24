/* dio_msg.h
   =========
   Author: R.J.Barnes
*/

#define DIO_PRIO 15

#define STAT_THRESH 3

#define LOCAL_FREQ 2
#define UNKNOWN_TYPE 0x7f
#define DIO_OK 0
#define DIO_ERR 'R'
#define SET_GAIN 'G'
#define SET_BEAM 'B'
#define SET_FILTER 'X'
#define SET_FREQ 'F'
#define RESET_XT 'r'
#define SET_ANTENNA 'A'
#define TEST_MODE 'T'
#define SEND_FCLR 'c'

#define READ_GAIN 'g'
#define READ_BEAM 'b'
#define READ_FREQ 'f'
#define READ_FILTER 'x'

#define SET_TSG 'S'

#define SEND_TSG 'o'

#define VALID_ID 'V'

#define READ_CHANNEL 'k'
#define SET_CHANNEL 'l' 

#define GET_STATUS_NOFULL_NOCLR '0'
#define GET_STATUS_FULL_NOCLR '1'
#define GET_STATUS_NOFULL_CLR '2'
#define GET_STATUS_FULL_CLR '3'

#define GET_FREQ_MODE 'm'
#define GET_TSTATUS 's'
