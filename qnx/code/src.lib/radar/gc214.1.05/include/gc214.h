/* gc214.h
   =======
   Author: R.J.Barnes
*/

/*
 Copyright © 2001 The Johns Hopkins University/Applied Physics Laboratory.
 All rights reserved.
 
 This material may be used, modified, or reproduced by or for the U.S.
 Government pursuant to the license rights granted under the clauses at DFARS
 252.227-7013/7014.
 
 For any other permissions, please contact the Space Department
 Program Office at JHU/APL.
 
 This Distribution and Disclaimer Statement must be included in all copies of
 RST-ROS (hereinafter "the Program").
 
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

#define SCAN_OK 0
#define SCAN_ERROR 'F'

int GC214GetBufNum(ipcid_t ipcid);
int GC214GetBufSize(ipcid_t ipcid);
void *GC214GetBufAdr(ipcid_t ipcid,int buffer);
int GC214GetScanStatus(ipcid_t ipcid);
int GC214DoScan(ipcid_t ipcid,int buffer,int sample,int channel);
int GC214DoFCLRScan(ipcid_t ipcid,int buffer,int sample,int bandwidth);
int GC214SetSRate(ipcid_t ipcid,int srate);

#endif

