/* resets.c 
   ======== */

/* 
 $Log: reset.c,v $
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

#include "id.h"
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
