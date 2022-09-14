/* rscript.h
   ========= 
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/




#ifndef _RSCRIPT_H
#define _RSCRIPT_H

struct ScriptData {
  int depth;
  int sze;
  int max;
  int stp;
  char *buf;

  struct {
    int (*user)(char *,int,void *);
    void *data;
  } text;
 
};

struct ScriptData *ScriptMake();
void ScriptFree(struct ScriptData *ptr);
 
int ScriptSetText(struct ScriptData *ptr,
	       int (*text)(char *,int,void *),void *data);

int ScriptDecode(struct ScriptData *ptr,char *buf,int sze);
  
#endif
