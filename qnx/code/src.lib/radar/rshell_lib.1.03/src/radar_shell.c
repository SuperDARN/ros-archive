/* radar_shell.c
   =============
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
$Log: radar_shell.c,v $
Revision 1.15  2001/06/27 20:54:04  barnes
Added license tag

Revision 1.14  2001/01/29 18:11:50  barnes
Added Author Name

Revision 1.13  1999/09/15 02:16:32  barnes
Added generic int type for 64-bit issues.

Revision 1.12  1999/09/15 02:07:48  barnes
Added standard int type in case of 64-bit issues.

Revision 1.11  1999/09/09 23:49:18  barnes
Modifications to the way the argument table is managed.

Revision 1.10  1999/04/07 21:35:14  barnes
Added a new function to poll scheduler without starting a shell.

Revision 1.9  1999/03/30 19:33:25  barnes
Moved schedule task name into header.

Revision 1.8  1999/03/30 19:31:40  barnes
Moved task name to a header.

Revision 1.7  1999/03/30 19:04:05  barnes
Modifications for the new scheduler.

Revision 1.6  1999/03/30 16:12:42  barnes
Changed schedule default name to local rather than global.

Revision 1.5  1999/03/29 16:35:42  barnes
Removed global variables.

Revision 1.4  1998/11/04 20:58:11  barnes
Fixed bugs associated with the buffer changing location in memory and
the pointers becoming corrupt.

Revision 1.3  1998/11/04 19:30:15  barnes
Fixed bug in allocating strings.

Revision 1.2  1998/11/04 18:37:31  barnes
Changed method of operation so that the shell does not have to wait
for two integration periods.

Revision 1.1  1998/10/28 16:26:29  barnes
Initial revision

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <sys/kernel.h>
#include <sys/name.h>
#include <sys/sendmx.h>
#include "task_name.h"

#define DELIM  " \n"

#include "radar_shell.h"

#define SHELL_SEND 's'
#define SHELL_SEND_MSG 'm'
#define SHELL_REPLY 'r'
  
struct rslist { /* structure for user defined variable list */
  short int type; /* type */
  short int cflag; /* changed flag */
  void *data;  /* pointer to our copy of the variable */
  long int copy; /* pointer to temporary copy of the variable */
  long int next; /* offset to next entry */
};

#define EXIT 'E'

#define CONTROL_PRIORITY 10

void terminate(pid_t schedule_id) {
  Reply(schedule_id,NULL,0);
}

pid_t register_program(char *schedule_name,char *control_name) {
  int status=0;
  int cp_nid=0;
  pid_t schedule_id=-1;
  /* change our priority to override scheduler */

  if(setprio(0,CONTROL_PRIORITY) == -1) {
     perror("unable to set priority\n");
	 exit(1);
   }

  if (control_name==NULL) cp_nid=qnx_name_attach(0,CONTROL_NAME); 
  else cp_nid=qnx_name_attach(0,control_name);

  if (schedule_name==NULL) 
    schedule_id=qnx_name_locate(0,SCHEDULE_NAME,0,NULL);
  else schedule_id=qnx_name_locate(0,schedule_name,0,NULL);
  return schedule_id;
}
 
int rs_replace(struct rslist *rs_replace,
                struct rslist *rs_search) {
  struct rslist *rs_ptr;
  rs_ptr=rs_replace;
  while (1) {
    
    if (strcmp((char *) (rs_search+1),(char *) (rs_ptr+1))==0) {
      if (rs_ptr->type==var_SHORT) {
	    *((short int *) (rs_ptr->data))=
	    *((short int *) ( (int) rs_search+rs_search->copy));
       } else if (rs_ptr->type==var_LONG) {
	    *((long int *) (rs_ptr->data))=
	    *((long int *) ( (int) rs_search+rs_search->copy));
	   } else if (rs_ptr->type==var_FLOAT) {
	     *((float *) (rs_ptr->data))= 
		 *((float *) ( (int) rs_search+rs_search->copy));
	   } else if (rs_ptr->type==var_DOUBLE) {
	     *((double *) (rs_ptr->data))= 
	     *((double *) ( (int) rs_search+rs_search->copy));
	   } else if (rs_ptr->type==var_STRING) {
	     if ((rs_ptr->data !=NULL) && (rs_search->data !=NULL)) 
            strcpy ( (char *) (rs_ptr->data), 
		             (char *) ( (int) rs_search+rs_search->copy));
	   } else {
	     *((int *) (rs_ptr->data))= 
	     *((int *) ( (int) rs_search +rs_search->copy));	
	   }
    }
	if (rs_ptr->next==0) break;											
	rs_ptr=(struct rslist *) ( (int) rs_ptr+rs_ptr->next);
  }
  return 0;
}

