/* magcmp.c
   ======== */

/*
 $Log: magcmp.c,v $
 Revision 1.1  1999/08/10 12:23:06  barnes
 Initial revision

 */

 
#include <math.h>


int magcmp(double date, double frho, double flat, double flon, 
           double *bx, double *by, double *bz, double *b) {

    frho = frho - 6372.;
 
    igrfcall(date, flat, flon, frho, bx, by, bz);
    *b = sqrt(*bx * *bx + *by * *by + *bz * *bz);
    *bx = -*bx;
    *bz = -*bz;
    return 0;
} 

