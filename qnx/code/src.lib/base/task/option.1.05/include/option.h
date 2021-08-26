/* option.h
   ======== 
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
 $Log: option.h,v $
 Revision 1.2  2005/08/05 18:30:28  barnes
 Added function for dumping command line options.

 Revision 1.1  2003/03/04 13:18:15  barnes
 Initial revision

*/


#ifndef _OPTION_H
#define _OPTION_H

struct OptionEntry {
 char *name;
 char type;
 int set;
 void *ptr;
};

struct OptionData {
  int num;
  struct OptionEntry *ptr;
};

struct OptionText {
  int num;
  char **txt;
};

struct OptionFile {
  int argc;
  char **argv;
};

char *OptionScanFileArg(FILE *file);
struct OptionFile *OptionProcessFile(FILE *fp);
void OptionFreeFile(struct OptionFile *ptr);
int OptionProcess(int offset,int argc,char *argv[], struct OptionData *opt,
		  int (*opterr)(char *));
int OptionAdd(struct OptionData *opt,char *name,char type,void *data);

int OptionDump(FILE *fp,struct OptionData *opt);
void OptionFree(struct OptionData *opt);



void OptionPrintInfo(FILE *fp,char *str[]);

#endif

