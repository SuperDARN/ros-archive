/* msg_mem.h
   ========= */

int write_raw(char *buf,int sze); 
void read_sock(struct fd_set *fdset,char *tmp_buf,int tmp_sze);
int write_sock();



