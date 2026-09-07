#include<math.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<time.h> 
#include<unistd.h>

#include"fonction.h"  
#include"fonctionL.h"   



/* general */

typedef struct{
  int n1,n2;
} nbis ;


typedef struct listmp{
  int    n             ;
  struct listmp *next  ;
} lis ;


typedef struct {
  int    n  ;
  double r2 ;
  double w  ;  
} lsics ;


typedef struct lisstmp{
  lsics     ic          ;
  struct lisstmp *next  ;
} lss ;


void splitword(char *word, char *sep, char ***res) ;
void splitwordfree(char **res);

/* RGN */
void init_genrand(unsigned long s) ;
unsigned long genrand_int32(void) ;

double rand01(void)  ;
double rand01i(void) ;
double rand11i(void)  ;
int    randin(int n) ;
double randG(void)   ;
void randvectoronsphere(double r, double u[3]) ;
void randvectorinsphere(double r, double u[3]) ;
double randbondvec(double v[3], double lmax2,double l);
void randmatrixrotation(double phi, double u[3][3]);



/* matrix 3x3 small functions*/
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


/* matrix 2x2 small functions*/
void fm_m2(double a[2][2], double a00, double a10, double a01, double a11);
void display_m2(double a[2][2]);
void pms_m2(double a[2][2], double x);
void cp_m2(double a[2][2], double b[2][2]);
void diag_m2(double a[2][2], double b[2][2]);
double anglevp_m2(double a[2][2],double lam);


/* liste d'entiers  */
void add_li(lis **list, int n) ;
void del_li(lis **list, int n) ;
void empty_li(lis **list) ;
void display_li(lis* liste) ;
int cardinal_li(lis* liste) ;
void file_li(lis* liste, char *buf) ;
int in_li(int m, lis* liste) ;


/* liste de structure  */
void add_ls(lss **list, lsics ic) ;
void del_ls(lss **list, lsics ic) ;
void empty_ls(lss **list) ;
void display_ls(lss* liste) ;
int cardinal_ls(lss* liste) ;
void file_ls(lss* liste, char *buf) ;
int in_ls(lsics ic, lss* liste) ;


/*nbi structure (two ints) */
void  swapnbi(nbis *tab, int a, int b);
int siftnbi(nbis *tab, int start, int count);
void heapsortnbi(nbis *tab, int count); 


/* tres divers */
void removestarcolword(char **colw) ;
int Nfromword(char *word) ;
void writestarword_m(FILE *f, double a[3][3]) ;
void copysmallfile(char *filea, char *fileb);
char *zstrncat( char *dst, const char *src, size_t len );
void writebuffer(char *buf, int *lcur, int lmax, FILE* f, char *string) ;


/* History  */
/* removed because unsafe */
/* #define  rand01  ( (double)rand()/(RAND_MAX+1.0) ) */
/* #define  rand11  ( -1.+2*rand01 )                  */


