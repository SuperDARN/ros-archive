/* pulse.c
   =======
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "rtypes.h"
#include "gc214.h"
#include "dio.h"

/*
 $Log: pulse.c,v $
 Revision 1.7  2006/02/07 17:04:23  barnes
 Fixed missing variable.

 Revision 1.6  2006/02/07 16:44:54  barnes
 Dieter Andre's modification to pass error codes back to the control
 program.

 Revision 1.5  2004/05/05 01:22:09  barnes
 Removed redundant code.

 Revision 1.4  2004/05/04 18:54:08  barnes
 Renamed the type header to avoid confusion.

 Revision 1.3  2004/05/03 20:07:07  barnes
 Changed type ipcid to ipcid_t.

 Revision 1.2  2004/05/03 15:30:09  barnes
 Added ipcid type for the IPC calls.

 Revision 1.1  2004/03/13 19:54:01  barnes
 Initial revision

*/

int IntegrateGC214Pulse(int buffer,int sample,
                        int channel,int tsgid,
	                ipcid_t dioid,
                        ipcid_t drid) {

  unsigned char xflg=0,sflg=0x01;
  unsigned char seq;
  int s;
  seq=tsgid; 

  if ((s=GC214DoScan(drid,buffer,sample,channel)) !=0) return -10+s;
  if ((s=DIOSendTSG(dioid,0,&seq,&xflg,&sflg,NULL,NULL)) != 0) return -20+s;
  return 0;
}


