/* matrix.c
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
#include "rfbuffer.h"
#include "rps.h"

/*
 $Log: matrix.c,v $
 Revision 1.1  2004/01/24 20:41:50  barnes
 Initial revision

*/


#ifndef PI
  #define PI 3.14159265358979323846
#endif

struct PostScriptMatrix *PostScriptMatrixString(char *str) {
  struct PostScriptMatrix *ptr;
  char *tmp=NULL;
  char *tok=NULL;
  int s=0;

  if (str==NULL) return NULL;

  tmp=malloc(strlen(str)+1);
  if (tmp==NULL) return NULL;
  strcpy(tmp,str);

  ptr=malloc(sizeof(struct PostScriptMatrix));
  if (ptr==NULL) {
    free(tmp);
    return NULL;
  }
  
  tok=strtok(tmp," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->a=atof(tok);
  if (s==0) tok=strtok(NULL," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->b=atof(tok);
  if (s==0) tok=strtok(NULL," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->c=atof(tok);
  if (s==0) tok=strtok(NULL," ");
  if (tok==NULL) s=-1;
  if (s==0) ptr->d=atof(tok);

  free(tmp);
  if (s !=0) {
    free(ptr);
    return NULL;
  }
  return ptr; 
}

int PostScriptScaleMatrix(struct PostScriptMatrix *ptr,
                            float xscale,float yscale) {
  
  if (ptr==NULL) return -1;
  ptr->a=ptr->a*xscale;
  ptr->b=ptr->b*yscale;
  ptr->c=ptr->c*xscale;
  ptr->d=ptr->d*yscale;
 return 0;

}
        
int PostScriptRotateMatrix(struct PostScriptMatrix *ptr,
                      float angle) {
  float costh,sinth;
  float ma,mb,mc,md;
  if (ptr==NULL) return -1;
  
  costh=cos(PI*angle/180.0);
  sinth=sin(PI*angle/180.0);

  ma=ptr->a*costh-ptr->c*sinth;
  mb=ptr->b*costh-ptr->d*sinth;
  mc=ptr->a*sinth+ptr->c*costh;
  md=ptr->b*sinth+ptr->d*costh;

  ptr->a=ma;
  ptr->b=mb;
  ptr->c=mc;
  ptr->d=md;  
 return 0;

}
