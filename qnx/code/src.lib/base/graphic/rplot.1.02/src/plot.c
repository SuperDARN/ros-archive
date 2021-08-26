/* plot.c
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
 $Log: plot.c,v $
 Revision 1.1  2003/03/14 20:00:16  barnes
 Initial revision

*/

int RplotMakePlot(struct Rplot *ptr,char *name,
                  float wdt,float hgt,int depth) {
  char quot='"';
  char buf[4096];
  char lbuf[256];
  

  if (ptr==NULL) return -1;
  if (name==NULL) sprintf(lbuf,"<rplot width=%c%g%c height=%c%g%c",
          quot,wdt,quot,quot,hgt,quot);
  else sprintf(lbuf,"<rplot name=%c%s%c width=%c%g%c height=%c%g%c",
               quot,name,quot,quot,wdt,quot,quot,hgt,quot);
  strcpy(buf,lbuf);
  sprintf(lbuf," depth=%c%.2d%c>\n",quot,depth,quot);
  strcat(buf,lbuf);
  if (ptr->text.user !=NULL) return (ptr->text.user)(buf,strlen(buf),
                                                       ptr->text.data);
  return 0;
}

int RplotEndPlot(struct Rplot *ptr) {
  char *buf="</rplot>\n";
  if (ptr==NULL) return -1;
  if (ptr->text.user !=NULL) return (ptr->text.user)(buf,strlen(buf),
                                                       ptr->text.data);
  return 0;
}



