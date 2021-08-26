/* rxml.h
   ===== 
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
 $Log: rxml.h,v $
 Revision 1.1  2003/03/01 17:37:10  barnes
 Initial revision

*/

#ifndef _RXML_H
#define _RXML_H

struct XMLinterface {
 struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;

  struct {
    int (*user)(char *,int,char **,char **,char *,int,void *);
    void *data;
  } start;


  struct {
    int (*user)(char *,char *,int,void *);
    void *data;
  } end;


  struct {
    int (*user)(char *,int,void *);
    void *data;
  } comment;

  struct {
    int (*user)(char *,int,void *);
    void *data;
  } process;
};

struct XMLdata {
  int sze;
  int max;
  int stp;
  char *buf;
  struct XMLinterface interface;
};


struct XMLdata *XMLMake();
 
void XMLFree(struct XMLdata *ptr);
 

int XMLSetText(struct XMLdata *ptr,
             int (*user)(char *,int,void *),void *data);
  

int XMLSetProcess(struct XMLdata *ptr,
		int (*user)(char *,int,void *),void *data);
  

int XMLSetComment(struct XMLdata *ptr,
		int (*user)(char *,int,void *),void *data);
  
int XMLSetStart(struct XMLdata *ptr,
             int (*user)(char *,int,char **,char **,
                         char *,int,void *),void *data);
int XMLSetEnd(struct XMLdata *ptr,
	    int (*user)(char *,char *,int,void *),void *data);

int XMLDecode(struct XMLdata *ptr,char *buf,int sze);
  
int XMLCallEnd(struct XMLdata *ptr,char *name,char *buf,int sze);

int XMLCallStart(struct XMLdata *ptr,
                 char *name,int atnum,char **atname,char **atval,
                 char *buf,int sze);
#endif
