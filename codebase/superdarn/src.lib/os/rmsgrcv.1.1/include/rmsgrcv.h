/* rmsgrcv.h
   =========
   Author: R.J.Barnes
*/

/*
 $License$
*/


#ifndef _RMSGRCV_H
#define _RMSGRCV_H

char RMsgRcvDecodeData(int sock,struct RMsgBlock *blk,
		      unsigned char **store);
char RMsgRcvDecodeOpen(int sock,size_t *size,unsigned char **store);

#endif 
