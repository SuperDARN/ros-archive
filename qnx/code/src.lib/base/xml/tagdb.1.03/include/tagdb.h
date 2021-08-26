/* tagdb.c
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

/*
 $Log: tagdb.h,v $
 Revision 1.1  2003/03/01 17:33:29  barnes
 Initial revision

*/

#ifndef _TAGDB_H
#define _TAGDB_H

struct TagDBtag {
  char *name;
  struct {
    struct XMLDBbuffer *encode;
    struct XMLDBbuffer *decode;
  } start;

  struct {
    struct XMLDBbuffer *encode;
    struct XMLDBbuffer *decode;
  } end;

};

struct TagDBtable {
  int num;
  struct TagDBtag **tag;
};

struct TagDBdata {
  struct XMLdata *ptr;
  struct TagDBtable *tagdb;
  struct {
    int (*user)(char *,int,void *); 
    void *data;
  } text;
};

struct TagDBtable *TagDBMakeTable();
void TagDBFreeTable(struct TagDBtable *ptr);
int TagDBAddTable(struct TagDBtable *ptr,
             struct XMLDBbuffer *name,struct XMLDBbuffer *start,
             struct XMLDBbuffer *end); 
int TagDBBuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data);

 
int TagDBText(char *buf,int sze,void *data);
int TagDBEnd(char *name,char *buf,int sze,void *data);
int TagDBStart(char *name,int atnum,char **atname,char **atval,
	       char *buf,int sze,void *data);
struct TagDBdata *TagDBMake(struct TagDBtable *tagdb);
void TagDBFree(struct TagDBdata *ptr);
int TagDBSetTable(struct TagDBdata *ptr,struct TagDBtable *tagdb);
int TagDBSetText(struct TagDBdata *ptr,
		int (*text)(char *,int,void *),void *data);

int TagDBDecode(char *buf,int sze,void *data);

#endif
