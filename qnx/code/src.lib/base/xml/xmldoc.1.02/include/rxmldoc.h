/* rxmldoc.h
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

/*
 $Log: rxmldoc.h,v $
 Revision 1.1  2003/03/01 17:57:12  barnes
 Initial revision

*/

#ifndef _RXMLDOC_H
#define _RXMLDOC_H

struct XMLdocfile {
  char *path;
  char *fname;
  int type;
  int (*text)(char *,int,void *); /* renderer */
  void *data;
};


struct XMLdocmod {
  char *name;
  char *path;
  struct TagDBtable *tagdb;
   struct {
     int action; /* what form is the action text/file */
     int xml; /* what form is the XML data text/file */
  } type;
  struct XMLDBbuffer *action; 
};

struct XMLdocmodtable {
  int num;
  struct XMLdocmod **ptr;
};

struct XMLdocelement {
  char *name;
  char *path;
  struct TagDBtable *tagdb;
  struct {
    int action; /* what form is the action text/file */
    int defval; /* what form is the default value text/file */
    int xml; /* what form is the XML data text/file */
  } type;
  struct XMLDBbuffer *action;  
  struct XMLDBbuffer *defval;
  struct XMLdocmodtable mod;
};

struct XMLdoctable {
  int num;
  struct XMLdocelement **ptr;
};

struct XMLdocdata {
  struct TagDBtable *tagdb;
  struct TagDBdata *tag;
  struct Entitydata *ent;
  struct XMLDBbuffer *root; /* document root */
  struct XMLdoctable xml;
};

int XMLdocBuild(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data);
struct XMLdocdata *XMLdocMake();
void XMLdocFree(struct XMLdocdata *ptr);
int XMLdocSetText(struct XMLdocdata *ptr,
             int (*text)(char *,int,
                         void *),void *data);
int XMLdocRender(struct XMLDBtable *ptr,struct XMLdocdata *xmldoc,int index);

int XMLdocGetConfigSection(struct XMLDBbuffer *ptr,char *root,char *path,
              int (*text)(struct XMLDBtable *,struct XMLDBtable *,void *),
			   void *data);

char *getDBstring(struct XMLDBbuffer *ptr);
int XMLdocIncludeText(struct XMLdocdata *xmldoc,char *buf,int sze);


#endif


