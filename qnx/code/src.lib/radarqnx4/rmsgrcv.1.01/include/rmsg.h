/* rmsg.h
   =====
   Author: R.J.Barnes
*/

/*
 Copyright 2004 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 "Radar Operating System" (hereinafter "the Program").
 
 The Program was developed at The Johns Hopkins University/Applied Physics
 Laboratory (JHU/APL) which is the author thereof under the "work made for
 hire" provisions of the copyright law.  
 
 JHU/APL assumes no obligation to provide support of any kind with regard to
 the Program.  This includes no obligation to provide assistance in using the
 Program or to provide updated versions of the Program.
 
 THE PROGRAM AND ITS DOCUMENTATION ARE PROVIDED AS IS AND WITHOUT ANY EXPRESS
 OR IMPLIED WARRANTIES WHATSOEVER.  ALL WARRANTIES INCLUDING, BUT NOT LIMITED
 TO, PERFORMANCE, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
 HEREBY DISCLAIMED.  YOU ASSUME THE ENTIRE RISK AND LIABILITY OF USING THE
 PROGRAM TO INCLUDE USE IN COMPLIANCE WITH ANY THIRD PARTY RIGHTS.  YOU ARE
 ADVISED TO TEST THE PROGRAM THOROUGHLY BEFORE RELYING ON IT.  IN NO EVENT
 SHALL JHU/APL BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING, WITHOUT
 LIMITATION, ANY LOST PROFITS, LOST SAVINGS OR OTHER INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, ARISING OUT OF THE USE OR INABILITY TO USE THE
 PROGRAM."
 
 
 
 
 
 
*/

/*
 $Log: rmsg.h,v $
 Revision 1.1  2004/07/12 22:36:36  barnes
 Initial revision

*/

#ifndef _RMSG_H
#define _RMSG_H

#define PRM_TYPE 0
#define FIT_TYPE 1
#define RAW_TYPE 2
#define NME_TYPE 3

#define MAX_BUF 32

#define TASK_OPEN	'O'
#define TASK_CLOSE	'C'	
#define TASK_QUIT	'Q'		
#define TASK_DATA   'd'
#define TASK_RESET  'r'

#define TASK_OK	 0	
#define TASK_ERR	'R'	
#define UNKNOWN_TYPE	0x7f

struct RMsgData {
  int type;
  int tag;
  int size;
  int index;
};
   
struct RMsgBlock {
  int num;
  int tsize;
  struct RMsgData data[MAX_BUF];
  unsigned char *ptr[MAX_BUF];
};

#endif
