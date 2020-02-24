/* math.h
   ====== */


void dsvdfit(double x[], double y[], double sig[], int ndata, double a[], int ma,
        double **u, double **v, double w[], double *chisq,
	     void (*funcs)(double, double [], int));

