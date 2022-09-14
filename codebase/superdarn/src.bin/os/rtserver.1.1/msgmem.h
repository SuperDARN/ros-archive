/* msgmem.h
   =========
   Author: R.J.Barnes
*/

/*
 $License$
*/

int writeraw(char *buf,int sze); 
void readsock(fd_set *fdset,char *tmp_buf,int tmp_sze);
int writesock();



