/* rxmldb.h
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
 $Log: rxmldb.h,v $
 Revision 1.1  2003/03/01 17:46:02  barnes
 Initial revision

*/

#ifndef _RXMLDB_H
#define _RXMLDB_H

struct XMLDBbuffer {
  int sze; 
  int max;
  int stp; 
 char *buf;
};

struct XMLDBtree {
  int depth;
  char **name;
};

struct XMLDBelement {
  char *name;
  int atnum;
  char **atname; 
  char **atval;
  struct XMLDBbuffer *data;
}; 

struct XMLDBtable {
  int num;
  struct XMLDBelement **element;
};

struct XMLDBdata {
  struct XMLdata *ptr;
  struct XMLDBtree *tree;
  struct XMLDBtable *doc;
  struct XMLDBbuffer *buf;
  struct XMLDBtable *table;

  struct {
    int (*user)(struct XMLDBtable *,struct XMLDBtable *,void *);
    void *data;
  } text;
 
  int depth;
};

struct XMLDBbuffer *XMLDBCopyBuffer(struct XMLDBbuffer *);

struct XMLDBbuffer *XMLDBMakeBuffer(int stp);
void XMLDBFreeBuffer(struct XMLDBbuffer *ptr);
int XMLDBAddBuffer(struct XMLDBbuffer *ptr,char *str,int sze);

struct XMLDBelement *XMLDBMakeElement(char *name,int atnum,
				      char **atname,char **atval,int stp);
void XMLDBFreeElement(struct XMLDBelement *ptr);


struct XMLDBtable *XMLDBMakeTable();
void XMLDBFreeTable(struct XMLDBtable *ptr);
int XMLDBRemoveTable(struct XMLDBtable *ptr);
int XMLDBAddTable(struct XMLDBtable *ptr,struct XMLDBelement *element);


struct XMLDBdata *XMLDBMake(struct XMLdata *xml);
void XMLDBFree(struct XMLDBdata *ptr);  

int XMLDBSetTree(struct XMLDBdata *ptr,struct XMLDBtree *tree);

int XMLDBSetText(struct XMLDBdata *ptr,
	      int (*text)(struct XMLDBtable *,struct XMLDBtable *,void *),
              void *data);

int XMLDBElementStart(char *name,int atnum,char **atname,char **atval,
		      char *buf,int sze,void *data);

int XMLDBElementEnd(char *name,char *buf,int sze,void *data);

int XMLDBElementText(char *buf,int sze,void *data);


int XMLDBPeerStart(char *name,int atnum,char **atname,char **atval,
		   char *buf,int sze,void *data);

int XMLDBPeerEnd(char *name,char *buf,int sze,void *data);

int XMLDBStart(char *name,int atnum,char **atname,char **atval,
	       char *buf,int sze,void *data);

int XMLDBEnd(char *name,char *buf,int sze,void *data);

struct XMLDBtree *XMLDBMakeTree();
void XMLDBFreeTree(struct XMLDBtree *ptr);
int XMLDBBuildTree(char *path,char delim,struct XMLDBtree *ptr);

#endif

  





  
