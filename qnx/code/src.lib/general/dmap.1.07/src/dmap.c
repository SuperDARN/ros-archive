/* dmap.c
   ====== 
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "rtypes.h"
#include "rconvert.h"
#include "dmap.h"

/*
 $Log: dmap.c,v $
 Revision 1.7  2004/06/22 17:21:43  barnes
 Changed references to Long to Int.

 Revision 1.6  2004/06/16 16:33:17  barnes
 Added missing header.

 Revision 1.5  2004/05/19 14:02:10  barnes
 Fixed bug in encoding double scalars.

 Revision 1.4  2004/05/16 21:40:20  barnes
 Optimized the read and write routines to work in memory.
 Also added a two word header to each block giving it an unique
 encoding/version number and the block size.

 Revision 1.3  2004/05/04 17:14:28  barnes
 Renamed the type header to avoid confusion.

 Revision 1.2  2004/04/26 20:40:21  barnes
 Audit - Enforced warning set to "all".

 Revision 1.1  2004/03/27 15:16:58  barnes
 Initial revision

*/

struct DataMap *DataMapMake() {
  struct DataMap *ptr;
  ptr=malloc(sizeof(struct DataMap));
  if (ptr==NULL) return ptr;
  ptr->sze=0;
  ptr->buf=NULL;
  ptr->scl=NULL;
  ptr->snum=0;
  ptr->arr=NULL;
  ptr->anum=0;
  return ptr;
}

struct DataMapScalar* DataMapMakeScalar(char *name,int type,
                                                void *data) {
  struct DataMapScalar *ptr;

  if (name==NULL) return NULL;
  ptr=malloc(sizeof(struct DataMap));
  if (ptr==NULL) return ptr;
  ptr->name=malloc(strlen(name)+1);
  if (ptr->name==NULL) {
    free(ptr);
    return NULL;
  }
  strcpy(ptr->name,name);
  ptr->type=type;
  ptr->data.vptr=data;
  return ptr;
}

void DataMapFreeScalar(struct DataMapScalar *ptr) {
  if (ptr==NULL) return; 
  if (ptr->name !=NULL) free(ptr->name);
  free(ptr);
  return;
}

int DataMapAddScalar(struct DataMap *ptr,
                         char *name,int type,void *data) {
  struct DataMapScalar *s;
  if (ptr==NULL) return -1;
  s=DataMapMakeScalar(name,type,data);
  if (s==NULL) return -1;

  if (ptr->scl==NULL) ptr->scl=malloc(sizeof(struct DataMapScalar *));
  else {
    struct DataMapScalar **tmp;
    tmp=realloc(ptr->scl,(ptr->snum+1)*sizeof(struct DataMapScalar *));
    if (tmp==NULL) {
      DataMapFreeScalar(s);
      return -1;
    }
    ptr->scl=tmp;
  }
  ptr->scl[ptr->snum]=s;
  ptr->snum++;
  return 0;
}

struct DataMapArray* DataMapMakeArray(char *name,int type,int dim,
                                              int32 *rng,void *data) {
  
  struct DataMapArray *ptr;

  if (name==NULL) return NULL;
  if (dim==0) return NULL;

  ptr=malloc(sizeof(struct DataMapArray));
  if (ptr==NULL) return ptr;
  ptr->name=malloc(strlen(name)+1);
  if (ptr->name==NULL) {
    free(ptr);
    return NULL;
  }
  strcpy(ptr->name,name);
  ptr->rng=rng;
  ptr->type=type;
  ptr->dim=dim;
  ptr->data.vptr=data;
  return ptr;
}

void DataMapFreeArray(struct DataMapArray *ptr) {
  if (ptr==NULL) return; 
  if (ptr->name !=NULL) free(ptr->name);
  free(ptr);
  return;
}


int DataMapAddArray(struct DataMap *ptr,
                        char *name,int type,int dim,
                        int32 *rng,void *data) {
  struct DataMapArray *a;
  if (ptr==NULL) return -1;
  a=DataMapMakeArray(name,type,dim,rng,data);
  if (a==NULL) return -1;

  if (ptr->arr==NULL) ptr->arr=malloc(sizeof(struct DataMapArray *));
  else {
    struct DataMapArray **tmp;
    tmp=realloc(ptr->arr,(ptr->anum+1)*sizeof(struct DataMapArray *));
    if (tmp==NULL) {
      DataMapFreeArray(a);
      return -1;
    }
    ptr->arr=tmp;
  }
  ptr->arr[ptr->anum]=a;
  ptr->anum++;
  return 0;
}


