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


/*
 $Log: dash.c,v $
 Revision 1.1  2003/03/01 19:03:22  barnes
 Initial revision

*/


struct FrameBufferDash *FrameBufferMakeDash(int *p,int phase,int sze) {
  struct FrameBufferDash *ptr=NULL;

  int i;
  int l=0; 
  
  if (p==NULL) return NULL;
  if (sze==0) return NULL;


  ptr=malloc(sizeof(struct FrameBufferDash));
  if (ptr==NULL) return NULL;
  ptr->p=NULL;
 
  ptr->sze=sze;
  ptr->state=1;
  ptr->p=malloc(sizeof(int)*sze);
  if (ptr->p==NULL) {
    free(ptr);
    return NULL;
  }

  for (i=0;i<sze;i++) {
    l+=p[i];
    ptr->p[i]=l;
  }
  phase=phase-l*(phase/l); 
  for (i=0;(i<sze) && (phase>ptr->p[i]);i++);
  ptr->c=phase;
  ptr->i=i;

  return ptr;
}

void FrameBufferFreeDash(struct FrameBufferDash *ptr) {
  if (ptr==NULL) return;
  if (ptr->p !=NULL) free(ptr->p);
  free(ptr);
  return;
}



int FrameBufferDashState(struct FrameBufferDash *ptr) {
  int c,i;

  if (ptr==NULL) return 1;
  if (ptr->p==NULL)  return 1;
  c=ptr->c;
  i=ptr->i;
   
  if (c>=ptr->p[i]) { /* end of segment */
    ptr->state=1-ptr->state;
    if (i==ptr->sze-1) {
       ptr->i=0;
       ptr->c-=ptr->p[i];
    } else ptr->i++;     
  }
  ptr->c++;
  return ptr->state;
}


struct FrameBufferDash *FrameBufferMakeDashString(char *str) {
  struct FrameBufferDash *ptr=NULL;
  char *tmp=NULL;
  char *tok=NULL;
  int *itmp;

  int stp=10,max=10;
  int i,sze=0,phase;
  int s=0;
  int l=0; 
  
  
  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);


  ptr=malloc(sizeof(struct FrameBufferDash));
  if (ptr==NULL) {
    free(tmp);
    return NULL;
  }
  ptr->p=NULL;
  ptr->state=1;

  tok=strtok(tmp," ");
  if (tok==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }
  phase=atoi(tok);

  ptr->p=malloc(stp*sizeof(int));
  if (ptr->p==NULL) {
    free(tmp);
    free(ptr);
    return NULL;
  }  

  while ((tok=strtok(NULL," ")) !=NULL) {
    ptr->p[sze]=atoi(tok);
    sze++;
    if (sze==max) {
   
      max+=stp;
      itmp=realloc(ptr->p,max*sizeof(int));
      if (itmp==NULL) {
        s=-1;
        break;
      }
      ptr->p=itmp;
    } 
  }

  if (s==-1) {
    free(ptr->p);
    free(ptr);
    return NULL;
  }

  ptr->sze=sze;
  for (i=0;i<sze;i++) {
    l+=ptr->p[i];
    ptr->p[i]=l;
  }

  itmp=realloc(ptr->p,sze);
  if (itmp==0) {
    free(ptr->p);
    free(ptr);
    return NULL;
  }

  phase=phase-l*(phase/l); 
  for (i=0;(i<sze) && (phase>ptr->p[i]);i++);
  ptr->c=phase;
  ptr->i=i;

  free(tmp);
  return ptr;
}
