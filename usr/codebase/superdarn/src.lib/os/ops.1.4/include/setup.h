/* setup.h
   ======= 
   Author: R.J.Barnes
*/

/*
 $License$
*/

#ifndef _SETUP_H
#define _SETUP_H

#define START_STRING "PROGRAM START->"

int OpsSetupCommand(int argc,char *argv[]);
int OpsStart(char *ststr);
int OpsFitACFStart();
void OpsLogStart(int sock,char *name,int argc,char *argv[]);
void OpsSetupTask(int tnum,struct TCPIPMsgHost *task,int sock,char *name);
void OpsSetupShell();
  
#endif
