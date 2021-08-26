/* block.c
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
#include "rxml.h"
#include "block.h"

/*
 $Log: block.c,v $
 Revision 1.2  2004/06/16 16:28:38  barnes
 Added missing header.

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/

struct RplotInBlock *RplotInSave(struct XMLdata *xml,char *name,void *data) {
  struct RplotInBlock *ptr;
  ptr=malloc(sizeof(struct RplotInBlock));
  if (ptr==NULL) return NULL;
  ptr->name=malloc(strlen(name)+1);
  strcpy(ptr->name,name);
  ptr->xml=xml;
  memcpy(&(ptr->interface),&(xml->interface),sizeof(struct XMLinterface));
  ptr->data=data;
  return ptr;
}

int RplotInRestore(struct RplotInBlock *ptr) {
  struct XMLdata *xml;
  if (ptr==NULL) return -1;
  xml=ptr->xml;
  memcpy(&(xml->interface),&(ptr->interface),sizeof(struct XMLinterface));
  free(ptr->name);
  free(ptr);
  return 0;
}
   

