/* text.c
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
 $Log: text.c,v $
 Revision 1.1  2003/03/10 21:55:33  barnes
 Initial revision

*/



int RplotText(struct Rplot *ptr,
	      struct RplotMatrix *matrix,
	      char *fname,float fsize,
              float x,float y,int num,char *txt,
              unsigned int color,unsigned  char mask,
              int sflg) {

  int s=0;
  int i,e;
  char quot='"';
  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};

  char buf[4096];
  char lbuf[256];

  if (ptr==NULL) return -1;

  sprintf(lbuf,"<text color=%c%.8x%c mask=%c%.2x%c font=%c%s%c size=%c%g%c",
               quot,color,quot,quot,mask,quot,quot,fname,quot,quot,fsize,quot);
  strcpy(buf,lbuf);

  sprintf(lbuf," position=%c%g %g%c",quot,x,y,quot);
  strcat(buf,lbuf);

  if (sflg !=0) {
    sprintf(lbuf," smooth=%c%d%c",quot,1,quot);
    strcat(buf,lbuf);
  }

 
  if (matrix !=NULL) {
   sprintf(lbuf," matrix=%c%g %g %g %g%c",quot,matrix->a,
           matrix->b,matrix->c,matrix->d,quot);
   strcat(buf,lbuf);
  }
  strcat(buf,">\n<t>");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);

  if (s !=0) return -1;

  for (i=0;i<num;i++) {
    for (e=0;symbol[e] !=NULL;e++) if (txt[i]==symbol[e][0]) break;
    if (symbol[e] !=NULL) {
       if (ptr->text.user !=NULL) s=(ptr->text.user)(entity[e],
                                                  strlen(entity[e]),
                                                  ptr->text.data); 
    } else {
       if (ptr->text.user !=NULL) s=(ptr->text.user)(txt+i,1,
                                                     ptr->text.data); 
    }
  }
  if (s !=0) return -1;

  
  sprintf(buf,"</t>\n</text>\n");
  if (ptr->text.user !=NULL) s=(ptr->text.user)(buf,strlen(buf),
                                      ptr->text.data);
  return s;
} 

