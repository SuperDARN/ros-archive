/* read_file.c
   ===========
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


/*
 $Log: read_file.c,v $
 Revision 1.11  2001/09/21 17:29:58  barnes
 Added support for digital format files.

 Revision 1.10  2001/06/27 20:38:16  barnes
 Added license tag

 Revision 1.9  2001/01/29 18:11:48  barnes
 Added Author Name

 Revision 1.8  2000/08/03 15:52:58  barnes
 Fixed further bugs in the seek routine.

 Revision 1.7  2000/08/03 15:26:34  barnes
 Fixed bug in searching.

 Revision 1.6  2000/08/02 12:18:56  barnes
 Added indexing and fast seeking.

 Revision 1.5  2000/08/01 22:58:51  barnes
 Added code to return just the time values.

 Revision 1.4  2000/03/14 17:23:53  barnes
 Added tabs to the list of spacing characters.

 Revision 1.3  1999/10/08 18:55:38  barnes
 Fixed bug in passing empty records to the decoders.

 Revision 1.2  1999/10/07 13:40:45  barnes
 Modification to allow empty records to be detected.

 Revision 1.1  1999/10/05 18:28:03  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include "cnv_data.h"
#include "cnv_time.h"
#include "file_lib.h"


int read_file_line(FILE *fp,int buflen,char *buffer) {
  char *s=NULL;
  do {
    s=fgets(buffer,buflen,fp);
    if (s==NULL) break;
  } while ((buffer[0]=='#') || (buffer[0]=='%'));
  if (s==NULL) return -1;
  return 0;
}

int read_raw_file_line(FILE *fp,int buflen,char *buffer) {
  int s;
  int j=0;
  do {
    s=fgetc(fp);
    if (s==EOF) break;
    buffer[j]=s;
    j++;
  } while (s !=0);
  if (s==EOF) return -1;
  return 0;
}



int read_file_raw(FILE *fp,int num,decoder *decode,
              void **ptr) {

  char line[LMAX];
  char name[LMAX];
  char type[LMAX];
  char unit[LMAX];
  char *tcmp[]={"text","char","float","double","short","long","int",0};
  int txt;
  int sptr;
 
  double st_time,ed_time;

  int i,j,k,l,ka,kb,s;
  int32 nblks;
  int32 nprm=0,npnt=0;

  struct file_data *data=NULL;
  sptr=ftell(fp)-1;

  fread_double(fp,&st_time);
  fread_double(fp,&ed_time);
  
 

  if (num==0) { /* no decoders - but we may still want the time */
    if (ptr !=NULL) {
      ((double *) ptr[0])[0]=st_time;
      ((double *) ptr[0])[1]=ed_time;
    }
  }

  fread_long(fp,&nblks);
 
  for (i=0;i<nblks;i++) {
    fread_long(fp,&npnt);
    fread_long(fp,&nprm);

    if (data !=NULL) data=realloc(data,sizeof(struct file_data)*nprm);
    else data=malloc(sizeof(struct file_data)*nprm);
    memset(data,0,sizeof(struct file_data)*nprm);
   
    /* now get the title string */
    name[0]=0;    
    for (j=0;j<nprm;j++) {
       if (read_raw_file_line(fp,LMAX,line) !=0) break;
       if (j>0) strcat(name," ");
       strcat(name,line);
    }
    unit[0]=0;
    for (j=0;j<nprm;j++) {
       if (read_raw_file_line(fp,LMAX,line) !=0) break;
       if (j>0) strcat(unit," ");
       strcat(unit,line);
    }
    type[0]=0;
    for (j=0;j<nprm;j++) {
       if (read_raw_file_line(fp,LMAX,line) !=0) break;
       if (j>0) strcat(type," ");
       strcat(type,line);
       for (k=0;(tcmp[k] !=0) && (strcmp(line,tcmp[k]) !=0);k++);
       if (tcmp[k]==0) k=0;
       data[j].type=k;
       if (k==0) txt=1;

    }
    
   
    if (npnt==0) { /* tell loaders about empty records */
       for (k=0;k<num;k++) {
         s=(decode[k])(name,unit,type,st_time,ed_time,
                          npnt,nprm,0,data,ptr[k]);
         if (s==-1) break;
       }
       if (k !=num) break;
    }
  

    for (l=0;l<npnt;l++) {
      for (j=0;j<nprm;j++) {
        if (data[j].type==0) { /* get string */
          if (read_raw_file_line(fp,LMAX,line) !=0) break;
          if (data[j].data.tval !=NULL) 
          data[j].data.tval=realloc(data[j].data.tval,strlen(line)+1);
          else data[j].data.tval=malloc(strlen(line)+1);
          strcpy(data[j].data.tval,line);
        } else if (data[j].type==1) {
           data[j].data.cval=fgetc(fp);
	}
        else if (data[j].type==2) fread_float(fp,&data[j].data.fval);
        else if (data[j].type==3) fread_double(fp,&data[j].data.dval);
        else if (data[j].type==4) fread_short(fp,&data[j].data.sval);
        else if (data[j].type==5) fread_long(fp,&data[j].data.lval);
        else if (data[j].type==6) fread_long(fp,&data[j].data.ival);
      }

     if (j !=nprm) break;
      
     for (k=0;k<num;k++) {
       s=(decode[k])(name,unit,type,st_time,ed_time,
                     npnt,nprm,l,data,ptr[k]);
       if (s==-1) break;
     }
     if (k !=num) break;
    }

    /* free text strings */
    if (num !=0) {
      if (txt==1) {
        for (k=0;k<nprm;k++) if ((data[k].type==0) &&
        (data[k].data.tval !=NULL)) free(data[k].data.tval);
      }
      if (l != npnt) break; 
    }     
  }
      
  if (num !=0) free(data);
  if (i<nblks) return -1;
  return ftell(fp)-sptr;


}