int DataMapSize(struct DataMap *ptr) {
  int c,x,m,n;
  char **tmp;
  struct DataMapScalar *s=NULL;
  struct DataMapArray *a=NULL;
  int sze=0;

  sze+=16;
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    n=0;
    while (s->name[n] !=0) n++;
    sze+=n+1+1;
    switch (s->type) {
    case DATACHAR:
      sze++;
      break;
    case DATASHORT:
      sze+=2;
      break;
    case DATAINT:
      sze+=4;
      break;
    case DATAFLOAT:
      sze+=4;
      break;
    case DATADOUBLE:
      sze+=8;
      break;
    default:
      tmp=(char **) s->data.vptr;
      if (*tmp !=NULL) {
        n=0;
        while((*tmp)[n] !=0) n++;
        sze+=n+1;  
      }
      break;
    }
  }
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    n=0;

    while (a->name[n] !=0) n++;
    sze+=n+1+1+4+4*a->dim;
    n=1;
    for (x=0;x<a->dim;x++) n=a->rng[x]*n;
    switch (a->type) {
    case DATACHAR:
      sze+=n;
      break;
    case DATASHORT:
      sze+=2*n;
      break;
    case DATAINT:
      sze+=4*n;
      break;
    case DATAFLOAT:
      sze+=4*n;
      break;
    case DATADOUBLE:
      sze+=8*n;
      break;
    default:
      tmp=(char **) s->data.vptr;
      for (x=0;x<n;x++) {
        if (tmp[x] !=NULL) {
          m=0;
          while( (tmp[x])[m] !=0) m++;
          sze+=m+1;       
	}
      }
      break;
   }    
  }
  return sze;
}




unsigned char *DataMapEncodeBuffer(struct DataMap *ptr,int *size) {
  int c,x,m,n;
  char **tmp;
  struct DataMapScalar *s=NULL;
  struct DataMapArray *a=NULL;
  unsigned char *buf=NULL;
  int off=0;
  int sze=0;


  sze=DataMapSize(ptr);
  if (size !=NULL) *size=sze;
  buf=malloc(sze);
  if (buf==NULL) return NULL;

  ConvertFromInt(DATACODE,buf+off);
  off+=4;
  ConvertFromInt(sze,buf+off);
  off+=4;
  ConvertFromInt(ptr->snum,buf+off);
  off+=4;
  ConvertFromInt(ptr->anum,buf+off);
  off+=4;
  
  for (c=0;c<ptr->snum;c++) {
    s=ptr->scl[c];
    n=0;
    while (s->name[n] !=0) n++;
    memcpy(buf+off,s->name,n+1);
    off+=n+1;
    buf[off]=s->type;
    off++;
    switch (s->type) {
    case DATACHAR:
      buf[off]=s->data.cptr[0];
      off++;
      break;
    case DATASHORT:
      ConvertFromShort(*(s->data.sptr),buf+off);
      off+=2;
      break;
    case DATAINT:
      ConvertFromInt(*(s->data.lptr),buf+off);
      off+=4;
      break;
    case DATAFLOAT:
      ConvertFromFloat(*(s->data.fptr),buf+off);
      off+=4;
      break;
    case DATADOUBLE:
      ConvertFromDouble(*(s->data.dptr),buf+off);
      off+=8;
      break;
    default:
      tmp=(char **) s->data.vptr;
      if (*tmp !=NULL) {
        n=0;
        while((*tmp)[n] !=0) n++;
        memcpy(buf+off,*tmp,n+1);
        off+=n+1;
      }
      break;
    }
  }
 
  for (c=0;c<ptr->anum;c++) {
    a=ptr->arr[c];
    n=0;
    while (a->name[n] !=0) n++;
    memcpy(buf+off,a->name,n+1);
    off+=n+1;
    buf[off]=a->type;
    off++;
    ConvertFromInt(a->dim,buf+off);
    off+=4;
    for (x=0;x<a->dim;x++)  {
      ConvertFromInt(a->rng[x],buf+off);
      off+=4;
    }
    n=1;
    for (x=0;x<a->dim;x++) n=a->rng[x]*n;
    switch (a->type) {
    case DATACHAR:
      memcpy(buf+off,a->data.cptr,n);
      off+=n;
      break;
    case DATASHORT:
      for (x=0;x<n;x++) {
        ConvertFromShort(a->data.sptr[x],buf+off);
        off+=2;
      }
      break;
    case DATAINT:
      for (x=0;x<n;x++) {
        ConvertFromInt(a->data.lptr[x],buf+off);
        off+=4;
      }
      break;
    case DATAFLOAT:
      for (x=0;x<n;x++) {
        ConvertFromFloat(a->data.fptr[x],buf+off);
        off+=4;
      }
      break;
    case DATADOUBLE:
      for (x=0;x<n;x++) {
        ConvertFromDouble(a->data.dptr[x],buf+off);
        off+=8;
      }
      break;
    default:
      tmp=(char **) a->data.vptr;
      for (x=0;x<n;x++) {
        if (tmp[x] !=NULL) {
          m=0;
          while( (tmp[x])[m] !=0) m++;
          memcpy(buf+off,tmp[x],m+1);
          off+=m+1;
	}
      }
      break;
   }    
  }
  return buf;
}


