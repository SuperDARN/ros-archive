/* echo_util.h
   ===========
   Author: R.J.Barnes
*/

#define ECHO_ADD 'a'
#define ECHO_REMOVE 'r'
#define ECHO_QUIT 'q'
#define ECHO_OK 'o'

#define PASS_RAW 0x01
#define PASS_FIT 0x10
#define PASS_AUX 0x08

/* name of the echo task */

int echo_register(char *name,char *echo_name,char *pass,int plen);