int read_file(FILE *fp,int num,decoder *decode,
              void **ptr) {

  double st_time,ed_time;
  int syr,smo,sdy,shr,smt,ssc;
  int eyr,emo,edy,ehr,emt,esc;
  int i,j,k,l,ka,kb,s;
  int nblks;

  int nprm=0,npnt=0;
  
  int sptr;
  int txt;
  
  char line[LMAX];
  char name[LMAX];
  char type[LMAX];
  char unit[LMAX];


  char *tcmp[]={"text","char","float","double","short","long","int",0};
  char *tcnv[]={"%s","%c","%g","%lg","%d","%d","%d",0};

  char form[LMAX];
  
  char tmp[1024]; /* buffer to store parameters */

  struct file_data *data=NULL;
  
  sptr=ftell(fp);

  s=fgetc(fp);
  if (s==0) return read_file_raw(fp,num,decode,ptr);
  line[0]=s;
  
  if (read_file_line(fp,LMAX-1,line+1) !=0) return -1;
 
  if (sscanf(line,"%d %d %d %d %d %d %d %d %d %d %d %d",
                  &syr,&smo,&sdy,&shr,&smt,&ssc,
                  &eyr,&emo,&edy,&ehr,&emt,&esc) !=12) return -1;


  st_time=time_epoch(syr,smo,sdy,shr,smt,ssc);
  ed_time=time_epoch(eyr,emo,edy,ehr,emt,esc);

  if (num==0) { /* no decoders - but we may still want the time */
    if (ptr !=NULL) {
      ((double *) ptr[0])[0]=st_time;
      ((double *) ptr[0])[1]=ed_time;
    }
  }

  if (read_file_line(fp,LMAX,line) !=0) return -1;

  if (sscanf(line,"%d",&nblks) !=1) return -1;
 
  for (i=0;i<nblks;i++) {

    if (read_file_line(fp,LMAX,line) !=0) break;
    if (sscanf(line,"%d %d",&npnt,&nprm) !=2) break;
    if (num !=0) {    
      if (data !=NULL) data=realloc(data,sizeof(struct file_data)*nprm);
      else data=malloc(sizeof(struct file_data)*nprm);
      memset(data,0,sizeof(struct file_data)*nprm);
    }

    if (read_file_line(fp,LMAX,line) !=0) break;
    if (num !=0) {
      ka=0;
      kb=0;
      name[0]=0;
      for (j=0;j<nprm;j++) {

        /* skip spaces */
        for (k=ka;((line[k]==' ') || (line[k]=='\t') || (line[k]=='\n')) && 
                   (line[k] !=0);k++);
        ka=k;
        for (k=ka;(line[k]!=' ')  && (line[k]!='\t') &&(line[k]!='\n') && 
                 (line[k] !=0);k++);
        kb=k;
        if (j>0) strcat(name," ");
        strncat(name,line+ka,kb-ka);
        ka=kb;
      }
    }
    if (read_file_line(fp,LMAX,line) !=0) break;
    if (num !=0) {
      ka=0;
      kb=0;
      unit[0]=0;
      for (j=0;j<nprm;j++) {

        /* skip spaces */
        for (k=ka;((line[k]==' ')  || (line[k]=='\t') || (line[k]=='\n')) &&
                   (line[k] !=0);k++);
        ka=k;
        for (k=ka;(line[k]!=' ')  &&  (line[k]!='\t') && (line[k]!='\n') && 
                  (line[k] !=0);k++);
        kb=k;
        if (j>0) strcat(unit," ");
        strncat(unit,line+ka,kb-ka);
        ka=kb;
      }
    }
    if (read_file_line(fp,LMAX,line) !=0) break;
    if (num !=0) {
      ka=0;
      kb=0;
      type[0]=0;
      for (j=0;j<nprm;j++) {

        /* skip spaces */
        for (k=ka;((line[k]==' ') || (line[k]=='\t') || (line[k]=='\n')) && 
            (line[k] !=0);k++);
        ka=k;
        for (k=ka;(line[k]!=' ')  && (line[k]!='\t')  && (line[k]!='\n') && 
            (line[k] !=0);k++);
        kb=k;
        if (j>0) strcat(type," ");
        strncat(type,line+ka,kb-ka);
        ka=kb;
      }

      txt=0;
      ka=0;
      kb=0;
      for (j=0;j<nprm;j++) {
    
        for (k=ka;(type[k]!=' ')  && (type[k]!='\t') && (type[k]!='\n') && 
            (type[k] !=0);k++);
        kb=k;
        for (k=0;(tcmp[k] !=0) && (strncmp(type+ka,tcmp[k],kb-ka) !=0);k++);
        if (tcmp[k]==0) k=0;
      
        data[j].type=k;
        if (k==0) txt=1;
        ka=kb+1;
      }

      if (npnt==0) { /* tell loaders about empty records */
         for (k=0;k<num;k++) {
            s=(decode[k])(name,unit,type,st_time,ed_time,
                          npnt,nprm,0,data,ptr[k]);
            if (s==-1) break;
         }
         if (k !=num) break;
      }
    }
    for (l=0;l<npnt;l++) {
      
      if (read_file_line(fp,LMAX,line) !=0) break;
      if (num !=0) {
        ka=0;
    
        for (j=0;j<nprm;j++) {

          for (k=ka;((line[k]==' ') || (line[k]=='\t') || (line[k]=='\n')) && 
              (line[k] !=0);k++);
          ka=k;
          for (k=ka;(line[k]!=' ')  && (line[k]!='\t') && (line[k]!='\n') && 
              (line[k] !=0);k++);
        
     
          if (sscanf(line+ka,tcnv[data[j].type],tmp) !=1) break;
        
          if (data[j].type==1) data[j].data.cval=*tmp;
          else if (data[j].type==2) data[j].data.fval=*((float *) tmp);
          else if (data[j].type==3) data[j].data.dval=*((double *) tmp);
          else if (data[j].type==4) data[j].data.ival=*((int *) tmp);
          else if (data[j].type==5) data[j].data.ival=*((int *) tmp);
          else if (data[j].type==6) data[j].data.ival=*((int *) tmp);
          else {
            if (data[j].data.tval !=NULL) 
              data[j].data.tval=realloc(data[j].data.tval,strlen(tmp)+1);
            else data[j].data.tval=malloc(strlen(tmp)+1);
            strcpy(data[j].data.tval,tmp);
          }
          ka=k;        
        }
        if (j !=nprm) break;
      
        for (k=0;k<num;k++) {
            s=(decode[k])(name,unit,type,st_time,ed_time,
                          npnt,nprm,l,data,ptr[k]);
          if (s==-1) break;
        }
        if (k !=num) break;
      }
    }
    
    /* free text strings */
    if (num !=0) {
      if (txt==1) {
        for (k=0;k<nprm;k++) if ((data[k].type==0) &&
        (data[k].data.tval !=NULL)) free(data[k].data.tval);
      }
      if (l != npnt) break; 
    }     
  }
      
  if (num !=0) free(data);
  if (i<nblks) return -1;
  return ftell(fp)-sptr;
}
  

