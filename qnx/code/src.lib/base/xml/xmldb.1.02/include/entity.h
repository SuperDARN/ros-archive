/* entity.h
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

/*
 $Log: entity.h,v $
 Revision 1.1  2003/03/01 17:46:02  barnes
 Initial revision

*/


#ifndef _ENTITY_H
#define _ENTITY_H

struct Entitydata {
  struct {
    int num;
    struct XMLDBbuffer **buf;
    int *state;
  } entity;
  struct {
    int num;
    struct XMLDBbuffer **buf;
    int *state;
  } symbol;
  char *buf;
  int sze;
  int max;
  int stp;
  struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;

  struct {
    int (*trap)(char *,int,void *);
    void *data;
  } decode;

 struct {
    int (*trap)(char *,int,void *);
    void *data;
  } encode;


};

struct Entitydata *EntityMake();

void EntityFree(struct Entitydata *ptr);

void EntityFreeSymbol(struct Entitydata *ptr);
void EntityFreeEntity(struct Entitydata *ptr);

int EntitySetText(struct Entitydata *ptr,
		  int (*text)(char *,int,
			      void *),void *data);


int EntitySetEncodeTrap(struct Entitydata *ptr,
		  int (*text)(char *,int,
			      void *),void *data);

int EntitySetDecodeTrap(struct Entitydata *ptr,
		        int (*text)(char *,int,
			      void *),void *data);



int EntityAddEntity(struct Entitydata *ptr,struct XMLDBbuffer *entity);

int EntityAddSymbol(struct Entitydata *ptr,struct XMLDBbuffer *symbol);


int EntityAddEntityString(struct Entitydata *ptr,char *str);
int EntityAddSymbolString(struct Entitydata *ptr,char *str);

int EntityAddEntityStringArray(struct Entitydata *ptr,char **str);
int EntityAddSymbolStringArray(struct Entitydata *ptr,char **str);

int EntityDecode(char *buf,int sze,void *data);
int EntityEncode(char *buf,int sze,void *data);


struct XMLDBbuffer *EntityDecodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src);
struct XMLDBbuffer *EntityEncodeBuffer(struct Entitydata *ptr,
                                       struct XMLDBbuffer *src);

#endif


