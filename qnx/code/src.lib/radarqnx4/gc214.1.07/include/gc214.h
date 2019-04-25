/* gc214.h
   =======
   Author: R.J.Barnes & D.Andre
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
 $Log: gc214.h,v $
 Revision 1.5  2007/03/27 19:00:03  code
 Added Todd's meteor code.

 Revision 1.4  2007/03/27 18:57:11  code
 Changed the sample rate to a float.

 Revision 1.3  2006/01/25 16:10:03  barnes
 Added scan overflow condition.

 Revision 1.2  2005/06/08 16:00:01  barnes
 Dieter Andre added GC214SetTxFreq for RF mode operation.

 Revision 1.1  2004/07/12 22:36:01  barnes
 Initial revision

*/

#ifndef _GC214_H
#define _GC214_H


#define GET_BUF_NUM 'n'
#define GET_BUF_ADR 'a'
#define GET_BUF_SIZE 's'
#define GET_SCAN_STATUS 'S'
#define DRV_ERROR 'e'
#define DO_SCAN 'd'
#define DO_FCLR_SCAN 'f'
#define SET_SRATE 'r'
#define SET_TX_FREQ 'T'

#define SCAN_OK 0
#define SCAN_ERROR 'F'
#define SCAN_OVERFLOW 'O'

#define SET_METEOR_SRATE 'm'
#define SET_METEOR_SAMPLE 'M'


int GC214GetBufNum(ipcid_t ipcid);
int GC214GetBufSize(ipcid_t ipcid);
void *GC214GetBufAdr(ipcid_t ipcid,int buffer);
int GC214GetScanStatus(ipcid_t ipcid);
int GC214DoScan(ipcid_t ipcid,int buffer,int sample,int channel);
int GC214DoFCLRScan(ipcid_t ipcid,int buffer,int sample,int bandwidth);
int GC214SetSRate(ipcid_t ipcid,float srate);
int GC214SetTxFreq(ipcid_t ipcid,int tfreq);

int GC214SetMeteorSRate(ipcid_t ipcid,float msrate);
int GC214SetMeteorSample(ipcid_t ipcid,int msample);


#endif

