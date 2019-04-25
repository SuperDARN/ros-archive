/* xmldoc.c
   ======== */

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
 $Log: xmldoc.c,v $
 Revision 1.9  2005/08/05 20:36:17  barnes
 Added option to dump command line options.

 Revision 1.8  2004/11/17 14:23:40  barnes
 Added the ability to filter out line feeds in the XML input file.

 Revision 1.7  2004/04/27 14:58:48  barnes
 Audit - Enforced warning set to "all".

 Revision 1.6  2004/04/26 18:46:49  barnes
 Cleaned up the implmenentation of the script reader.

 Revision 1.5  2003/09/08 20:53:04  barnes
 Added trap for missing command line options.

 Revision 1.4  2003/04/25 14:58:36  barnes
 Changed library calls.

 Revision 1.3  2003/02/10 03:00:09  barnes
 Added the ability to specify inline scripts.

 Revision 1.2  2003/02/10 00:13:46  barnes
 Added code to deal with top and tail scripts.

 Revision 1.1  2003/02/09 23:59:46  barnes
 Initial revision

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "option.h"
#include "rscript.h"
#include "rxml.h"
#include "rxmldb.h"
#include "entity.h"
#include "tagdb.h"

#include "rxmldoc.h"
#include "xmldoc.h"
#include "loadconfig.h"

#include "hlpstr.h"

int arg;
struct OptionData opt;


struct xmldoc xmldoc;

int stream_output(char *buf,int sze,void *data) {
  /* Output to a stream */
  FILE *outp;
  outp=(FILE *) data;
  fwrite(buf,sze,1,outp);
  return 0;
}

int cmp(char *name,struct XMLDBbuffer *ptr,void *data) {
  
  /* Search for a string in an XMLDB buffer structure */

  char zero[1];
  char *dst;
  int s=0;
  struct XMLDBbuffer *src=NULL;

  zero[0]=0;
  dst=(char *)data;
 
  src=XMLDBCopyBuffer(ptr);
  if (src==NULL) return -1;
  XMLDBAddBuffer(src,zero,1);
  if (strcmp(src->buf,dst)==0) s=1;
  XMLDBFreeBuffer(src); 
  return s;
  
}

int render(struct XMLDBtable *ptr,struct XMLDBtable *tree,void *data) {
  int i,j;
  int c=0,s=0;
  struct xmldoc *xmldoc; 
  struct XMLdocdata *doc;
  struct XMLDBelement *e;
  char lbuf[255];
  FILE *fp;


  xmldoc=(struct xmldoc *) data; 
  doc=xmldoc->doc;
  
  if ((xmldoc->compare.data !=NULL) && (xmldoc->match.txt !=NULL)) {
    
     for (i=0;i<ptr->num;i++) {
       e=ptr->element[i];
       for (j=0;j<xmldoc->match.num;j++) {
         if ((xmldoc->match.txt[j] !=NULL) && 
             (strcmp(xmldoc->match.txt[j],e->name)==0) && 
             (xmldoc->compare.check !=NULL)) {
	   c=(xmldoc->compare.check)(xmldoc->match.txt[j],e->data,
                                   xmldoc->compare.data);
           /* Match found */

           if (c !=0) break;
         }
       }
       if (c !=0) break;
     }
     /* No match */

     if (c==0) return 0;
   }
   
  xmldoc->table=ptr;
  
  if (xmldoc->sc.buf==NULL) return 0; /* nothing to do */

  if (xmldoc->sc.type==1) {
    fp=fopen((xmldoc->sc.buf)->buf,"r");
    if (fp==NULL) return 0;

     while(fgets(lbuf,255,fp) !=NULL) { 
       s=ScriptDecode(xmldoc->script,lbuf,strlen(lbuf));
       if (s !=0) break;
     }
     fclose(fp);
  } else s=ScriptDecode(xmldoc->script,(xmldoc->sc.buf)->buf,
		       (xmldoc->sc.buf)->sze);

  return 0;
}


