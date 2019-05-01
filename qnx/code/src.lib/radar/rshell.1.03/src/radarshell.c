/* radarshell.c
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



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#ifdef _QNX4
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/sendmx.h>
#include <sys/sched.h>
#endif
#include <sys/types.h>
#include "rtypes.h"
#include "taskname.h"

#define DELIM  " \n"

#include "radarshell.h"

#define SHELL_SEND 's'
#define SHELL_SEND_MSG 'm'
#define SHELL_REPLY 'r'

#define EXIT 'E'
#define CONTROL_PRIORITY 10
  


/*
 $Log: radarshell.c,v $
 Revision 1.3  2004/06/16 22:09:32  barnes
 Removed redudant code and added missing header.

 Revision 1.2  2004/05/06 19:11:49  barnes
 Defined type ipcid_t type for the IPC calls.

 Revision 1.1  2003/11/05 16:35:23  barnes
 Initial revision

*/

void RShellTerminate(ipcid_t sid) {
#ifdef _QNX4
  Reply(sid,NULL,0);
#endif
}

ipcid_t RShellRegister(char *schedule,char *control) {
#ifdef _QNX4
  int cpnid=0;
  pid_t sid=-1;
  /* change our priority to override scheduler */

  if (setprio(0,CONTROL_PRIORITY) == -1) {
    perror("unable to set priority\n");
    exit(1);
  }

  if (control==NULL) cpnid=qnx_name_attach(0,CONTROL_NAME); 
  else cpnid=qnx_name_attach(0,control);

  if (schedule==NULL) 
    sid=qnx_name_locate(0,SCHEDULE_NAME,0,NULL);
  else sid=qnx_name_locate(0,schedule,0,NULL);
  return sid;
#else
  return 0;
#endif
}
 
int RShellReplace(struct RShellList *replace,
                struct RShellList *search) {
  struct RShellList *ptr;
  ptr=replace;
  while (1) {
    
    if (strcmp((char *) (search+1),(char *) (ptr+1))==0) {
      if (ptr->type==var_SHORT) {
        *((short int *) (ptr->data))=
	  *((short int *) ( (int) search+search->copy));
      } else if (ptr->type==var_LONG) {
        *((long int *) (ptr->data))=
	  *((long int *) ( (int) search+search->copy));
      } else if (ptr->type==var_FLOAT) {
	*((float *) (ptr->data))= 
	  *((float *) ( (int) search+search->copy));
      } else if (ptr->type==var_DOUBLE) {
        *((double *) (ptr->data))= 
	  *((double *) ( (int) search+search->copy));
      } else if (ptr->type==var_STRING) {
        if ((ptr->data !=NULL) && (search->data !=NULL)) 
        strcpy ( (char *) (ptr->data), 
		 (char *) ( (int) search+search->copy));
      } else {
        *((int *) (ptr->data))= 
	  *((int *) ( (int) search +search->copy));	
      }
    }
    if (ptr->next==0) break;
    ptr=(struct RShellList *) ( (int) ptr+ptr->next);
  }
  return 0;
}

