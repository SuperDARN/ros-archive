/* element.c
   ========= 
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
#include "rxmldb.h"

/*
 $Log: element.c,v $
 Revision 1.2  2004/04/26 20:27:45  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 17:42:50  barnes
 Initial revision

*/

struct XMLDBelement *XMLDBMakeElement(char *name,int atnum, 
                                     char **atname,char **atval,int stp) {
  int s=0,i=0;
  struct XMLDBelement *ptr;

  ptr=malloc(sizeof(struct XMLDBelement));
  if (ptr==NULL) return NULL;
 
  ptr->name=malloc(strlen(name)+1);  
  if (ptr->name==NULL) {
    free(ptr);
    return NULL;
  }

  strcpy(ptr->name,name);
  ptr->data=NULL;
  ptr->atnum=atnum;
  ptr->atname=NULL;
  ptr->atval=NULL;

  if (atnum !=0) {
    ptr->atname=malloc(sizeof(char *)*atnum);
    if (ptr->atname==NULL) s=-1;
    if (s==0) ptr->atval=malloc(sizeof(char *)*atnum);
    if (ptr->atval==NULL) s=-1;

    if (s==0) for (i=0;i<atnum;i++) {
      ptr->atname[i]=NULL;
      ptr->atval[i]=NULL;
 
      if (atname[i] !=NULL) {
	ptr->atname[i]=malloc(strlen(atname[i])+1);
        if (ptr->atname[i]==NULL) break;
        strcpy(ptr->atname[i],atname[i]);
      }

      if (atval[i] !=NULL) {
	ptr->atval[i]=malloc(strlen(atval[i])+1);
        if (ptr->atval[i]==NULL) break;
        strcpy(ptr->atval[i],atval[i]);
      }
      
    }
    if (i<atnum) s=-1;
    ptr->atnum=atnum;
  }

  

  if (s==0) ptr->data=XMLDBMakeBuffer(stp);
  if (ptr->data==NULL) s=-1;

  if (s !=0) {
    for (i=0;i<atnum;i++) {
      if ((ptr->atname !=NULL) && (ptr->atname[i] !=NULL)) 
        free(ptr->atname[i]);
      if ((ptr->atval !=NULL) && (ptr->atval[i] !=NULL)) 
        free(ptr->atval[i]);
    }
    if (ptr->atname !=NULL) free(ptr->atname);
    if (ptr->atval !=NULL) free(ptr->atval);
    if (ptr->data !=NULL) XMLDBFreeBuffer(ptr->data);
    free(ptr->name);
    free(ptr);
    return NULL;
  }
 
  return ptr;
}

void XMLDBFreeElement(struct XMLDBelement *ptr) {
  int i;

  if (ptr==NULL) return;
  if (ptr->data !=NULL) XMLDBFreeBuffer(ptr->data);
  if (ptr->name !=NULL) free(ptr->name);

  if (ptr->atnum>0) {
    for (i=0;i<ptr->atnum;i++) {
      if ((ptr->atname !=NULL) && (ptr->atname[i] !=NULL)) 
          free(ptr->atname[i]);
      if ((ptr->atval !=NULL) && (ptr->atval[i] !=NULL)) free(ptr->atval[i]);
    }
    if (ptr->atname !=NULL) free(ptr->atname);
    if (ptr->atval !=NULL) free(ptr->atval);
  }
  free(ptr);

}







  


