/* decode.c
   ======== 
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
#include "rxml.h"
#include "rfbuffer.h"
#include "block.h"
#include "rplotin.h"

/*
 $Log: decode.c,v $
 Revision 1.2  2004/06/16 16:28:38  barnes
 Added missing header.

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/

int RplotInStart(char *name,int atnum,char **atname,char **atval,
		  char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInPlot *root;
  struct RplotInPlot *plt;

  float wdt=0,hgt=0;
  int depth=0;
  char *nstr=NULL;
  int i,s=0;

  root=(struct RplotInPlot *) data;
  if (root==NULL) return -1;

  if (strcmp(name,"rplot")!=0) {
    char *wrap="<>"; 
    if (root->pass.user !=NULL) {
      s=(root->pass.user)(wrap,1,root->pass.data);
      if (s==0) s=(root->pass.user)(buf,sze,root->pass.data);
      if (s==0) s=(root->pass.user)(wrap+1,1,root->pass.data);
    }
    return s;
  }

  for (i=0;i<atnum;i++) {
    if ((strcmp(atname[i],"name")==0) && (atval[i] !=NULL)) nstr=atval[i];
    if ((strcmp(atname[i],"width")==0) && (atval[i] !=NULL))
      wdt=atof(atval[i]);
    if ((strcmp(atname[i],"height")==0) && (atval[i] !=NULL))
      hgt=atof(atval[i]);
    if ((strcmp(atname[i],"depth")==0) && (atval[i] !=NULL))
      depth=atoi(atval[i]);
  }

  if (depth==0) depth=24; /* default to 24 bit color */
  if (wdt==0) return -1;
  if (hgt==0) return -1;

  if (root->make.user !=NULL) 
    s=(root->make.user)(nstr,depth,wdt,hgt,root->make.data);

  if (s !=0) return s;
 
  plt=malloc(sizeof(struct RplotInPlot));
  memcpy(plt,root,sizeof(struct RplotInPlot));
 
  blk=RplotInSave(root->xml,"rplot",plt);
  XMLSetStart(blk->xml,RplotInPlotStart,blk);
  XMLSetEnd(blk->xml,RplotInPlotEnd,blk);
  return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze); 
}

int RplotInEnd(char *name,char *buf,int sze,void *data) {
  int s=0;
  struct RplotInPlot *plt;
  
  plt=(struct RplotInPlot *) data;
  if (plt==NULL) return -1;

  if (strcmp(name,"rplot")!=0) {
    char *wrap="<>"; 
    if (plt->pass.user !=NULL) {
      s=(plt->pass.user)(wrap,1,plt->pass.data);
      if (s==0) s=(plt->pass.user)(buf,sze,plt->pass.data);
      if (s==0) s=(plt->pass.user)(wrap+1,1,plt->pass.data);
    }
    return s;
  }

  if (plt->end.user !=NULL) 
    s=(plt->end.user)(plt->end.data);

  return s;
}

int RplotInText(char *buf,int sze,void *data) {
  int s=0;
  struct RplotInPlot *plt;
  
  plt=(struct RplotInPlot *) data;
  if (plt==NULL) return -1;

  if (plt->pass.user !=NULL) s=(plt->pass.user)(buf,sze,plt->pass.data);

  return s;
}



