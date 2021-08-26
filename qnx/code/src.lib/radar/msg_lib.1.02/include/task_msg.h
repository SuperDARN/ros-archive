/* task_msg.h
   ========== */

#ifndef _TASK_MSG
#define _TASK_MSG

#define MAX_BUF 32

#define TASK_OPEN	'O'
#define TASK_CLOSE	'C'	
#define TASK_QUIT	'Q'		
#define TASK_DATA   'd'
#define TASK_RESET  'r'

#define TASK_OK	 0	
#define TASK_ERR	'R'	
#define UNKNOWN_TYPE	0x7f

struct data_rec {
  double time;
  char type;
  int size;
  int index;
};
   
struct task_block {
  int num;
  int tsize;
  struct data_rec data[MAX_BUF];
  unsigned char *ptr[MAX_BUF];
};

struct task_decode {
  struct task_block block;
  unsigned char *store;
  unsigned char *comment;
  double opentme;
  double closetme;
  struct { 
     unsigned char *ptr;
     int size;
  } open;
};

#endif