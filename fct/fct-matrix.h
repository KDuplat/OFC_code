void fm_m(double a[3][3], double a00, double a10, double a20, double a01, double a11, double a21, double a02, double a12, double a22 ) ;
void display_m(double a[3][3]) ;
double det_m(double a[3][3]) ;
double trace_m(double a[3][3]) ;
double tracexy_m(double a[3][3]) ;
void trans_m(double a[3][3], double b[3][3]) ;
void inv_m(double a[3][3], double b[3][3] ) ;
void sm_m(double a[3][3], double b[3][3], double c[3][3]) ;
void pm_m(double a[3][3], double b[3][3], double  c[3][3]) ;
void pm3_m(double a1[3][3], double a2[3][3], double a3[3][3], double b[3][3]) ;
double modvc_m(int k, double a[3][3]) ;
void pmv_m(double a[3][3], double x, double y, double z, double *xr, double *yr, double *zr) ;
void pmv_v(double a[3][3], double u[3], double res[3]) ;

void cp_m(double a[3][3], double b[3][3]) ;
void pms_m(double a[3][3], double x) ;
void init0_m(double a[3][3]);
void init1_m(double a[3][3]);
void init0_im(int a[3][3]) ;
void traceless_m(double a[3][3],  double b[3][3]) ;
void tracelessxy_m(double a[3][3],  double b[3][3]) ;

void convertword_m(char *word, double a[3][3]) ;
double  dis2maxH_m(double a[3][3]) ;
void ftnirpf_m(FILE *fo , double ma[3][3]);
void ftnirps_m(char *buf, double ma[3][3]);


void fm_m2(double a[2][2], double a00, double a10, double a01, double a11);
void display_m2(double a[2][2]);
void pms_m2(double a[2][2], double x);
void cp_m2(double a[2][2], double b[2][2]);
void diag_m2(double a[2][2], double b[2][2]);
double anglevp_m2(double a[2][2],double lam);