int RShell(ipcid_t sid,struct RShellEntry *ptr) {
#ifdef _QNX4
  int block=0;
  int offset=0;
  char *buffer=NULL; /* workspace */
  char *tbuffer=NULL;
  int extent=0;
  int textent=0;

  char msg;
  struct _mxfer_entry mx[3];   
  struct RShellList *rsptr=NULL;
  pid_t pid;

  /* check for radar_shell task */

  pid=Creceive(0,&msg,sizeof(msg));
  if (pid==-1) return 0;
  if ((pid==sid) && (msg==EXIT)) return 1;

  /* decode the variable list */

  if (ptr !=NULL) {
    int i=0;
    while (ptr[i].name !=0) {
      if (buffer ==NULL) buffer=malloc(sizeof(struct RShellList)+1
		 		       +strlen(ptr[i].name));
      else buffer=realloc(buffer,extent+sizeof(struct RShellList)+1
		   	  +strlen(ptr[i].name)); 
      offset=extent;
      rsptr=(struct RShellList *) (buffer+offset);  
      block=sizeof(struct RShellList)+1+strlen(ptr[i].name); 
      extent+=block;
      strcpy( (char *) (rsptr+1),ptr[i].name); /* copy variable name */
      rsptr->copy=block;

      rsptr->data=ptr[i].data; 
      rsptr->cflag=0;
		 
      /* copy the value of the variable */
      if (ptr[i].type==var_SHORT) {
        rsptr->type=var_SHORT;
	buffer=realloc(buffer,extent+sizeof(short int));
  	rsptr=(struct RShellList *) (buffer+offset); 
	extent+=sizeof(short int);
	block+=sizeof(short int);
	*( (short int *) ( (int) rsptr+rsptr->copy))=
	  *( (short int *) rsptr->data);
      } else if (ptr[i].type==var_LONG) {
        rsptr->type=var_LONG;
	buffer=realloc(buffer,extent+sizeof(long int));
  	rsptr=(struct RShellList *) (buffer+offset); 
        extent+=sizeof(long int);
	block+=sizeof(long int);
	*( (long int *) ( (int) rsptr+rsptr->copy))=
	  *( (long int *) rsptr->data);
      } else if (ptr[i].type==var_FLOAT) {
        rsptr->type=var_FLOAT;
        buffer=realloc(buffer,extent+sizeof(float));
        rsptr=(struct RShellList *) (buffer+offset); 
	extent+=sizeof(float);
	block+=sizeof(float);		   
	*( (float *) ( (int) rsptr+rsptr->copy))=
	  *( (float *) rsptr->data);		   
      } else if (ptr[i].type==var_DOUBLE) {
        rsptr->type=var_DOUBLE;
	buffer=realloc(buffer,extent+sizeof(double));
        rsptr=(struct RShellList *) (buffer+offset); 
	extent+=sizeof(double);
	block+=sizeof(double);		   
	*( (double *) ( (int) rsptr+rsptr->copy))=
	  *( (double *) rsptr->data);		   
      } else if (ptr[i].type==var_STRING) {
        rsptr->type=var_STRING;
	if (rsptr->data !=NULL) {
          buffer=realloc(buffer,extent+STR_MAX+1);
    	  rsptr=(struct RShellList *) (buffer+offset); 
          strncpy( (char *) ( (int) rsptr+rsptr->copy),
		   (char *) (rsptr->data),STR_MAX);
	  extent+=STR_MAX+1;
   	  block+=STR_MAX+1;		   
        }  		   
      } else {
        rsptr->type=var_INT;
	buffer=realloc(buffer,extent+sizeof(int)); 
    	rsptr=(struct RShellList *) (buffer+offset); 
	extent+=sizeof(int);
	block+=sizeof(int);
	*( (int *) ( (int) rsptr+rsptr->copy))=
	*( (int *) rsptr->data);
      } 
      rsptr->next=block;
      i++;
    }
  } 
  if (rsptr !=NULL) rsptr->next=0;

  /* send the variable list to the task here */
  
  if (msg==SHELL_SEND) {
    _setmx(&mx[0],&extent,sizeof(int));
    Replymx(pid,1,mx);
    if (extent !=0) { /* wait for the other half of the message */
      pid=Receive(pid,&msg,sizeof(msg));
      if (msg==SHELL_SEND_MSG) {
        _setmx(&mx[0],buffer,extent);
        Replymx(pid,1,mx);
      } else Reply(pid,0,0);
    } 
  } else if (msg==SHELL_REPLY) {
    _setmx(&mx[0],&textent,sizeof(int));
    Readmsgmx(pid,sizeof(msg),1,mx);
    if ((textent !=0) && ((tbuffer=malloc(textent)) !=NULL)) {
      Readmsg(pid,sizeof(msg)+sizeof(long int),tbuffer,textent);
    }
    Reply(pid,0,0); 
    if ((buffer !=NULL) && (tbuffer !=NULL)) {
      rsptr=(struct RShellList *) tbuffer;
      while (1) { 
        if (rsptr->cflag==1) 
          RShellReplace( (struct RShellList *) buffer,rsptr);
        if (rsptr->next==0) break;
        rsptr=(struct RShellList *) ( (int) rsptr+rsptr->next);
      }
      free(tbuffer);
      tbuffer=NULL;
    }
    if (tbuffer !=NULL) free(tbuffer);
    if (buffer !=NULL) free (buffer);
    buffer=NULL;
  }
  if (buffer !=NULL) free(buffer);
#endif
  return 0;
}
 
int RShellPollExit(ipcid_t sid) {
#ifdef _QNX4  
  char msg;
  pid_t pid;

  /* check for radar_shell task */

  pid=Creceive(0,&msg,sizeof(msg));
  if (pid==-1) return 0;
  if ((pid==sid) && (msg==EXIT)) return 1;
#endif
  return 0;
}


int RadarShell(ipcid_t sid,struct RShellTable *rptr) {
  return RShell(sid,rptr->ptr);
}

int RadarShellAdd(struct RShellTable *rptr,
	      char *name,int type,void *data) {
  
  if (rptr->ptr !=NULL) {
    rptr->ptr=realloc(rptr->ptr,sizeof(struct RShellEntry)*(rptr->num+2));
    if (rptr->ptr==NULL) return -1;
  } else {
    rptr->ptr=malloc(sizeof(struct RShellEntry)*2);
    if (rptr->ptr==NULL) return -1;
    rptr->num=0; 
  }
  rptr->ptr[rptr->num].name=malloc(strlen(name)+1);
  strcpy(rptr->ptr[rptr->num].name,name);
  rptr->ptr[rptr->num].type=type;
  rptr->ptr[rptr->num].data=data;
  rptr->ptr[rptr->num+1].name=NULL;
  rptr->num++;
  return 0;
}



void RadarShellFree(struct RShellTable *rptr) {
  if (rptr->ptr !=NULL) {
    int i;
    for (i=0;(rptr->ptr[i].name !=NULL);i++) free(rptr->ptr[i].name);
    free(rptr->ptr);
  }
  rptr->ptr=NULL;
  rptr->num=0;
}

struct RShellEntry *RadarShellRead(struct RShellTable *rptr,int num) {
  if (rptr==NULL) return NULL;
  if (num>=rptr->num) return NULL;
  return &rptr->ptr[num];
}
  

int RadarShellParse(struct RShellTable *rptr,char *name,...) {
  int status=0;
  va_list ap;
  char *vname=NULL;
  char *vtype=NULL;
  void *data=NULL;
  int type=0;

  if (name ==NULL) return 0;
  va_start(ap,name);
  vname=strtok(name,DELIM);
  while (vname !=NULL) {

    vtype=strtok(NULL,DELIM);
    if (vtype==NULL) break;
    
    type=var_STRING;
    if (strcmp(vtype,"s")==0) type=var_SHORT;
    if (strcmp(vtype,"i")==0) type=var_INT;
    if (strcmp(vtype,"l")==0) type=var_LONG;
    if (strcmp(vtype,"f")==0) type=var_FLOAT;
    if (strcmp(vtype,"d")==0) type=var_DOUBLE;
    data=va_arg(ap,void *);
  
    if (data !=NULL) status=RadarShellAdd(rptr,vname,type,data);
    
    if (status !=0) return -1;
    
    vname=strtok(NULL,DELIM);
  }
  va_end(ap);
  return 0;
}

  
  
