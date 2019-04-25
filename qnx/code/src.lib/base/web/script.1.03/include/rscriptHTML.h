/* rscriptHTML.h
   ============= 
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
 $Log: rscriptHTML.h,v $
 Revision 1.2  2004/04/27 14:54:37  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/03/01 18:04:13  barnes
 Initial revision

*/

#ifndef _RSCRIPTHTML_H
#define _RSCRIPTHTML_H

struct ScriptHTML {
  struct ScriptData *ptr;

  struct {
    int num;
    char **txt;
  } search;

 struct {
    int num;
    char **txt;
  } replace;

 struct {
    int num;
    char **txt;
  } ignore;

 struct {
    int num;
    char **txt;
  } remove;

  struct {
    int num;
    char **txt;
  } post;

  int iflg; /* ignore flag */
  int pflg; /* post processing flag */

  struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;

  struct {
    int (*user)(char *,int,void *);
    void *data;
  } ssi;

 struct {
    int (*user)(char *,int,char *,void *);
    void *data;
  } postp;

};


struct ScriptHTML *ScriptHTMLMake(struct ScriptData *script);
 
void ScriptHTMLFree(struct ScriptHTML *ptr);
  
int ScriptHTMLSetText(struct ScriptHTML *ptr,
             int (*text)(char *,int,void *),void *data);
 
int ScriptHTMLSetSsi(struct ScriptHTML *ptr,
             int (*text)(char *,int,void *),void *data);

int ScriptHTMLSetPostp(struct ScriptHTML *ptr,
             int (*text)(char *,int,char *,void *),void *data);
 
int ScriptHTMLSetSearch(struct ScriptHTML *ptr,int num,char **search);

int ScriptHTMLSetReplace(struct ScriptHTML *ptr,int num,char **replace);
 
int ScriptHTMLSetIgnore(struct ScriptHTML *ptr,int num,char **ignore);
 
int ScriptHTMLSetRemove(struct ScriptHTML *ptr,int num,char **remove);
  
int ScriptHTMLSetPost(struct ScriptHTML *ptr,int num,char **post);

int ScriptHTMLSetSearchCount(struct ScriptHTML *ptr,char **search);

int ScriptHTMLSetReplaceCount(struct ScriptHTML *ptr,char **replace);
 
int ScriptHTMLSetIgnoreCount(struct ScriptHTML *ptr,char **ignore);
 
int ScriptHTMLSetRemoveCount(struct ScriptHTML *ptr,char **remove);
  
int ScriptHTMLSetPostCount(struct ScriptHTML *ptr,char **post);
 
int cmp_tag(char *token,char *str);

int cmp_tags(char *token,int num,char *str[]);
 
int HTMLWriter(char *buf,int sze,void *data);

#endif
 
