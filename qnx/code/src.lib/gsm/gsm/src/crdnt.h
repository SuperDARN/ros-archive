/* crdnt.h
   ======= */

int sun(int iyear, int imnth, int iday, int ihour,
	int imin, double rsec);
int kpl(int iy, int m, int id, double u, double f, double al,
		double dt, double *x, double *y, double *a, double *d,
		double *g, double *q);
int sky(double h, double d, double f, double *x, double *y);
double vallt(double xlong);
int idleap(int iyear);
int ttdym(int iyear, int imnth, int *itday);
int daydoy(int iyear, int imnth, int iday, int *idoy);
int doyday(int iyear, int idoy, int *imnth, int *iday);
int sodhms(double sod, int *ihour, int *imin, double *rsec);
int hmssod(int ihour, int imin, double rsec, double *sod);
int soymdh(int iyear, double soy, int *imnth, int *iday, int *ihour,
		int *imin, double *rsec);
int mdhsoy(int iyear, int imnth, int iday, int ihour, int imin,
		double rsec, double *soy);
int dipole(int iyear, int imnth, int iday, int ihour, int imin,
		double rsec);



int setmtr(double *dmmtrx);
int getmtr(double *dmmtrx);
int invtrs();
int cnvrt(double *frmvct, double *tovct);
int geigeo();
int geogei();
int geigse();
int gsegei();
int geism();
int smgei();
int geomag();
int maggeo();
int geosm();
int smgeo();
int geogsm();
int gsmgeo();
int gsexyp(double *posgse, double *xypp);
int xypgse(double *posgse, double *xypp);
int geogse();
int gsegeo();
int gsevdh(double *posgse);
int vdhgse(double *posgse);
int geovdh(double *posgeo);
int vdhgeo(double *posgeo);
int geopen(double *posgeo);
int pengeo(double *posgeo);
int penvdh(double *posgeo);
int gsesm();
int smgse();
int gsmsm();
int smgsm();
int gsegsm();
int gsmgse();
int gsmgsw();
int gsmhng(double *posgsm, double *poshng);
int gsegsw();
int gswgse();
int gsease();
int asegse();
int lmnmod(int jmode);
int gselmn(double *posgse);
int lmngse(double *posgse);
int r_lat_lt(double *posv, double *posa);
   
