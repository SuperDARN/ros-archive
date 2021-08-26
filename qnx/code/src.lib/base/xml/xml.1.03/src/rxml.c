/* rxml.c
   ===== 
   Author R.J.Barnes
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
#include <ctype.h>
#include "rxml.h"

/*
 $Log: rxml.c,v $
 Revision 1.3  2004/06/16 21:41:25  barnes
 Added missing header.

 Revision 1.2  2004/04/26 20:24:54  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2003/04/01 19:12:00  barnes
 Initial revision

*/

struct XMLdata *XMLMake() {
  struct XMLdata *ptr;
  ptr=malloc(sizeof(struct XMLdata));
  if (ptr==NULL) return NULL;
  ptr->sze=0;
  ptr->max=0;
  ptr->stp=100;
  ptr->buf=NULL;
  ptr->interface.text.user=NULL;
  ptr->interface.text.data=NULL; 
  ptr->interface.start.user=NULL;
  ptr->interface.start.data=NULL;
  ptr->interface.end.user=NULL;
  ptr->interface.end.data=NULL;
  ptr->interface.comment.user=NULL;
  ptr->interface.comment.data=NULL;
  ptr->interface.process.user=NULL;
  ptr->interface.process.data=NULL;
  return ptr;
}

void XMLFree(struct XMLdata *ptr) {
  if (ptr==NULL) return;
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

int XMLSetText(struct XMLdata *ptr,
             int (*user)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.text.user=user;
  ptr->interface.text.data=data;
  return 0;
} 

int XMLSetProcess(struct XMLdata *ptr,
             int (*user)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.process.user=user;
  ptr->interface.process.data=data;
  return 0;
} 

int XMLSetComment(struct XMLdata *ptr,
             int (*user)(char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.comment.user=user;
  ptr->interface.comment.data=data;
  return 0;
} 

int XMLSetStart(struct XMLdata *ptr,
             int (*user)(char *,int,char **,char **,
                         char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.start.user=user;
  ptr->interface.start.data=data;
  return 0;
} 

int XMLSetEnd(struct XMLdata *ptr,
             int (*user)(char *,char *,int,void *),void *data) {
  if (ptr==NULL) return -1;
  ptr->interface.end.user=user;
  ptr->interface.end.data=data;
  return 0;
} 




int XMLDecode(struct XMLdata *ptr,char *buf,int sze) {
  int c=0;
  int s=0;
  int status=0;

  char *tbuf=NULL;
  int tsze=0;

  char *name;
  int atnum=0;
  char **atmp=NULL;
  char **atname=NULL;
  char **atval=NULL;

  int i,j;

  if (buf==NULL) { /* flush */
    status=(ptr->interface.text.user)(NULL,0,
            ptr->interface.text.data);
    return -1;
  }

  while(c<sze) {

    if (buf[c]=='<') { /* start of a tag */

      if (((c-s) !=0) && (ptr->interface.text.user !=NULL)) 
         status=(ptr->interface.text.user)(buf+s,c-s,
                 ptr->interface.text.data);
      
      if (status !=0) return status;

      if (ptr->buf !=NULL) return -1; /* bad XML syntax */
      ptr->buf=malloc(ptr->stp);
      if (ptr->buf==NULL) return -1; /* memory error */
      ptr->max=ptr->stp;
      ptr->sze=0;
    } else if (buf[c]=='>') { /* end of a tag */
      if (ptr->buf == NULL) return -1;

      /* at this point we can decode the tag */

      /* copy the location and size of the buffered tag data */

      tbuf=ptr->buf;
      tsze=ptr->sze;

      /* reset the structure */

      ptr->buf=NULL;
      ptr->max=0;
      ptr->sze=0;

     
      switch (tbuf[0]) {
      case '?':
        if (ptr->interface.process.user !=NULL) 
	  status=(ptr->interface.process.user)(tbuf,tsze,
                  ptr->interface.process.data);
        if (status !=0) return status;
        break;
      case '!':
        if (ptr->interface.comment.user !=NULL) 
	  status=(ptr->interface.comment.user)(tbuf,
                  tsze,ptr->interface.end.data);
        if (status !=0) return status;
        break;
      case '/':

        for (i=1;(i<tsze) && (isspace(tbuf[i]));i++);
        for (j=i;(j<tsze) && (!isspace(tbuf[j]));j++);
	name=malloc(j-i+1);
        if (name==NULL) return -1;
        memcpy(name,tbuf+i,j-i);
        name[j-i]=0;

        if (ptr->interface.end.user !=NULL) 
	  status=(ptr->interface.end.user)(name,
                  tbuf,tsze,ptr->interface.end.data);
         
        free(name);
        if (status !=0) return status;
        break;
      default:
        /* extract the tag name */

        for (i=0;(i<tsze) && (isspace(tbuf[i]));i++);
        for (j=i;(j<tsze) && (!isspace(tbuf[j]));j++);
	name=malloc(j-i+1);
        if (name==NULL) return -1;
        memcpy(name,tbuf+i,j-i);
        name[j-i]=0;
       
        /* further decode to get out any attribute pairs */
	
        atnum=0;
	
        while (j<tsze) {
          
        
	  for (i=j;(i<tsze) && (isspace(tbuf[i]));i++);
          for (j=i;(j<tsze) && (tbuf[j] !='=') && (!isspace(tbuf[j]));j++);
          
          if ((j-i) !=0) { 
	      
	    if (atname==NULL) atname=malloc(sizeof(char *));
            else {
              atmp=realloc(atname,sizeof(char *)*(atnum+1));           
              if (atmp==NULL) break;
              atname=atmp; 
	    }
            if (atname==NULL) break;

	    if (atval==NULL) atval=malloc(sizeof(char *));
            else {
              atmp=realloc(atval,sizeof(char *)*(atnum+1));           
	      if (atmp==NULL) break;
              atval=atmp;
	    }
            if (atval==NULL) break;    
 
            atname[atnum]=NULL;
            atval[atnum]=NULL;

	    
            atname[atnum]=malloc(j-i+1);
            if (atname[atnum]==NULL) break;

            memcpy(atname[atnum],tbuf+i,j-i);
            atname[atnum][j-i]=0;
           
          
            /* search for attribute value after the space */
     
            for (i=j;(i<tsze) && (tbuf[i] !='"');i++);
            j=i;
            if (j>=tsze) continue; /* no value */
            i++;
            for (j=i;(j<tsze) && 
                       (tbuf[j] !='"');j++);

            if ((j-i) !=0) {
              atval[atnum]=malloc(j-i+1);
              if (atval[atnum]==NULL) break;
              memcpy(atval[atnum],tbuf+i,j-i);
              atval[atnum][j-i]=0;
	    }
            j++;	    
            atnum++;
	    
	    
           
	  }
	  
	}
              
        if (j<tsze) status=-1;
 
        if ((status==0) && (ptr->interface.start.user !=NULL)) 
	  status=(ptr->interface.start.user)(name,atnum,atname,atval,
                  tbuf,tsze,ptr->interface.start.data);
      

        for (i=0;i<atnum;i++) {
	  if ((atname !=NULL) && (atname[i] !=NULL)) free(atname[i]);
          if ((atval !=NULL) &&  (atval[i] !=NULL)) free(atval[i]);
	}
        if (atname !=NULL) free(atname);
        if (atval !=NULL) free(atval);
        free(name);

        name=NULL;
        atname=NULL;
        atval=NULL;     	  

        if (status !=0) return status;
        break;
      }


      /* free the buffer */
      free(tbuf);
      tbuf=NULL;
      tsze=0;
      s=c+1;
    } else { /* text data or tag contents */

      if (ptr->buf !=NULL) { /* part of a tag */
        ptr->buf[ptr->sze]=buf[c];
        ptr->sze++;
        if (ptr->sze==ptr->max) {
          char *tmp;
          ptr->max+=ptr->stp;
          tmp=realloc(ptr->buf,ptr->max);
          if (tmp==NULL) return -1;
          ptr->buf=tmp;
	}
      } 
    }
    c++;
  }

  if ((ptr->buf==NULL) && ((c-s) !=0) && (ptr->interface.text.user !=0)) 
     status=(ptr->interface.text.user)(buf+s,c-s,ptr->interface.text.data);
  
  return status;
}





