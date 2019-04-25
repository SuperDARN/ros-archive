/* imagename.c
   =========== 
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
 $Log: imagename.c,v $
 Revision 1.2  2004/04/26 20:11:23  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/22 23:16:32  barnes
 Initial revision

*/


int RplotInImageNameStart(char *name,int atnum,char **atname,char **atval,
		       char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInImageName *ptr;
  struct RplotInPlot *plt;


  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInImageName *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;

  if (strcmp(name,blk->name)==0) {
    unsigned int mask;
    char tmp[64];
    int i;
    ptr->mask=0;
    ptr->smooth=0;
    ptr->matrix=NULL;
    ptr->x=0;
    ptr->y=0;
    ptr->name=NULL;
 
    for (i=0;i<atnum;i++) {
      if (strcmp(atname[i],"mask")==0) {
        strcpy(tmp,"0x");
        strcat(tmp,atval[i]);
        sscanf(tmp,"%x",&mask);
        ptr->mask=mask;
      }  else if (strcmp(atname[i],"smooth")==0) ptr->smooth=1;
      else if (strcmp(atname[i],"matrix")==0)
        ptr->matrix=RplotInMatrixString(atval[i]);
      else if (strcmp(atname[i],"position")==0)
        RplotInPointString(atval[i],&ptr->x,&ptr->y);
    }
    return 0;
  }
  
 if (strcmp(name,"name")==0) {
    struct RplotInString *str;
    str=malloc(sizeof(struct RplotInString));
    str->txt=&ptr->name;
    blk=RplotInSave(blk->xml,name,str);
    XMLSetStart(blk->xml,RplotInStringStart,blk);
    XMLSetText(blk->xml,RplotInStringText,blk);
    XMLSetEnd(blk->xml,RplotInStringEnd,blk);
    return XMLCallStart(blk->xml,name,atnum,atname,atval,buf,sze);
  }

  if (strcmp(name,"include")==0) {
    struct RplotInInclude *inc;
    inc=malloc(sizeof(struct RplotInInclude));
    inc->inc=&plt->inc;
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

int RplotInImageNameEnd(char *name,char *buf,int sze,void *data) {

  struct RplotInBlock *blk;
  struct RplotInImageName *ptr;
  struct RplotInPlot *plt;

  blk=(struct RplotInBlock *) data;
  if (blk==NULL) return -1;
  ptr=(struct RplotInImageName *) blk->data;
  if (ptr==NULL) return -1;
  plt=ptr->plt;
 
  if (strcmp(name,blk->name)==0) {
    int s=0;   
    struct RplotInMatrix *matrix=NULL;
  
    if (ptr->name==NULL) return -1;
    matrix=ptr->matrix;

    if (plt->matrix !=NULL) {
      RplotInMatrixTransform(plt->matrix,ptr->x,ptr->y,&ptr->x,&ptr->y);
      if (matrix !=NULL) RplotInMatrixMultiply(plt->matrix,matrix,matrix);
      else matrix=plt->matrix;
    }
    ptr->x+=plt->x;
    ptr->y+=plt->y;

    if (plt->imagename.user !=NULL) s=(plt->imagename.user)(
                                                    ptr->name,matrix,
                                                    ptr->x,ptr->y,
						    ptr->mask,ptr->smooth,
                                                    plt->imagename.data);

        
    free(ptr->name);
    free(ptr);   
    if (ptr->matrix !=NULL) free(ptr->matrix);
    RplotInRestore(blk);
    return s;
  }
  return 0;
}

