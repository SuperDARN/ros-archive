/* dio_msg.h
   =========
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
 $Log: dio_msg.h,v $
 Revision 1.2  2004/11/12 15:13:41  barnes
 Modifications to deal with digitally phased radars.

 Revision 1.1  2004/07/13 22:59:15  barnes
 Initial revision

*/


#define DIO_PRIO 15

#define STAT_THRESH 3

#define LOCAL_FREQ 2
#define UNKNOWN_TYPE 0x7f
#define DIO_OK 0
#define DIO_ERR 'R'
#define SET_GAIN 'G'
#define SET_BEAM 'B'
#define SET_FILTER 'X'
#define SET_FREQ 'F'
#define RESET_XT 'r'
#define SET_ANTENNA 'A'
#define SET_PHASE 'P'
#define TEST_MODE 'T'
#define SEND_FCLR 'c'

#define READ_GAIN 'g'
#define READ_BEAM 'b'
#define READ_PHASE  'p'
#define READ_FREQ 'f'
#define READ_FILTER 'x'

#define SET_TSG 'S'

#define SEND_TSG 'o'

#define VALID_ID 'V'

#define READ_CHANNEL 'k'
#define SET_CHANNEL 'l' 

#define GET_STATUS_NOFULL_NOCLR '0'
#define GET_STATUS_FULL_NOCLR '1'
#define GET_STATUS_NOFULL_CLR '2'
#define GET_STATUS_FULL_CLR '3'

#define GET_FREQ_MODE 'm'
#define GET_TSTATUS 's'
