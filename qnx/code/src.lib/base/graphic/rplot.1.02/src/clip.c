/* clip.c
   ====== 
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
#include "rfbuffer.h"
#include "rplot.h"

/*
 $Log: clip.c,v $
 Revision 1.2  2004/04/26 19:55:44  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/03 22:07:37  barnes
 Initial revision

*/


int RplotClip(struct Rplot *ptr,
              int num,float *px,float *py,int *t) {

  int s=0;
  int n;
  char buf[4096];
  char lbuf[256];

  if (ptr==NULL) return -1;

  sprintf(lbuf,"<clip>\n");
  strcpy(buf,lbuf);
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
						  ptr->text.data);
  if (s !=0) return -1; 
  for (n=0;n<num;n++) {
    sprintf(buf,"<pp><x>%g</x><y>%g</y><t>%d</t></pp>\n",
            px[n],py[n],t[n]);
    if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
						    ptr->text.data);
    if (s !=0) return -1;
  }
  sprintf(buf,"</clip>\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  return s;
} 






