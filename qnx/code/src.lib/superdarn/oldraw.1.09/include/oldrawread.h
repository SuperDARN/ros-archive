/* raw_read.h
   ==========
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
 $Log: oldrawread.h,v $
 Revision 1.2  2004/08/04 13:20:28  barnes
 Added function to create file pointer for open files.

 Revision 1.1  2004/07/12 22:10:20  barnes
 Initial revision

*/

#ifndef _OLDRAWREAD_H
#define _OLDRAWREAD_H


struct OldRawFp {
  int rawfp;
  int inxfp;
  struct stat rstat;
  struct stat istat;
  double ctime;
  double stime;
  int frec;
  int rlen;
  int ptr;
  int thr;
  int major_rev;
  int minor_rev;
  int (*rawread)(struct OldRawFp *ptr,struct RadarParm *,struct RawData *);

};




int OldRawRead(struct OldRawFp *fp,struct RadarParm *prm,struct RawData *raw);
int OldRawReadData(struct OldRawFp *fp,struct RadarParm *prm,
		   struct RawData *raw);
int OldRawSeek(struct OldRawFp *ptr,int yr,int mo,int dy,int hr,int mt,int sc,
	     double *atme);
void OldRawClose(struct OldRawFp *fp);

struct OldRawFp *OldRawOpenFd(int rawfd,int inxfd);
struct OldRawFp *OldRawOpen(char *rawfile,char *inxfile);

#endif



