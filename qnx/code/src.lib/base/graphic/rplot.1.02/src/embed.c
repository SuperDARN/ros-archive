/* embed.c
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
#include <string.h>
#include "rfbuffer.h"
#include "rplot.h"

/*
 $Log: embed.c,v $
 Revision 1.1  2003/03/01 18:38:45  barnes
 Initial revision

*/


int RplotEmbed(struct Rplot *ptr,
	      struct RplotMatrix *matrix,
              float x,float y) {
  int s=0;
  char quot='"';
  char buf[4096];
  char lbuf[256];

  if (ptr==NULL) return -1;

  sprintf(lbuf,"<embed");
  strcpy(buf,lbuf);

  sprintf(lbuf," position=%c%g %g%c",quot,x,y,quot);
  strcat(buf,lbuf);

  if (matrix !=NULL) {
   sprintf(lbuf," matrix=%c%g %g %g %g%c",quot,matrix->a,
           matrix->b,matrix->c,matrix->d,quot);
   strcat(buf,lbuf);
  }
  strcat(buf,">\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  if (s !=0) return -1;
  return 0;
} 


int RplotEndEmbed(struct Rplot *ptr) {
  int s=0;
  char buf[4096];
 
  if (ptr==NULL) return -1;
  sprintf(buf,"</embed>\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
						  ptr->text.data);
  if (s !=0) return -1;
  return 0;
}


