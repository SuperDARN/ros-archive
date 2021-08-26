/* read_tsg.c 
   ========== */

/*
 $Log: read_tsg.c,v $
 Revision 1.1  1998/10/27 19:15:30  barnes
 Initial revision

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_TSG 16000


int cnt=0,ex=0;
unsigned char code[MAX_TSG];
unsigned char rep[MAX_TSG];
unsigned char xtd[MAX_TSG];

int scan_bits(char *str,char c) {
  int i;
  for (i=0;(str[i] !=0) && (str[i] !=c);i++);
  return (str[i] !=0);
}

int read_tsg(FILE *fp) {
  char delim[]={' ','\t','\n'};
  char line[1024];
  int i;
  char *tkn;
  char *bits;
  int bit;
  char *fstr,*bstr,*rstr;
  cnt=0;
  ex=0;
  while (fgets(line,1024,fp) !=0) {
    for (i=0; (line[i] !=0) && 
              ((line[i] ==' ') || (line[i]=='\n'));i++);

    /* ignore comments or empty lines */

    if ((line[i]==0) || (line[i]=='#')) continue;
    tkn=line+i; 
    if ((cnt==0) && ((tkn[0]=='e') || (tkn[0]=='E'))) {
      ex=1; 
      continue;
    }
    bits=strtok(tkn,delim);
    if (ex==1) {
       bstr=strtok(NULL,delim);
       fstr=strtok(NULL,delim);
    }
    rstr=strtok(NULL,delim);
    if (rstr==NULL) continue;
    if ((ex==1) && ((bstr==NULL) || (fstr==NULL))) continue;

    if (ex==1) {
      unsigned int b;
      unsigned int f;
      b=atoi(bstr);
      f=atoi(fstr);
      f=f & 0x01;
      b=b & 0x0f;
      xtd[cnt]=b | (f<<4);
    } 

    code[cnt]=0;
    if (scan_bits(bits,'s') !=0) code[cnt]=code[cnt] | 0x01;
    if (scan_bits(bits,'S') !=0) code[cnt]=code[cnt] | 0x01;
    if (scan_bits(bits,'r') !=0) code[cnt]=code[cnt] | 0x02;
    if (scan_bits(bits,'R') !=0) code[cnt]=code[cnt] | 0x02;
    if (scan_bits(bits,'x') !=0) code[cnt]=code[cnt] | 0x04;
    if (scan_bits(bits,'X') !=0) code[cnt]=code[cnt] | 0x04;
    if (scan_bits(bits,'a') !=0) code[cnt]=code[cnt] | 0x08;
    if (scan_bits(bits,'A') !=0) code[cnt]=code[cnt] | 0x08;
    if (scan_bits(bits,'p') !=0) code[cnt]=code[cnt] | 0x10;
    if (scan_bits(bits,'P') !=0) code[cnt]=code[cnt] | 0x10;
    rep[cnt]=atoi(rstr) & 0xff;
    cnt++;

  }
  if (cnt==0) return -1;
  return 0;
}
