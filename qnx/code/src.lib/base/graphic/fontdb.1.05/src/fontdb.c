/* fontdb.c
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
#include <math.h>
#include <ctype.h>
#include "rxml.h"
#include "rfbuffer.h"
#include "fontdb.h"

/*
 $Log: fontdb.c,v $
 Revision 1.5  2004/06/18 00:06:42  barnes
 Removed debugging code.

 Revision 1.4  2004/06/16 16:24:08  barnes
 Added missing header.

 Revision 1.3  2004/06/15 19:55:05  barnes
 Added the ability to define the path in terms of an environment variable.

 Revision 1.2  2004/04/26 19:50:37  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/01/24 20:43:45  barnes
 Initial revision

*/

int FrameBufferFontDBText(char *buf,int sze,void *data) {
  int o=0,c=0,i;
  char *tmp;
  struct FrameBufferFontDB *ptr;
  ptr=(struct FrameBufferFontDB *)data;
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


int FrameBufferFontDBStart(char *name,int atnum,char **atname,char **atval,
		  char *buf,int sze,void *data) {
  int i;
  struct FrameBufferFontDB *ptr;
  ptr=(struct FrameBufferFontDB *)data;
  if (strcmp(name,"font") !=0) return 0;
  if (ptr==NULL) return -1;
  if (ptr->buf !=NULL) free(ptr->buf);
  ptr->buf=NULL;
  ptr->flg=0;
  for (i=0;i<atnum;i++) if (strcmp(atname[i],"default")==0) ptr->flg=1;
  return 0;
}

int FrameBufferFontDBEnd(char *name,char *buf,int sze,void *data) {
  FILE *fp;
  char *fname=NULL;
  int s;
  struct FrameBufferFont *font=NULL;
  struct FrameBufferFontDB *ptr;
  ptr=(struct FrameBufferFontDB *)data;

  if (strcmp(name,"path")==0) {
    if (ptr->path !=NULL) free(ptr->path);
    ptr->path=ptr->buf;
    ptr->buf=NULL;
    return 0;
  }

  if (strcmp(name,"env")==0) {
    char *tmp;
    tmp=getenv(ptr->buf);
    if (tmp !=NULL) {
      if (ptr->path !=NULL) free(ptr->path);
      ptr->path=malloc(strlen(tmp)+1);
      if (ptr->path !=NULL) strcpy(ptr->path,tmp);
    }
    free(ptr->buf);
    ptr->buf=NULL;
    return 0;
  }


  if (strcmp(name,"font") !=0) return 0;
  if (ptr==NULL) return -1;
  if (ptr->buf==NULL) return 0;

  s=strlen(ptr->buf)+1;
  if (ptr->path !=NULL) s+=strlen(ptr->path)+1;
  fname=malloc(s);
  if (fname==NULL) return -1;
  if (ptr->path !=NULL) sprintf(fname,"%s/%s",ptr->path,ptr->buf);
  else sprintf(fname,ptr->buf);
 
  fp=fopen(fname,"r");
  free(fname);
  free(ptr->buf);
  ptr->buf=NULL;

  if (fp !=NULL) {
    font=FrameBufferLoadFont(fp);
    fclose(fp);
  }
  if (font==NULL) return -1;

  if (ptr->font==NULL) ptr->font=malloc(sizeof(struct FrameBufferFont *));
  else {
    struct FrameBufferFont **tmp;
    tmp=realloc(ptr->font,sizeof(struct FrameBufferFont *)*(ptr->num+1));
    if (tmp==NULL) return -1;
    ptr->font=tmp;
  }
  if (ptr->font==NULL) return -1;
  ptr->font[ptr->num]=font;
  if (ptr->flg) ptr->dfont=font;
  ptr->num++;

  return 0;
}


void FrameBufferFontDBFree(struct FrameBufferFontDB *ptr) {
  int i;
  if (ptr==NULL) return;
  if (ptr->path !=NULL) free(ptr->path);
  if (ptr->buf !=NULL) free(ptr->buf);
  if (ptr->font !=NULL) {
    for (i=0;i<ptr->num;i++) FrameBufferFreeFont(ptr->font[i]);
    free(ptr->font);
  }
  free(ptr);  
   
}

struct FrameBufferFontDB *FrameBufferFontDBLoad(FILE *fp) {
  int s=0;
  int i;
  char lbuf[256];
  struct XMLdata *xmldata=NULL;
  struct FrameBufferFontDB *ptr;


  if (fp==NULL) return NULL;  
  ptr=malloc(sizeof(struct FrameBufferFontDB));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->path=NULL;
  ptr->buf=NULL;
  ptr->font=NULL;
  ptr->dfont=NULL;
  
  xmldata=XMLMake();
  if (xmldata==NULL) {
    free(ptr);
    return NULL;
  }

  XMLSetStart(xmldata,FrameBufferFontDBStart,ptr);
  XMLSetEnd(xmldata,FrameBufferFontDBEnd,ptr);
  XMLSetText(xmldata,FrameBufferFontDBText,ptr);

  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }
  XMLFree(xmldata);
  if (s !=0) { 
    if (ptr->path !=NULL) free(ptr->path);
    if (ptr->buf !=NULL) free(ptr->buf);   
    if (ptr->font !=NULL) for (i=0;i<ptr->num;i++) FrameBufferFreeFont(ptr->font[i]);
    free(ptr->font);
    free(ptr);
    return NULL;
  }
  return ptr;
}


struct FrameBufferFont *FrameBufferFontDBFind(struct FrameBufferFontDB *ptr,char *name,int sze) {
  int n;
  int a,b;
  struct FrameBufferFont *fnt=NULL;
  struct FrameBufferFont *tst=NULL;
   
  if (ptr==NULL) return NULL;

  for (n=0;n<ptr->num;n++) {
    tst=ptr->font[n];
    if (strcmp(tst->name,name) !=0) continue;
    if (tst->sze==sze) {
      fnt=tst;
      break;
    }
    if (fnt==NULL) {
      fnt=tst;    
      continue;
    }
    a=fabs(tst->sze-sze);
    b=fabs(fnt->sze-sze);
    if (a<b) fnt=tst;
  }
  if (fnt==NULL) return ptr->dfont;
  return fnt;

}
 
