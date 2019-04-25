/* rscript.c
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
#include "rscript.h"

/*
 $Log: rscript.c,v $
 Revision 1.2  2004/04/26 20:23:17  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 18:03:27  barnes
 Initial revision

*/

struct ScriptData *ScriptMake() {
  struct ScriptData *ptr;
  ptr=malloc(sizeof(struct ScriptData));
  if (ptr==NULL) return NULL;
  ptr->buf=NULL;
  ptr->depth=0;
  ptr->sze=0;
  ptr->max=0;
  ptr->stp=100;
  ptr->text.user=NULL;
  return ptr;
}

void ScriptFree(struct ScriptData *ptr) {
  if (ptr==NULL) return;
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

int ScriptSetText(struct ScriptData *ptr,
             int (*text)(char *,int,void *),void *data) {

  if (ptr==NULL) return -1;
  ptr->text.user=text;
  ptr->text.data=data;
  return 0;
} 


int ScriptDecode(struct ScriptData *ptr,char *buf,int sze) {
  int status=0;
  int c=0;
  int s=0;
          
  while(c<sze) {

    if (buf[c]=='<') { 
      
      if ((ptr->depth==0) && ((c-s) !=0) && (ptr->text.user !=NULL)) 
         status=(ptr->text.user)(buf+s,c-s,ptr->text.data);
      
      if (status !=0) return status;
      if (ptr->buf==NULL) { 
        ptr->buf=malloc(ptr->stp);
        if (ptr->buf==NULL) return -1;
        ptr->max=ptr->stp;
        ptr->sze=0;
      }
      ptr->buf[ptr->sze]='<';
      ptr->sze++;
      ptr->depth++;
    
    } else if (buf[c]=='>') {
    
      if (ptr->buf == NULL) return -1;

      ptr->buf[ptr->sze]='>';
      ptr->sze++;
      ptr->depth--;

      /* at this point we can user the tag on */

      if (ptr->depth==0) {   
        char *tmpbuf;
        int tmpsze;

        /* make temporary copies of the buffer pointer and its size */

        tmpbuf=ptr->buf;
        tmpsze=ptr->sze;

        /* reset the structure so that recursion is possible */

        ptr->sze=0;
        ptr->buf=NULL;
        ptr->max=0;

        status=(ptr->text.user)(tmpbuf,tmpsze,ptr->text.data);

        if (tmpbuf !=NULL) free(tmpbuf);
        if (status !=0) return status;
        s=c+1;
      }
    } else { /* text data or tag contents */
      if (ptr->buf !=NULL) { /* part of a tag */
        ptr->buf[ptr->sze]=buf[c];
        ptr->sze++;
        if (ptr->sze==ptr->max) {
          char *tmp;
          ptr->max+=ptr->stp;
          tmp=realloc(ptr->buf,ptr->max);
          if (tmp==NULL) return -1;
          ptr->buf=tmp;
	}
      } 
    }
    c++;
  }
  if ((ptr->buf==NULL) && ((c-s) !=0) && (ptr->text.user !=0)) 
     status=(ptr->text.user)(buf+s,c-s,ptr->text.data);
  
  return status;
}

