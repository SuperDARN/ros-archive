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
#include "rps.h"


/*
 $Log: plot.c,v $
 Revision 1.1  2003/03/03 15:53:47  barnes
 Initial revision

*/


int PostScriptMakePlot(struct PostScript *ptr,
                       float x,float y,float wdt,float hgt,
                       int land) {
 
  char buf[4096];
  char lbuf[256];
  
  if (ptr==NULL) return -1;
  ptr->x=x;
  ptr->y=y;
  ptr->wdt=wdt;
  ptr->hgt=hgt;
  ptr->land=land;
  ptr->pagenum=0;
  ptr->cnt=0;
  sprintf(lbuf,"%%!PS-Adobe-3.0\n");
  strcpy(buf,lbuf);
  sprintf(lbuf,"%%%%Title:Test Document\n");
  if (land==1) 
    sprintf(lbuf,"%%%%BoundingBox:%g %g %g %g\n",
            ptr->y,ptr->x,ptr->y+ptr->hgt,ptr->x+ptr->wdt);
  else 
     sprintf(lbuf,"%%%%BoundingBox:%g %g %g %g\n",
          ptr->x,ptr->y,
	  ptr->x+ptr->wdt,ptr->y+ptr->hgt);
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%PageOrder: Ascend\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Pages: (atend)\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%EndComments\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%BeginProlog\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"/bdef {bind def} bind def\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"/ldef {load def} bdef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/m /moveto ldef /l /lineto ldef /c /setrgbcolor ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/w /setlinewidth ldef /f /fill ldef /d /stroke ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/n /newpath ldef /e /closepath ldef /s /gsave  ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,
          "/r /grestore ldef /o /currentpoint ldef /t /show ldef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"/b /curveto ldef\n"); 
  strcat(buf,lbuf);
  sprintf(lbuf,"/F {findfont exch scalefont setfont } bdef\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%EndProlog\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Page: %d %d\n",ptr->pagenum,ptr->pagenum+1);
  strcat(buf,lbuf);

  if (land==1) {
    sprintf(lbuf,"%%%%PageOrientation: Landscape\n");
    strcat(buf,lbuf);
  }
  sprintf(lbuf,"gsave\n");
  strcat(buf,lbuf);

  if (land==1) {
    sprintf(lbuf,"%g %g translate 90 rotate\n",2*ptr->y+ptr->hgt,
            0.0);
    strcat(buf,lbuf);
  }

  sprintf(lbuf,"0 0 0 c %g %g m 0.5 w ",ptr->x,ptr->y+ptr->hgt);
  strcat(buf,lbuf);

  ptr->px=0;
  ptr->py=0;
  ptr->width=0.5;
  ptr->color=0;
  ptr->dash=NULL;
  ptr->clip=NULL;
  ptr->pagenum=1;

  if (ptr->text.user !=NULL) return (ptr->text.user)(buf,strlen(buf),
                                                     ptr->text.data);
  return 0;
}

int PostScriptNewPage(struct PostScript *ptr) {


  char buf[4096];
  char lbuf[256];
  
  if (ptr==NULL) return -1;

  sprintf(lbuf,"\nstroke\n");
  strcpy(buf,lbuf);
  sprintf(lbuf,"%%%%PageTrailer\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"grestore\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"showpage\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Page: %d %d\n",ptr->pagenum,ptr->pagenum+1);
  strcat(buf,lbuf);
 
  if (ptr->land==1) { 
     sprintf(lbuf,"%%%%PageOrientation: Landscape\n");
     strcat(buf,lbuf);
  }

  sprintf(lbuf,"gsave\n");
  strcat(buf,lbuf);

  if (ptr->land==1) { 
    sprintf(lbuf,"%g %g translate 90 rotate\n",2*ptr->y+
            ptr->hgt,0.0);
    strcat(buf,lbuf);
  }
  ptr->pagenum++;
  if (ptr->text.user !=NULL) return (ptr->text.user)(buf,strlen(buf),
                                                     ptr->text.data);

  return 0;
}


int PostScriptEndPlot(struct PostScript *ptr) {
 
  char buf[4096];
  char lbuf[256];
  
  if (ptr==NULL) return -1;


  sprintf(lbuf,"\nstroke\n");
  strcpy(buf,lbuf);
  sprintf(lbuf,"%%%%PageTrailer\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"grestore\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"showpage\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Trailer\n");
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%Pages: %d\n",ptr->pagenum);
  strcat(buf,lbuf);
  sprintf(lbuf,"%%%%EOF\n");
  strcat(buf,lbuf);

  if (ptr->text.user !=NULL) return (ptr->text.user)(buf,strlen(buf),
                                                     ptr->text.data);

  return 0;

}