int mapxml(char *buf,int sze,void *data) {
  int i,j,status;
  struct XMLdocdata *doc;
  struct xmldoc *xmldoc;
  struct ScriptData *script;
  xmldoc=(struct xmldoc *) data;

  doc=xmldoc->doc;
  script=xmldoc->script;
  if ((sze>7) && 
      (buf[0]=='<') && (buf[1]=='!') && (buf[2]=='-') && 
      (buf[3]=='-') && (buf[sze-2]=='-') && (buf[sze-3]=='-')) {
  
      char *tag;
      struct XMLdocelement *e;
     
      tag=malloc(sze-6);
      if (tag==NULL) return -1;
      memcpy(tag,buf+4,sze-7);
      tag[sze-7]=0;   

     
      if (xmldoc->map.search.txt !=NULL) {
        for (i=0;i<xmldoc->map.search.num;i++) {
          if ((xmldoc->map.search.txt[i] !=NULL) &&
             (strcmp(xmldoc->map.search.txt[i],tag)==0)) break;
	}
         

        if (i<xmldoc->map.search.num) {
          if ((xmldoc->map.replace.txt==NULL) || 
              (xmldoc->map.replace.txt[i]==NULL))
	    return 0;  
          for (j=0;j<doc->xml.num;j++) {
            e=doc->xml.ptr[j];
            if (strcmp(e->name,xmldoc->map.replace.txt[i])==0) 
	      break;
	  } 
          if (j<doc->xml.num) XMLdocRender(xmldoc->table,doc,j);
          return 0;
        }
      }

      if (xmldoc->external.search.txt !=NULL) {
        for (i=0;i<xmldoc->external.search.num;i++) {
          if ((xmldoc->external.search.txt[i] !=NULL) &&
             (strcmp(xmldoc->external.search.txt[i],tag)==0)) break;
	}
         

        if (i<xmldoc->external.search.num) {
          if ((xmldoc->external.replace.txt==NULL) || 
              (xmldoc->external.replace.txt[i]==NULL))
	    return 0;  
         
	  XMLdocIncludeText(xmldoc->doc,xmldoc->external.replace.txt[i],
                            strlen(xmldoc->external.replace.txt[i]));
          return 0;

        }
      }
  } else if ((buf[0]=='<') && (buf[sze-1]=='>')) {
    status=XMLdocIncludeText(xmldoc->doc,"<",1);
    if (status==0) status=ScriptDecode(script,buf+1,sze-2);
    if (status==0) status=XMLdocIncludeText(xmldoc->doc,">",1);
    return status;
  }
  return XMLdocIncludeText(xmldoc->doc,buf,sze);
}


int main(int argc,char *argv[]) {
  int s=0,m,n;
  FILE *fp;
  FILE *xmlfp;
  unsigned char help=0;
  unsigned char option=0;
  unsigned char ilf=0; 
 
  
  struct XMLdata *xmldata=NULL;
  struct XMLDBdata *xmldbdata=NULL;
  struct XMLDBtree *tree=NULL;

  char lbuf[255];
 
  OptionAdd(&opt,"-help",'x',&help);
  OptionAdd(&opt,"-option",'x',&option);


  OptionAdd(&opt,"ilf",'x',&ilf);

  arg=OptionProcess(1,argc,argv,&opt,NULL);   

  if (help==1) {
    OptionPrintInfo(stdout,hlpstr);
    exit(0);
  } 

  if (option==1) {
    OptionDump(stdout,&opt);
    exit(0);
  }
 

  if ((argc-arg)<3) {
    fprintf(stderr,"xmldoc pwd cfgfile xmldata\n");
    exit(-1);
  }
 
  xmlfp=fopen(argv[arg+2],"r");
  if (xmlfp==NULL) {
    fprintf(stderr,"file not found.\n");
    exit(-1);
  }

  chdir(argv[arg]);

  fp=fopen(argv[arg+1],"r");
  loadconfig(fp,&xmldoc);
  fclose(fp);

 
  if (argc>3) xmldoc.compare.data=argv[arg+3];
  else xmldoc.compare.data=NULL;

  xmldoc.compare.check=cmp;  
 
  XMLdocSetText(xmldoc.doc,stream_output,stdout);
  
  xmldata=XMLMake();
  xmldbdata=XMLDBMake(xmldata);
  tree=XMLDBMakeTree();

  XMLDBBuildTree(xmldoc.tree.path,xmldoc.tree.delim,tree);
  XMLDBSetTree(xmldbdata,tree);
    
  /* Set the renderer to XMLrender */

  XMLDBSetText(xmldbdata,render,&xmldoc);
  XMLSetStart(xmldata,XMLDBStart,xmldbdata);
  XMLSetEnd(xmldata,XMLDBEnd,xmldbdata); 
  
  xmldoc.script=ScriptMake();
  ScriptSetText(xmldoc.script,mapxml,&xmldoc);
 
  while(fgets(lbuf,255,xmlfp) !=NULL) {
    if (ilf) {
      m=0;
      for (n=0;(lbuf[n] !=0) && (n<256);n++) {
        if (lbuf[n]=='\n') continue;
        lbuf[m]=lbuf[n];
        m++;
      }    
      lbuf[m]=0;
    }
    s=XMLDecode(xmldata,lbuf,strlen(lbuf));
    if (s !=0) break;
  }

  XMLFree(xmldata);
  XMLDBFree(xmldbdata);
  XMLDBFreeTree(tree);
  ScriptFree(xmldoc.script);
  fclose(xmlfp); 
  fflush(stdout);  
  return 0;

}
