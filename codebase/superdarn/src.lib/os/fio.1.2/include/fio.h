/* fio.h
   ========
   Author: R.J.Barnes
*/

/*
 (c) 2009 JHU/APL
 
*/

#ifndef _FIO_H
#define _FIO_H

char *FIOMakeFile(char *pathenv,int yr,int mo,int dy,int hr,int mt,int sc,
		  char *code,char *extra,char *ext,int mode,int flag);

#endif 
