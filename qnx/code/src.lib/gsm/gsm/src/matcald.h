/* matcald.h
   ========= */


int angl2d(double v1, double v2, double *angle);
int angl3d(double v1, double v2, double v3, double *angle1, double *angle2);

double vlngth(double v1, double v2, double v3);
int polcrd(double  v1, double v2, double v3, double *vln, 
           double *angle1, double *angle2);
int unifyv(double  *a,double *b);
double prdin(double *a, double *b);
int prdout(double *a, double *b, double *c);
double det3(double *x);
int inver3(double *x, double *y);
int mtrvct(double *mtrx, double *vectp, double *vecta);
int mtrprd(double *a, double *b, double *c);
int asmunt(double *vx, double *vy, double *vz, double *trmtrx);
int rotmtr(int idaxis, double angl, double *a);
    


