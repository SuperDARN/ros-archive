/* hardware.h
   ========== */

#ifndef _HARDWARE_H
#define _HARDWARE_H

#ifndef _RADAR_NAME_H
#include "radar_name.h"
#endif


struct radar_site {
        int st_id;
        int year;
        int yrsec;
	double geo_lat, geo_long, altitude; /* the location of the radar */
	double boresite, beam_sep; /* the beam pattern of the radar */
	double vdir;	/* the definition of velocity direction (+1.0 is the
			   normal case, -1.0 indicates the I & Q inputs to
			   the receiver were reversed). */
	double atten;	/* the attenuation step in dB */
	double tdiff;	/* the time delay in microsec of the signal from
			   the interferometer array and the main array */
	double phidiff;	/* the sign of the phase shift of the interferometer
			   signal.  A +1 indicates the interferometer array
			   is in front of the main array */
	double interfer_pos[3]; /* the x, y, z position of the interferometer
				   array with respect to the main array.
				   the values are given in meters.  x is
				   in the direction parallel to the main
				   array, with +x being to the right, as you
				   look along the boresite.  y is in the 
				   boresite direction, so a + value indicates
				   that the interferometer array is in front
				   of the main array.  z completes the right
				   handed coordinate system and is positive
				   upwards */
	double rec_rise;	/* the rise time of the receiver in microsec */
	};


struct radar_hardware {
  int num;
  struct radar_site *site;
};

struct radar_hardware *load_hardware(FILE *fp);
int read_hardware(FILE *fp,struct radar_hardware *ptr);
struct radar_hardware *load_hdw(char *path,struct radar_id *rid);
struct radar_site *get_site(struct radar_hardware *ptr,
				int yr,int mo,int dy,int hr,int mt,int sc,
				int st_id);


#endif
 