int rs_shell(pid_t schedule_id,struct rsentry *ptr) {
  
 
   int status=0;
   int wait=1;
   int block=0;
   int offset=0;
   char *buffer=NULL; /* workspace */
   char *tbuffer=NULL;
   int extent=0;
   int textent=0;

   char *parse_option=NULL;
   char *variable;
   char *type;
   char msg;
   struct _mxfer_entry mx[3];   
   struct rslist *rs_ptr=NULL;
   pid_t pid;

   /* check for radar_shell task */

   pid=Creceive(0,&msg,sizeof(msg));
   if (pid==-1) return 0;
   if ((pid==schedule_id) && (msg==EXIT)) return 1;

   /* decode the variable list */

   if (ptr !=NULL) {
     int i=0;
     while (ptr[i].name !=0) {
	   if (buffer ==NULL) buffer=malloc(sizeof(struct rslist)+1
		 			     +strlen(ptr[i].name));
	   else buffer=realloc(buffer,extent+sizeof(struct rslist)+1
		   				   +strlen(ptr[i].name)); 
	   offset=extent;
  	   rs_ptr=(struct rslist *) (buffer+offset);  
	   block=sizeof(struct rslist)+1+strlen(ptr[i].name); 
	   extent+=block;
	   strcpy( (char *) (rs_ptr+1),ptr[i].name); /* copy variable name */
	   rs_ptr->copy=block;

	   rs_ptr->data=ptr[i].data; 
           rs_ptr->cflag=0;
		 
	   /* copy the value of the variable */
	   if (ptr[i].type==var_SHORT) {
	     rs_ptr->type=var_SHORT;
		 buffer=realloc(buffer,extent+sizeof(short int));
  	     rs_ptr=(struct rslist *) (buffer+offset); 
		 extent+=sizeof(short int);
		 block+=sizeof(short int);
		 *( (short int *) ( (int) rs_ptr+rs_ptr->copy))=
		 *( (short int *) rs_ptr->data);
           } else if (ptr[i].type==var_LONG) {
	         rs_ptr->type=var_LONG;
		 buffer=realloc(buffer,extent+sizeof(long int));
  	         rs_ptr=(struct rslist *) (buffer+offset); 
		 extent+=sizeof(long int);
		 block+=sizeof(long int);
		 *( (long int *) ( (int) rs_ptr+rs_ptr->copy))=
		 *( (long int *) rs_ptr->data);
	   } else if (ptr[i].type==var_FLOAT) {
	     rs_ptr->type=var_FLOAT;
		 buffer=realloc(buffer,extent+sizeof(float));			   
  	     rs_ptr=(struct rslist *) (buffer+offset); 
		 extent+=sizeof(float);
		 block+=sizeof(float);		   
		 *( (float *) ( (int) rs_ptr+rs_ptr->copy))=
		 *( (float *) rs_ptr->data);		   
	   } else if (ptr[i].type==var_DOUBLE) {
		 rs_ptr->type=var_DOUBLE;
		 buffer=realloc(buffer,extent+sizeof(double));				   
  	     rs_ptr=(struct rslist *) (buffer+offset); 
		 extent+=sizeof(double);
		 block+=sizeof(double);		   
		 *( (double *) ( (int) rs_ptr+rs_ptr->copy))=
		 *( (double *) rs_ptr->data);		   
	   } else if (ptr[i].type==var_STRING) {
	     rs_ptr->type=var_STRING;
		 if (rs_ptr->data !=NULL) {
		   buffer=realloc(buffer,extent+STR_MAX+1);
    	   rs_ptr=(struct rslist *) (buffer+offset); 
           strncpy( (char *) ( (int) rs_ptr+rs_ptr->copy),
			 	  (char *) (rs_ptr->data),STR_MAX);
		   extent+=STR_MAX+1;
   		   block+=STR_MAX+1;		   
	     }  		   
	   } else {
	          rs_ptr->type=var_INT;
		 buffer=realloc(buffer,extent+sizeof(int)); 
    	         rs_ptr=(struct rslist *) (buffer+offset); 
		 extent+=sizeof(int);
		 block+=sizeof(int);
		 *( (int *) ( (int) rs_ptr+rs_ptr->copy))=
		 *( (int *) rs_ptr->data);
           } 
	   rs_ptr->next=block;
	   i++;
     }
   } 
   if (rs_ptr !=NULL) rs_ptr->next=0;

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
      rs_ptr=(struct rslist *) tbuffer;
      while (1) { 
        if (rs_ptr->cflag==1) 
        rs_replace( (struct rslist *) buffer,rs_ptr);
	    if (rs_ptr->next==0) break;											
	    rs_ptr=(struct rslist *) ( (int) rs_ptr+rs_ptr->next);
      }
      free(tbuffer);
      tbuffer=NULL;
    }
    if (tbuffer !=NULL) free(tbuffer);
    if (buffer !=NULL) free (buffer);
    buffer=NULL;
  }
  if (buffer !=NULL) free(buffer);
  return 0;
}
 
int poll_exit(pid_t schedule_id) {
  
   char msg;
   pid_t pid;

   /* check for radar_shell task */

   pid=Creceive(0,&msg,sizeof(msg));
   if (pid==-1) return 0;
   if ((pid==schedule_id) && (msg==EXIT)) return 1;
   return 0;
}


int radar_shell(pid_t schedule_id,struct rstable *rptr) {
  return rs_shell(schedule_id,rptr->ptr);
}

int radar_shell_add(struct rstable *rptr,char *name,int type,void *data) {
  
  if (rptr->ptr !=NULL) {
    rptr->ptr=realloc(rptr->ptr,sizeof(struct rsentry)*(rptr->num+2));
    if (rptr->ptr==NULL) return -1;
  } else {
    rptr->ptr=malloc(sizeof(struct rsentry)*2);
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



void radar_shell_free(struct rstable *rptr) {
  if (rptr->ptr !=NULL) {
    int i;
    for (i=0;(rptr->ptr[i].name !=NULL);i++) free(rptr->ptr[i].name);
    free(rptr->ptr);
  }
  rptr->ptr=NULL;
  rptr->num=0;
}

struct rsentry *radar_shell_read(struct rstable *rptr,int num) {
  if (rptr==NULL) return NULL;
  if (num>=rptr->num) return NULL;
  return &rptr->ptr[num];
}
  

int radar_shell_parse(struct rstable *rptr,char *name,...) {
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
  
    if (data !=NULL) status=radar_shell_add(rptr,vname,type,data);
    
    if (status !=0) return -1;
    
    vname=strtok(NULL,DELIM);
  }
  va_end(ap);
  return 0;
}

  
  
