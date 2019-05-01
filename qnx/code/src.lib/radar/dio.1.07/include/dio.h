/* dio.h
   ========
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
 $Log: dio.h,v $
 Revision 1.1  2004/07/12 22:30:51  barnes
 Initial revision

*/

#ifndef _DIO_H
#define _DIO_H
      
#define FREQ_LOCAL 2
#define UNKNOWN_TYPE 0x7f
#define DIO_OK 0
#define DIO_ERR 'R'
#define SET_GAIN 'G'
#define SET_BEAM 'B'
#define SET_FREQ 'F'
#define DIO_RESET 'r'
#define SEND_FCLR 'c'
#define SET_FILTER 'X'
#define SET_ANTENNA 'A'
#define SET_TEST_MODE 'T' 

#define SET_CHANNEL 'l'

#define SET_TSG 'S'
#define GET_TSTATUS 's'

#define SEND_TSG 'o'

#define VERIFY_ID 'V'

#define GET_STATUS_NOFULL_NOCLR '0'
#define GET_STATUS_FULL_NOCLR '1'
#define GET_STATUS_NOFULL_CLR '2'
#define GET_STATUS_FULL_CLR '3'
#define GET_FREQ_MODE 'm'

#define PULSE_ATTEN 0x08
#define PULSE_TX 0x04
#define PULSE_RX 0x02
#define PULSE_SAMPLE 0x01
#define PULSE_PHASE 0x10


int DIOSetGain(ipcid_t ipcid,unsigned char atten);
int DIOSetFilter(ipcid_t ipcid,unsigned char filter);
int DIOSetChannel(ipcid_t ipcid,unsigned char channel);
int DIOSetBeam(ipcid_t ipcid,unsigned char beam);
int DIOSetAntenna(ipcid_t ipcid,unsigned char mode,
		  unsigned char bmnum);
int DIOSetTestMode(ipcid_t ipcid,unsigned char mode);
int DIOSetFreq(ipcid_t ipcid,int freq);
int DIOReadFreqMode(ipcid_t ipcid);
int DIOReadTStatus(ipcid_t ipcid);
int DIOReset(ipcid_t ipcid);
int DIOVerifyID(ipcid_t ipcid,unsigned char seq);
int DIOSetTSG(ipcid_t ipcid,unsigned char id,int length,char *codebyte,
	      char *repbyte, char *xtdbyte);
int DIOSendTSG(ipcid_t ipcid,int stereo,unsigned char *id,
	       unsigned char *xtdflag,unsigned char *stflag,
	       int *delay,int **freq);
int DIOSendFCLR(ipcid_t ipcid,int stereo,unsigned char *id,
		int *delay,int *freqnum,int **freqtable);
int DIOGetStatus(ipcid_t ipcid,int clear,
		 int full,int *lstat,int *astat);

#endif
