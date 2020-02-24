/* main.h
   ======
   Author: R.J.Barnes
*/

#define ANTENNA_BEAM 'b'
#define ANTENNA_AUTO 'a'
#define ANTENNA_FIX 'f'

   
struct dio_param {
  char stereo;
  char channel;
  char beam[2];
  char filter[2];
  char gain[2];
  int freq[2]; 
  char antenna_mode[2];
  char antenna[2];
  char test_mode[2];
  int seq_no;
  int seq_id[2];
  int status;
  int fstatus[2];
  
};
