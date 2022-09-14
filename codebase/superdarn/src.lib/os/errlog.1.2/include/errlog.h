/* errlog.h
   ========
   Author: R.J.Barnes
*/


/* 
 $License$
*/

#ifndef _ERRLOG_H
#define _ERRLOG_H

#define ERROR_MSG 'M'
#define ERROR_OK 'O'
#define ERROR_FAIL 'F'

int ErrLog(int sock,char *name,char *buffer);

#endif
