/* radarshell.h
   ============
   Author: R.J.Barnes
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

/*
 $Log: radarshell.h,v $
 Revision 1.1  2004/07/12 22:38:39  barnes
 Initial revision

*/


#ifndef _RADARSHELL_H
#define _RADARSHELL_H


#define STR_MAX 128 

enum {  /* variable types */
  var_SHORT,
  var_INT,
  var_LONG,
  var_FLOAT,
  var_DOUBLE,
  var_STRING
};

struct RShellEntry {
  char *name; 
  int type;
  void *data;
};

struct RShellTable {
  int num;
  struct RShellEntry *ptr;
};

struct RShellList { /* structure for user defined variable list */
  short int type; /* type */
  short int cflag; /* changed flag */
  void *data;  /* pointer to our copy of the variable */
  long int copy; /* pointer to temporary copy of the variable */
  long int next; /* offset to next entry */
};



void RShellTerminate(ipcid_t sid);
ipcid_t RShellRegister(char *schedule,char *control); 
int RShellReplace(struct RShellList *replace,
		  struct RShellList *search);
int RShell(ipcid_t sid,struct RShellEntry *ptr);
int RShellPollExit(ipcid_t sid);
int RadarShell(ipcid_t sid,struct RShellTable *rptr);
int RadarShellAdd(struct RShellTable *rptr,
		  char *name,int type,void *data);
void RadarShellFree(struct RShellTable *rptr);
struct RShellEntry *RadarShellRead(struct RShellTable *rptr,int num);
int RadarShellParse(struct RShellTable *rptr,char *name,...);

#endif


