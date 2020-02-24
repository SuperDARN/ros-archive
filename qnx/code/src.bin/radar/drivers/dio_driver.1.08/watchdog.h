/* watchdog.h
   ==========
   Author: R.J.Barnes
*/

#ifndef WATCHDOG_H

/* The port address is set by SW1-1 and SW-2:
 *							 OFF	   OFF		0x219  (default)
 * 							 ON		   OFF		0x299
 * 							 OFF	   ON		0x319
 *							 ON		   ON		0x399
 */

void kick_watchdog(int watchdog_port,int time);
void disable_watchdog(int watchdog_port);

#endif
#define WATCHDOG_H