int DataMapWrite(int fid,struct DataMap *ptr) {
  unsigned char *buf=NULL;
  int sze=0,s=0;
  buf=DataMapEncodeBuffer(ptr,&sze);
  if (buf==NULL) return -1;
  s=write(fid,buf,sze);
  free(buf);
  return s;
}

int DataMapFwrite(FILE *fp,struct DataMap *ptr) {
  return DataMapWrite(fileno(fp),ptr);
}

void DataMapFree(struct DataMap *ptr) {
  int s,a;
  if (ptr==NULL) return;
  if (ptr->scl !=NULL) {  
    for (s=0;s<ptr->snum;s++) {
      DataMapFreeScalar(ptr->scl[s]);
    }
    free(ptr->scl);
  }
  if (ptr->arr !=NULL) {
    for (a=0;a<ptr->anum;a++) {
      DataMapFreeArray(ptr->arr[a]);
    }
    free(ptr->arr);
  }
  if (ptr->buf !=NULL) free(ptr->buf);
  free(ptr);
}

char *DataMapReadName(int fid) {
  char *ptr,*tmp;
  char t;
  int st=0;
  int blk=256;
  int stp=256;
  int sze=0;

  ptr=malloc(sizeof(char)*blk);
  while (((st=read(fid,&t,1))==1) && (t !=0)) {
    ptr[sze]=t;
    sze++;
    if (sze>=blk) {
      blk+=stp;
      tmp=realloc(ptr,blk);
      if (tmp==NULL) free(ptr);
      ptr=tmp;
    }
    if (ptr==NULL) return NULL;
  }
  if (st != 1) {
    free(ptr);
    return NULL;
  } 
  ptr[sze]=0;
  
  sze++;
  tmp=realloc(ptr,sze);
  if (tmp==NULL) free(ptr);
  ptr=tmp;
  return ptr;
}

char *DataMapReadString(int fid) {
  char *ptr,*tmp;
  char t;
  int st=0;
  int blk=256;
  int stp=256;
  int sze=0;

  ptr=malloc(sizeof(char)*blk);
  while (((st=read(fid,&t,1))==1) && (t !=0)) {
    ptr[sze]=t;
    sze++;
    if (sze>=blk) {
      blk+=stp;
      tmp=realloc(ptr,blk);
      if (tmp==NULL) free(ptr);
      ptr=tmp;
    }
    if (ptr==NULL) return NULL;
  }
  if (st != 1) {
    free(ptr);
    return NULL;
  } 
  ptr[sze]=0;
  
  sze++;
  tmp=realloc(ptr,sze);
  if (tmp==NULL) free(ptr);
  ptr=tmp;
  return ptr;
}

struct DataMap *DataMapDecodeBuffer(unsigned char *buf,int size) {
  int c,x,n,i;
  int32 sn,an;
  int32 code,sze;
  char *name;
  char *tmp;
  int32 *rng=NULL;
  char **sptr;
  char type;
  int *offset=NULL;
  int off=0;  

  struct DataMap *ptr;
  struct DataMapScalar *s;
  struct DataMapArray *a;
  
  ptr=DataMapMake();
  if (ptr==NULL) return NULL;
  ConvertToInt(buf+off,&code);
  off+=4;
  ConvertToInt(buf+off,&sze);
  off+=4;
  ConvertToInt(buf+off,&sn);
  off+=4;
  ConvertToInt(buf+off,&an);
  off+=4;
  if (sn>0) {
    ptr->snum=sn;
    ptr->scl=malloc(sizeof(struct DataMapScalar *)*sn); 
    if (ptr->scl==NULL) {
      DataMapFree(ptr);
      return NULL;
    }
    for (c=0;c<sn;c++) ptr->scl[c]=NULL;
  }