struct file_index *load_index(FILE *fp) {

  int syr,smo,sdy,shr,smt,ssc,eyr,emo,edy,ehr,emt,esc;
  double tme;
  int off; 
  char line[LMAX];

  struct file_index *ptr;
  ptr=malloc(sizeof(struct file_index));

  ptr->time.start=malloc(sizeof(double)*FILE_STEP);
  ptr->time.end=malloc(sizeof(double)*FILE_STEP);
  ptr->offset=malloc(sizeof(int)*FILE_STEP);
  ptr->num=0;


  while (read_file_line(fp,LMAX,line)==0) {
    if (sscanf(line,"%d %d %d %d %d %d %d %d %d %d %d %d %d",
        &syr,&smo,&sdy,&shr,&smt,&ssc,
        &eyr,&emo,&edy,&ehr,&emt,&esc,&off) !=13) 
      break;
    tme=time_epoch(syr,smo,sdy,shr,smt,ssc);
    if (ptr->num==0) ptr->st_time=tme;
    ptr->time.start[ptr->num]=tme;

    tme=time_epoch(eyr,emo,edy,ehr,emt,esc);
    ptr->ed_time=tme;
    ptr->time.end[ptr->num]=tme;

    ptr->offset[ptr->num]=off;

    ptr->num++;
    if ((ptr->num % FILE_STEP)==0) {
      int inc;
      inc=FILE_STEP*(ptr->num/FILE_STEP+1);
      ptr->time.start=realloc(ptr->time.start,sizeof(double)*inc);
      ptr->time.end=realloc(ptr->time.end,sizeof(double)*inc);

      ptr->offset=realloc(ptr->offset,sizeof(int)*inc);
    }
  }
  ptr->time.start=realloc(ptr->time.start,sizeof(double)*ptr->num);
  ptr->time.end=realloc(ptr->time.end,sizeof(double)*ptr->num);

