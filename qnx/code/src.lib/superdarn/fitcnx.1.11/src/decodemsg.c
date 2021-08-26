/* decodemsg.c
   ===========
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
#include <string.h>
#include <sys/types.h>
#include "rtypes.h"
#include "rconvert.h"
#include "dmap.h"
#include "limit.h"
#include "rprm.h"
#include "fitdata.h"
#include "fitread.h"
#include "cnxstr.h"

/*
 $Log: decodemsg.c,v $
 Revision 1.6  2004/10/27 22:07:06  barnes
 Simplified code by using the FitDecode function

 Revision 1.5  2004/06/22 17:40:48  barnes
 Changed references of Long to Int.

 Revision 1.4  2004/05/05 15:41:32  barnes
 Added missing header.

 Revision 1.3  2004/05/04 18:27:58  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 22:15:52  barnes
 Audit - Enforced warning "all".

 Revision 1.1  2004/01/26 01:28:55  barnes
 Initial revision

*/


int FitCnxDecodeIPMsg(struct RadarParm *prm,struct FitData *fit,
                         unsigned char *buffer,int size) {
  struct DataMap *ptr;
  int s;

  ptr=DataMapDecodeBuffer(buffer,size);
  if (ptr==NULL) return 0;
  s=FitDecode(ptr,prm,fit);
  DataMapFree(ptr);
  return s;
}







