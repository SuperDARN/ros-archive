/* nrutil.h
   ======== */

double *dvector(long nl, long nh);
double **dmatrix(long nrl, long nrh, long ncl, long nch);
void free_dvector(double *v, long nl, long nh);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
