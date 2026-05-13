/* oldfitread.h
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
 $Log: oldfitread.h,v $
 Revision 1.3  2004/08/04 20:53:40  barnes
 Removed legacy code for memory bufferinf and implemented the ability
 to use open file pointers to create the structure.

 Revision 1.2  2004/08/04 20:34:15  barnes
 Added function to create file pointer for open files.

 Revision 1.1  2004/07/12 22:09:15  barnes
 Initial revision

*/

#ifndef _OLDFITREAD_H
#define _OLDFITREAD_H

struct OldFitFp {
  int fitfp;
  int inxfp;
  int fit_recl;
  int inx_recl;
  struct stat fstat;
  struct stat istat;
  int blen;
  int inx_srec;
  int inx_erec;
  double ctime;
  double stime;
  double etime;
  int time;
  char header[80];
  char date[32];
  char extra[256];
  char major_rev; 
  char minor_rev;
  int (*fitread)(struct OldFitFp *ptr,struct RadarParm *prm,
                 struct FitData *fit);
};


int OldFitReadInx(struct OldFitFp *ptr,int32 *buffer,int recno);
int OldFitRead(struct OldFitFp *ptr,struct RadarParm *prm,
               struct FitData *fit);
int OldFitSeek(struct OldFitFp *ptr,
               int yr,int mo,int dy,int hr,int mt,int sc,
	       double *atme);
void OldFitClose(struct OldFitFp *ptr);
struct OldFitFp *OldFitOpenFd(int fitfp,int inxfp);

struct OldFitFp *OldFitOpen(char *fitfile,char *inxfile);

#endif


