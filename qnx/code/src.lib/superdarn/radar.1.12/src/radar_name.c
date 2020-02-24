/* radar_name.c
   ============
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
 $Log: radar_name.c,v $
 Revision 1.6  2001/06/27 20:49:59  barnes
 Added license tag

 Revision 1.5  2001/01/29 18:11:53  barnes
 Added Author Name

 Revision 1.4  2000/04/19 16:13:56  barnes
 Modification to add the name of the operating institute.

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
  char line[256],name[64],operator[128];
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


    for (j=i;(line[j] !='"') && (line[j] !=0);j++);
    if (line[j]==0) continue;
    i=j+1;
    for (k=0;(line[i+k] !='"') && (line[i+k] !=0) && (k<127);k++) 
       operator[k]=line[i+k];
    if (line[i+k]==0) continue;
    if (k==127) continue; /* name too long */ 
    operator[k]=0;
    i=i+k+1;

    if (ptr->radar==NULL) ptr->radar=malloc(sizeof(struct radar_name));
    else ptr->radar=realloc(ptr->radar,sizeof(struct radar_name)*(num+1));
    if (ptr->radar==NULL) break;

    ptr->radar[num].id=id;
    ptr->radar[num].code=code;
    ptr->radar[num].status=status;
    strcpy(ptr->radar[num].name,name);
    strcpy(ptr->radar[num].operator,operator);
    num++; 
  }
  ptr->num=num;
  return ptr;
}

char *radar_operator(struct radar_id *ptr,int st_id) {
  int i;
  for (i=0;(i<ptr->num) && (ptr->radar[i].id !=st_id);i++);
  if (i==ptr->num) return unknown;
  return ptr->radar[i].operator;
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













