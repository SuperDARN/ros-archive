/* DIO.c
   ===== */

/*
 $Log: DIO.c,v $
 Revision 1.9  1999/04/02 15:49:34  barnes
 Modifications to support Frequency switching.

 Revision 1.8  1998/11/07 00:17:57  barnes
 Extensive modifications for stereo operations.
 Removed all hard coded information about how the DIO cards are set up
 and replaced it with a configuration file.
 Added the ability to set the DIO card addresses on the command line.

 Revision 1.7  1998/10/23 17:39:58  barnes
 Modifications to support the extended timing sequence.

 Revision 1.6  1998/09/11 17:33:25  root
 Modifications to remove short integer types.

 Revision 1.5  1998/06/03 19:10:24  root
 Modification to the version control system.

 Revision 1.4  1997/06/10 20:17:13  root
 Moved sd_radar_id.h header.

 Revision 1.3  1997/04/16 10:59:46  root
 Debug mode can now be set on the command line.

 Revision 1.2  1996/03/14 10:10:02  root
 Added debug code.

 * Revision 1.1  1996/03/13  15:13:31  root
 * Initial revision
 *
*/

/* 
 * void PUT_DIO(int dio_port,unsigned char dio_value,unsigned char dio_mask)
 * unsigned char GET_DIO(int dio_port)
 *
 * These two functions get and put bytes to the DIO card.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <i86.h>

extern int debug;

int dio_adr[2]={0x510,0};
static unsigned char port_cont[24];


/* the internal port number decodes like this 
 *
 *   card|clock|connector|port
 *    1     1     1        2 
 *
 */

int DIO_adr(int dio_port) {
  int card;
  int port;
  card=(dio_port & 0x10)>>4;
  port=(dio_port & 0x0f);
  return dio_adr[card]+port;
}

int DIO_state(int dio_port) {
  return port_cont[dio_port];
}

int DIO_value(int dio_port,unsigned char dio_value,
		      unsigned char dio_mask) {
  return (port_cont[dio_port] & ~dio_mask)
		| (dio_value & dio_mask);
}
  
void PUT_DIO(int dio_port,unsigned char dio_value,
			  unsigned char dio_mask) {
  int card;
  int port;
  card=(dio_port & 0x10)>>4;
  port=(dio_port & 0x0f);
  port_cont[dio_port] = (port_cont[dio_port] & ~dio_mask)
		| (dio_value & dio_mask);

  if (debug !=0) return;
  if (dio_adr[card]==0) return;
  outp(dio_adr[card]+port,port_cont[dio_port]);
  return;
}

unsigned char GET_DIO(int dio_port) {
  unsigned char i;
  int card,port;
  card=(dio_port & 0x10)>>4;
  port=(dio_port & 0x0f);
  if (debug !=0) return 0;
  if (dio_adr[card]==0) return 0;
  i=inp(dio_adr[card]+port);
  return i;
}
