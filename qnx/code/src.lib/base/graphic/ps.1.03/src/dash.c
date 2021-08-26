/* dash.c
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
 $Log: dash.c,v $
 Revision 1.2  2004/04/26 19:53:27  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 18:50:43  barnes
 Initial revision

*/

struct PostScriptDash *PostScriptMakeDash(float *p,float phase,int sze) {
  struct PostScriptDash *ptr=NULL;

  int i;
  
  if (p==NULL) return NULL;
  if (sze==0) return NULL;

  ptr=malloc(sizeof(struct PostScriptDash));
  if (ptr==NULL) return NULL;
  ptr->p=NULL;
 
  ptr->sze=sze;
  ptr->p=malloc(sizeof(float)*sze);
  if (ptr->p==NULL) {
    free(ptr);
    return NULL;
  }

  for (i=0;i<sze;i++) {
    ptr->p[i]=p[i];
  }
  ptr->phase=phase;  
  return ptr;
}

struct PostScriptDash *PostScriptMakeDashString(char *str) {
  struct PostScriptDash *ptr=NULL;
  char *tmp=NULL;
  char *tok=NULL;
  float *ftmp=NULL;
  int stp=10,max=10;
  int sze=0;
  int s=0;
   
  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);

  ptr=malloc(sizeof(struct PostScriptDash));
  if (ptr==NULL) {
    free(tmp);
    return NULL;
  }
  ptr->p=NULL;

  tok=strtok(tmp," ");
  if (tok==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }
  ptr->phase=atof(tok);

  ptr->p=malloc(stp*sizeof(float));
  if (ptr->p==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }  

  while ((tok=strtok(NULL," ")) !=NULL) {
    ptr->p[sze]=atof(tok);
    sze++;
    if (sze==max) {
      max+=stp;
      ftmp=realloc(ptr->p,max*sizeof(float));
      if (ftmp==NULL) {
        s=-1;
        break;
      }
      ptr->p=ftmp;
    } 
  }

  if (s==-1) {
    free(ptr->p);
    free(ptr);
    return NULL;
  }

  ptr->sze=sze;
  ftmp=realloc(ptr->p,sze);
  if (ftmp==NULL) {
    free(ptr->p);
    free(ptr);
    return NULL;
  }

  free(tmp);
  return ptr;
}

void PostScriptFreeDash(struct PostScriptDash *ptr) {
  if (ptr==NULL) return;
  if (ptr->p !=NULL) free(ptr->p);
  free(ptr);
  return;
}