  ptr->offset=realloc(ptr->offset,sizeof(int)*ptr->num);

  return ptr;
}




       

int seek_file(FILE *fp,
	      int yr,int mo,int dy,int hr,int mt,int sc,struct file_index *inx,
              double *aval) {

  
  int status=0;
  int pstatus=0;
  int tval;

  tval=time_epoch(yr,mo,dy,hr,mt,sc);
  
  if (inx !=NULL) {
    int rec=0,prec=-1;
    int st_rec;
    int ed_rec;

    /* search using index records */

    double st_time,ed_time;
    st_time=inx->st_time;
    ed_time=inx->ed_time;

    st_rec=0;
    ed_rec=inx->num;

    if (tval<st_time) { /* before start of file */
      if (aval !=NULL) *aval=st_time;
      fseek(fp,inx->offset[st_rec],SEEK_SET);
      return 0;
    } else if (tval>ed_time) { /* after end of file */
      if (aval !=NULL) *aval=st_time;
      fseek(fp,inx->offset[ed_rec-1],SEEK_SET);
      return 0;
    }

    do {
     
      prec=rec;
      rec=st_rec+(int) ((tval-st_time)*(ed_rec-st_rec))/(ed_time-st_time);
      if (inx->time.start[rec]==tval) break;
      if (inx->time.start[rec]<tval) {
         st_rec=rec;
         st_time=inx->time.start[rec];
      } else {
         ed_rec=rec;
         ed_time=inx->time.end[rec];
      }
    } while (prec != rec);
    if (aval !=NULL) *aval=inx->time.start[rec]; 
    fseek(fp,inx->offset[rec],SEEK_SET);
    return 0;
  } else {
    int ptr=0,pptr=0;
    double tme[2];
    void *tptr[2];
    tptr[0]=tme;
 
    /* search manually */

    status=read_file(fp,0,NULL,tptr);
    if (status==0) {
      if (tme[0]>tval) fseek(fp,0,SEEK_SET); /* need to rewind to start */
    } else {
      fseek(fp,0,SEEK_SET); /* set to the start of the file */
      status=0;
    }
    ptr=ftell(fp);
    if (aval !=NULL) *aval=tme[0];
    do {
      pptr=ptr;
      ptr=ftell(fp);
      status=read_file(fp,0,NULL,tptr);
      if (aval !=NULL) *aval=tme[0];
    } while ((tval>tme[0]) && (status !=-1));
    
    if (status==-1) {
      fseek(fp,pptr,SEEK_SET);
      return -1;
    }
    fseek(fp,ptr,SEEK_SET);
   
  }
  return 0;
}





