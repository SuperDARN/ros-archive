/* limit.h
   =======
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
 $Log: limit.h,v $
 Revision 1.7  2006/07/10 16:08:23  code
 Added definitions for the origin array lengths.

 Revision 1.6  2006/05/10 21:34:07  barnes
 Fixed on 96 as the size of the lag table.

 Revision 1.5  2006/05/10 17:46:06  barnes
 Changed limits of lag table again.

 Revision 1.4  2006/05/05 14:34:32  barnes
 Changed size of lag table.

 Revision 1.3  2006/05/01 18:07:47  barnes
 Changed limits to deal with really long sequences.

 Revision 1.2  2006/03/14 21:48:05  barnes
 Modification to deal with extra beams.

 Revision 1.1  2004/07/12 22:24:49  barnes
 Initial revision

*/

#ifndef _LIMIT_H
#define _LIMIT_H

#define ORIGIN_TIME_SIZE 26
#define ORIGIN_COMMAND_SIZE 128
#define PULSE_SIZE 64    
#define LAG_SIZE 96	       
#define MAX_RANGE 300	
#define COMBF_SIZE 80       
#define MAX_BEAM 255
#endif
