/* scdoc.c
   ======= */

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
 $Log: scdoc.c,v $
 Revision 1.2  2005/08/09 13:32:32  barnes
 Added command line handling.

 Revision 1.1  2004/04/27 14:57:09  barnes
 Initial revision

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "option.h"
#include "rxml.h"
#include "rxmldb.h"
#include "rscript.h"
#include "rscriptHTML.h"
#include "entity.h"
#include "tagdb.h"
#include "rxmldoc.h"


#include "hlpstr.h"

int arg;
struct OptionData opt;


struct {
  int num;
  char **txt;
} search;
    
struct {
  int num;
  char **txt;
} replace;


int streamoutput(char *buf,int sze,void *data) {
  /* Output to a stream */
  FILE *outp;
  outp=(FILE *) data;
  fwrite(buf,sze,1,outp);
  return 0;
}

int decodetable(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {

  int i,s=0,n;
  struct XMLDBelement *e;
  struct XMLDBbuffer *dbuf=NULL;
  char *stxt=NULL;
  char *rtxt=NULL;

  char *symbol[]={"\"","\\","<",">","&",0};
  char *entity[]={"&quot;","&apos;","&lt;","&gt;","&amp;",0};
  struct Entitydata *entptr;

  entptr=EntityMake();
  if (entptr==NULL) return -1;
  EntityAddEntityStringArray(entptr,entity);
  EntityAddSymbolStringArray(entptr,symbol);

  for (i=0;i<ptr->num;i++) {
    e=ptr->element[i];
    if (strcmp(e->name,"search")==0) {
      if (stxt !=NULL) free(stxt);
      stxt=getDBstring(e->data);
      if (stxt==NULL) break;
    } else if (strcmp(e->name,"replace")==0) {
      if (rtxt !=NULL) free(rtxt);
      dbuf=EntityDecodeBuffer(entptr,e->data);
      if (dbuf !=NULL) {
	rtxt=getDBstring(dbuf);
        XMLDBFreeBuffer(dbuf);
      }
      if (rtxt==NULL) break;
     }
  }
  if (i<ptr->num) s=-1;  

  if ((s==0) && (stxt !=NULL)) {
    n=search.num; 
    if (s==0) {
      if (search.txt==NULL) 
        search.txt=malloc(sizeof(char *));
      else search.txt=realloc(search.txt,
                                     sizeof(char *)*(n+1));
      if (search.txt==NULL) s=-1;
      if (s==0) search.txt[n]=NULL;
    }


    if (s==0) {
      if (replace.txt==NULL) 
         replace.txt=malloc(sizeof(char *));
      else replace.txt=realloc(replace.txt,
                                     sizeof(char *)*(n+1));
      if (replace.txt==NULL) s=-1;
      if (s==0) replace.txt[n]=NULL;
    }

    if (s==0) {
      search.txt[n]=stxt;
      replace.txt[n]=rtxt;
      search.num++;
      replace.num++;
    }
  }
  free(entptr);
  if (s !=0) {
    if (stxt !=NULL) free(stxt);
    if (rtxt !=NULL) free(rtxt);
    return -1;
  }
  return 0;
}

int ssiproc(char *token,int sze,void *data) {
  fwrite(token,sze,1,stdout);
  return 0;
}

int loadtable(FILE *fp) {
  int s=0;
  char *epath="table/entry";
  char edelim='/';

  struct XMLdata *xmldata;
  struct XMLDBdata *xmldbdata;
  struct XMLDBtree *tree;

  char lbuf[255];

  xmldata=XMLMake();
  xmldbdata=XMLDBMake(xmldata);
  tree=XMLDBMakeTree();

  XMLDBBuildTree(epath,edelim,tree);   
  XMLDBSetTree(xmldbdata,tree);

  XMLDBSetText(xmldbdata,decodetable,NULL);
  
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata);
  
  while(fgets(lbuf,255,fp) !=NULL) {
    s=XMLDecode(xmldata,lbuf,strlen(lbuf)); 
    if (s !=0) break;
  
  }
  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  return 0;
}


int main(int argc,char *argv[]) {

  int s=0;
  FILE *fp;

  unsigned char help=0;
  unsigned char option=0;

  struct ScriptData *script;
  struct ScriptHTML *shtml;
  
  char lbuf[255];
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  } 

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }


  if (argc-arg<2) {
    fprintf(stderr,"scdoc table scfile\n");
    exit(-1);
  }

  fp=fopen(argv[1],"r");
  loadtable(fp);
  fclose(fp);

 
  script=ScriptMake();
  shtml=ScriptHTMLMake(script);
  ScriptSetText(script,HTMLWriter,shtml);
  ScriptHTMLSetText(shtml,streamoutput,stdout);
   
  ScriptHTMLSetSearch(shtml,search.num,search.txt);
  ScriptHTMLSetReplace(shtml,replace.num,replace.txt); 
  ScriptHTMLSetSsi(shtml,ssiproc,NULL);

  

  fp=fopen(argv[2],"r");
  if (fp !=NULL) {
    while(fgets(lbuf,255,fp) !=NULL) { 
      s=ScriptDecode(script,lbuf,strlen(lbuf));
      if (s !=0) break;
    }
    fclose(fp);
  }
  ScriptFree(script);
  fflush(stdout);  
  return 0;
}
