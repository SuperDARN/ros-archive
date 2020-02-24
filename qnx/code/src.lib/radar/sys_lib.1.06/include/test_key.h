/* test_key.h
   ==========
   Author: R.J.Barnes
*/

struct key_str {
  int kb_fd;
  pid_t kb_proxy;
};

struct key_str *register_key(int console); 
void free_key(struct key_str *ptr);
int test_key(pid_t pid,struct key_str *ptr);
int read_key(pid_t pid,struct key_str *ptr,char *buf,int blen);
void key_wait(struct key_str *ptr);
 
