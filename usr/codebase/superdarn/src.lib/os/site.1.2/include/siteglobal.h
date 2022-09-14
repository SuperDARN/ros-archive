/* siteglobal.h
   ============ 
   Author: R.J.Barnes
*/
   

/*
 $License$
*/

#ifndef _SITEGLOBAL_H
#define _SITEGLOBAL_H

extern int sock;
extern char server[256];
extern int port;
extern int num_transmitters;
extern struct timeval tock;
extern struct ControlPRM rprm;
extern struct ControlData rdata;
extern struct BadTR badtrdat;
extern struct TXStatus txstatus;
extern struct SiteLibrary sitelib;

#endif
