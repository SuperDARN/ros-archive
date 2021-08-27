/* radar_name.c
   ============ */


/*
 $Log: radar_name.c,v $
 Revision 1.3  1998/06/11 17:04:03  barnes
 Fixed problem of name list not being correctly initialized.

 Revision 1.2  1998/06/11 14:46:21  barnes
 No longer use a header to define the radar names but a data file.

 Revision 1.1  1998/06/03 18:57:12  root
 Initial revision

 */


#include <stdio.h>
#include <stdlib.h>
#include "radar_name.h"

static char *unknown={"Unknown"};

struct radar_id *load_radar(FILE *fp) {
  
  struct radar_id *ptr=NULL;
  int i,j,k;
  char line[256],name[64];
  int id,status;
  char code;
  int num=0;

  ptr=malloc(sizeof(struct radar_id));
  if (ptr==NULL) return NULL;
  ptr->num=0;
  ptr->radar=NULL;
    
  while(fgets(line,256,fp) !=NULL) {
    for (i=0;(line[i] !=0) && ((line[i]=='\n') || (line[i]==' '));i++);
    if (line[i]==0) continue;
    if (line[i]=='#') continue;

    /* extract id, name, code and status */

    for (j=i;(line[j] !=' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    line[j]=0;
    id=atoi(line+i);
    i=j+1;
    for (j=i;(line[j] !='"') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    i=j+1;
    for (k=0;(line[i+k] !='"') && (line[i+k] !=0) && (k<63);k++) 
       name[k]=line[i+k];
    if (line[i+k]==0) continue;
    if (k==63) continue; /* name too long */ 
    name[k]=0;
    i=i+k+1;
    for (j=i;(line[j]==' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    code=line[j];
    i=j+1;
    for (j=i;(line[j]==' ') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    status=atoi(line+i);
    i=j+1;

    if (ptr->radar==NULL) ptr->radar=malloc(sizeof(struct radar_name));
    else ptr->radar=realloc(ptr->radar,sizeof(struct radar_name)*(num+1));
    if (ptr->radar==NULL) break;

    ptr->radar[num].id=id;
    ptr->radar[num].code=code;
    ptr->radar[num].status=status;
    strcpy(ptr->radar[num].name,name);
    num++; 
  }
  ptr->num=num;
  return ptr;
}

char *radar_name(struct radar_id *ptr,int st_id) {
  int i;
  for (i=0;(i<ptr->num) && (ptr->radar[i].id !=st_id);i++);
  if (i==ptr->num) return unknown;
  return ptr->radar[i].name;
}

char radar_code(struct radar_id *ptr,int st_id) {
  int i;
  for (i=0;(i<ptr->num) && (ptr->radar[i].id !=st_id);i++);
  if (i==ptr->num) return 'u';
  return ptr->radar[i].code;
}


int radar_id(struct radar_id *ptr,char code) {
  int i;
  for (i=0;(i<ptr->num) && (ptr->radar[i].code !=code);i++);
  if (i==ptr->num) return -1;
  return ptr->radar[i].id;
}

int radar_status(struct radar_id *ptr,int st_id) {
  int i;
  for (i=0;(i<ptr->num) && (ptr->radar[i].id !=st_id);i++);
  if (i==ptr->num) return 0;
  return ptr->radar[i].status;
}













