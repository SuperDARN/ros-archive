/* float.c
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
#include <ctype.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "block.h"
#include "rplotin.h"

/*
 $Log: float.c,v $
 Revision 1.3  2004/06/16 16:28:38  barnes
 Added missing header.

 Revision 1.2  2004/04/26 20:11:23  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/


int RplotInFloatText(char *buf,int sze,void *data) {
  struct RplotInBlock *blk;
  struct RplotInFloat *ptr;

  char *tmp;
  int o=0,c=0,i;
  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInFloat *) blk->data;
  if (ptr==NULL) return -1;

  if (buf==NULL) return 0;
  if (sze==0) return 0;

  for (i=0;i<sze;i++) if (!isspace(buf[i])) c++;

  if (ptr->buf !=NULL) o=strlen(ptr->buf);
  if (ptr->buf==NULL) ptr->buf=malloc(c+1);
  else {
    tmp=realloc(ptr->buf,o+c+1);
    if (tmp==NULL) return -1;
    ptr->buf=tmp;
  }
  if (ptr->buf==NULL) return -1;
  c=0;
  for (i=0;i<sze;i++) if (!isspace(buf[i])) {
    ptr->buf[o+c]=buf[i];
    c++;
  }
  ptr->buf[o+c]=0;
  return 0;
}


int RplotInFloatStart(char *name,int atnum,char **atname,char **atval,
                      char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInFloat *ptr;
  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInFloat *) blk->data;
  if (ptr==NULL) return -1;

  if (strcmp(name,blk->name)==0) {
    ptr->buf=NULL;
    return 0;
  }

  if (strcmp(name,"include")==0) {
    struct RplotInInclude *inc;
    inc=malloc(sizeof(struct RplotInInclude));
    inc->inc=ptr->inc;
    blk=RplotInSave(blk->xml,name,inc);
    XMLSetStart(blk->xml,RplotInIncludeStart,blk);
    XMLSetEnd(blk->xml,RplotInIncludeEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"ignore")==0) {
    blk=RplotInSave(blk->xml,name,NULL);
    XMLSetStart(blk->xml,RplotInIgnoreStart,blk);
    XMLSetText(blk->xml,RplotInIgnoreText,blk);
    XMLSetEnd(blk->xml,RplotInIgnoreEnd,blk);
    return 0;
  }

  return 0;
}

int RplotInFloatEnd(char *name,char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInFloat *ptr;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInFloat *) blk->data;
  if (ptr==NULL) return -1;
 
  if (strcmp(name,blk->name)==0) {
    if (ptr->buf==NULL) return -1;
    *(ptr->val)=atof(ptr->buf);
    free(ptr->buf);
    free(ptr);
    RplotInRestore(blk);
    return 0;
  }
  return 0;
}

