/* reset.c
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
 $Log: reset.c,v $
 Revision 1.8  2001/06/27 20:55:38  barnes
 Added license tag

 Revision 1.7  2001/01/29 18:12:07  barnes
 Added Author Name

 Revision 1.6  2000/09/18 17:57:09  barnes
 Removed redundant header.

 Revision 1.5  1998/11/07 00:19:16  barnes
 Extensive changes for stereo operations.
 Moved all of the hard coded configuration information into a configuration
 file.

 Revision 1.4  1998/09/11 17:33:25  root
 Modifications to remove short integer types.

 Revision 1.3  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.2  1997/06/10 20:17:13  root
 Moved sd_radar_id.h header.

 Revision 1.1  1996/03/13 15:16:34  root
 Initial revision

 */

/* Resets the DIO board */

#include "DIO.h"
#include "PIO48.h"
#include "port.h"

extern struct dio_hdw hdw;

int reset(int stereo) {

  /*  Initialize the state of the digital i/o cards */

  PUT_DIO(0x03,hdw.control_word[0],0xff);
  PUT_DIO(0x07,hdw.control_word[1],0xff);

  if (hdw.control_word[2] !=-1) { 
    PUT_DIO(0x13,hdw.control_word[2],0xff);
    PUT_DIO(0x17,hdw.control_word[3],0xff);
  }

  PUT_DIO(hdw.one_shot_cw[0], i8254_MODE_1 +
					   i8254_SC1 +
					   i8254_RLL,  0xFF); 
  PUT_DIO(hdw.one_shot[0],3,  0xFF); 
  PUT_DIO(hdw.one_shot_cw[0], i8254_MODE_1 +
					   i8254_SC2 +
					   i8254_RLL,  0xFF);
  if ((stereo !=0) && (hdw.one_shot_cw[0] != hdw.one_shot_cw[1])) {
    PUT_DIO(hdw.one_shot_cw[1], i8254_MODE_1 +
  					        i8254_SC1 +
					        i8254_RLL,  0xFF); 
    PUT_DIO(hdw.one_shot[1],3,  0xFF); 
    PUT_DIO(hdw.one_shot_cw[1], i8254_MODE_1 +
					        i8254_SC2 +
					        i8254_RLL,  0xFF);
  }
  


  PUT_DIO(hdw.Atten_port[0], 0x70, hdw.Atten_mask[0]); /*turn attenuators on */
  PUT_DIO(hdw.TSG_port[0], 0x00, 0xFF); /* turn all control bits off */
  if (stereo !=0) {
    PUT_DIO(hdw.Atten_port[1], 0x70, hdw.Atten_mask[1]); /*turn attenuators on */
    PUT_DIO(hdw.TSG_port[1], 0x00, 0xFF); /* turn all control bits off */
  }
  return 0;
}
