/* rxml.c
   ======
   Author: R.J.Barnes
*/
 

/*
 (c) 2009 JHU/APL
 
*/

#include <stdio.h>
#include <stdlib.h>
#include "rxml.h"



int XMLCallEnd(struct XMLdata *ptr,char *name,char *buf,int sze) {
  if (ptr->interface.end.user !=NULL) 
      return (ptr->interface.end.user)(name,buf,sze,ptr->interface.end.data);
  return 0;   
}

int XMLCallStart(struct XMLdata *ptr,
                 char *name,char end,int atnum,char **atname,char **atval,
                 char *buf,int sze) {
  
 if (ptr->interface.start.user !=NULL)
   return (ptr->interface.start.user)(name,end,atnum,atname,atval,
                               buf,sze,ptr->interface.start.data);
  return 0;

}