  if (an>0) {
    ptr->anum=an;
    ptr->arr=malloc(sizeof(struct DataMapArray *)*an);
    if (ptr->arr==NULL) {
      DataMapFree(ptr);
      return NULL;
    }
    for (c=0;c<an;c++) ptr->arr[c]=NULL;
  }

  if ((sn+an)>0) {
    offset=malloc(sizeof(int)*(sn+2*an));
    if (offset==NULL) {
      DataMapFree(ptr);
      return NULL;
    }
  }
  for (c=0;c<sn;c++) {
    n=0;
    while ((buf[off+n] !=0) && (off+n<size)) n++;
    if (off+n>=size) break;
    name=malloc(n+1);
    if (name==NULL) break;
    memcpy(name,buf+off,n+1);
    off+=n+1;
    type=buf[off];
    off++;
    s=malloc(sizeof(struct DataMapScalar));
    if (s==NULL) {
      free(name);
      break;
    }

    s->name=name;
    s->type=type;
    ptr->scl[c]=s;   

    switch (s->type) {
    case DATACHAR:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(char));
      if (tmp==NULL) break;
      ptr->buf=tmp;
      ptr->buf[ptr->sze]=buf[off];
      off++;
      offset[c]=ptr->sze;
      ptr->sze++;
      break;
    case DATASHORT:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(int16));
      if (tmp==NULL) break;
      ptr->buf=tmp;
      ConvertToShort(buf+off,(int16 *) (ptr->buf+ptr->sze));
      off+=2;
      offset[c]=ptr->sze;     
      ptr->sze+=sizeof(int16);
      break;
    case DATAINT:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(int32));
      if (tmp==NULL) break;
      ptr->buf=tmp;
      ConvertToInt(buf+off,(int32 *) (ptr->buf+ptr->sze));
      off+=4;
      offset[c]=ptr->sze;     
      ptr->sze+=sizeof(int32);
      break;
    case DATAFLOAT:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(float));
      if (tmp==NULL) break;
      ptr->buf=tmp;
      ConvertToFloat(buf+off,(float *) (ptr->buf+ptr->sze));
      off+=4;
      offset[c]=ptr->sze;     
      ptr->sze+=sizeof(float);
      break;
    case DATADOUBLE:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(double));
      if (tmp==NULL) break;
      ptr->buf=tmp;
      ConvertToDouble(buf+off,(double *) (ptr->buf+ptr->sze));
      off+=8;
      offset[c]=ptr->sze;     
      ptr->sze+=sizeof(double);
      break;
    default:
      n=0;
      while ((buf[off+n] !=0) && (off+n<size)) n++;
      if (off+n>=size) break;
      tmp=realloc(ptr->buf,ptr->sze+sizeof(char *)+n+1);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      memcpy(ptr->buf+ptr->sze+sizeof(char *),buf+off,n+1);
      off+=n+1;
      offset[c]=ptr->sze;
      ptr->sze+=sizeof(char *)+n+1;
      break;
    }
  }

  if (c !=sn) {
    free(offset);
    DataMapFree(ptr);
    return NULL;
  }
  for (c=0;c<an;c++) {
    n=0;
    while ((buf[off+n] !=0) && (off+n<size)) n++;
    if (off+n>=size) break;
    name=malloc(n+1);
    if (name==NULL) break;
    memcpy(name,buf+off,n+1);
    off+=n+1;
    type=buf[off];
    off++;
    a=malloc(sizeof(struct DataMapArray));
    if (a==NULL) {
      free(name);
      break;
    }
    a->name=name;
    a->type=type;
    ptr->arr[c]=a;   
    ConvertToInt(buf+off,(int32 *) &(a->dim));
    off+=4;
    tmp=realloc(ptr->buf,ptr->sze+a->dim*sizeof(int32));
    if (tmp==NULL) break;
    ptr->buf=tmp;
    offset[sn+c]=ptr->sze; 
    rng=(int32 *) (ptr->buf+ptr->sze);
    ptr->sze+=sizeof(int32)*a->dim;
    for (x=0;x<a->dim;x++)  {
       ConvertToInt(buf+off,&rng[x]);
       off+=4;
    }
    if (x!=a->dim) break;
    n=1;
    for (x=0;x<a->dim;x++) n=rng[x]*n;
    switch (a->type) {
    case DATACHAR:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(char)*n);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      memcpy(ptr->buf+ptr->sze,buf+off,sizeof(char)*n);
      off+=sizeof(char)*n;
      offset[sn+an+c]=ptr->sze;
      ptr->sze+=n;
      break;
    case DATASHORT:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(int16)*n);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      for (x=0;x<n;x++) {
        ConvertToShort(buf+off,(int16 *) (ptr->buf+ptr->sze+sizeof(int16)*x));
        off+=2;
      }
      offset[sn+an+c]=ptr->sze;     
      ptr->sze+=sizeof(int16)*n;
      break;
    case DATAINT:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(int32)*n);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      for (x=0;x<n;x++) {
        ConvertToInt(buf+off,(int32 *) (ptr->buf+ptr->sze+sizeof(int32)*x));
        off+=4;
      }
      offset[sn+an+c]=ptr->sze;     
      ptr->sze+=sizeof(int32)*n;
      break;
    case DATAFLOAT:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(float)*n);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      for (x=0;x<n;x++) {
        ConvertToFloat(buf+off,(float *) (ptr->buf+ptr->sze+sizeof(float)*x));
        off+=4;
      }
      offset[sn+an+c]=ptr->sze;     
      ptr->sze+=sizeof(float)*n;
      break;
     case DATADOUBLE:
      tmp=realloc(ptr->buf,ptr->sze+sizeof(double)*n);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      for (x=0;x<n;x++) {
        ConvertToDouble(buf+off,(double *) 
                       (ptr->buf+ptr->sze+sizeof(double)*x));
        off+=8;
      }
      offset[sn+an+c]=ptr->sze;     
      ptr->sze+=sizeof(double)*n;
      break;
    default:
      offset[sn+an+c]=ptr->sze;
      tmp=realloc(ptr->buf,ptr->sze+sizeof(char *)*n);
      if (tmp==NULL) break;
      ptr->buf=tmp;
      ptr->sze+=sizeof(char *)*n;
      for (x=0;x<n;x++) {
         i=0;
         while ((buf[off+i] !=0) && (off+i<size)) i++;
         if (off+i>=size) break;
         tmp=realloc(ptr->buf,ptr->sze+i+1);
         if (tmp==NULL) break;
         ptr->buf=tmp;
         memcpy(ptr->buf+ptr->sze,buf+off,i+1); 
         sptr=(char **) (ptr->buf+offset[sn+an+c]);
         sptr[x]=(char *) ptr->sze; 
         ptr->sze+=i+1;
      }
      if (x !=n) break;
    }
  }

  if (c !=an) {
    free(offset);
    DataMapFree(ptr);
    return NULL;
  }
  

  for (c=0;c<ptr->snum;c++) {
    if (ptr->scl[c]==NULL) continue;
    s=ptr->scl[c];
    s->data.cptr=ptr->buf+offset[c];
    if (s->type==DATASTRING) 
      *((char **) s->data.vptr)=ptr->buf+offset[c]+sizeof(char *);
  }
  
  for (c=0;c<ptr->anum;c++) {
    if (ptr->arr[c]==NULL) continue;
    a=ptr->arr[c];
    a->rng=(int32 *) (ptr->buf+offset[sn+c]);
    a->data.cptr=ptr->buf+offset[sn+an+c];
    if (a->type==DATASTRING) {
      sptr=(char **) a->data.cptr;
      n=1;
      for (x=0;x<a->dim;x++) n=rng[x]*n;
      for (x=0;x<n;x++) sptr[x]=ptr->buf+(int) sptr[x];
    }

  }
  free(offset);
  return ptr;
}


struct DataMap *DataMapRead(int fid) {
  unsigned char *buf=NULL;
  struct DataMap *ptr;
  int32 code,sze,*iptr;
  int size=0,st=0;

  st=ConvertReadInt(fid,&code);
  st=ConvertReadInt(fid,&sze);

  size=sze;
  buf=malloc(size);
  if (buf==NULL) return NULL;
  iptr=(int32 *) buf;
  iptr[0]=code;
  iptr[1]=sze;
  
  st=read(fid,buf+2*sizeof(int32),size-2*sizeof(int32));
  if (st !=(size-2*sizeof(int32))) {
    free(buf);
    return NULL;
  }
  ptr=DataMapDecodeBuffer(buf,size);
  free(buf);
  return ptr;
}





struct DataMap *DataMapFread(FILE *fp) {
  return DataMapRead(fileno(fp));
}
