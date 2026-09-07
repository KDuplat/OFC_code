#include"fct.h"


/* matrix 3x3 small functions*/
void fm_m(double a[3][3], double a00, double a10, double a20, double a01, double a11, double a21, double a02, double a12, double a22 ) {
  a[0][0]=a00 ; a[0][1]=a01 ; a[0][2]=a02 ; 
  a[1][0]=a10 ; a[1][1]=a11 ; a[1][2]=a12 ; 
  a[2][0]=a20 ; a[2][1]=a21 ; a[2][2]=a22 ; 
}

void display_m(double a[3][3]){
  int i, j ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) {fprintf(stderr,"%g ",a[i][j]) ; }
    fprintf(stderr,"\n") ; }
  fprintf(stderr,"\n") ;
}


double det_m(double a[3][3]){
  return(   a[0][0]*a[1][1]*a[2][2] + a[0][1]*a[1][2]*a[2][0] + a[0][2]*a[2][1]*a[1][0]   
	   -a[0][0]*a[1][2]*a[2][1] - a[1][1]*a[0][2]*a[2][0] - a[2][2]*a[0][1]*a[1][0] ) ;
}


double trace_m  (double a[3][3]){ return(a[0][0]+a[1][1]+a[2][2]) ; }

double tracexy_m(double a[3][3]){ return(a[0][0]+a[1][1]) ; }

void cp_m(double a[3][3], double b[3][3]) {
  int i, j ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      b[i][j] =  a[i][j] ;
    }}
}

void trans_m(double a[3][3], double b[3][3]){
  int i, j ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      b[i][j] =  a[j][i] ;
    }}
}

void inv_m(double a[3][3], double b[3][3] ){
  double x ;
  x = det_m(a) ;
  if (1e-10<fabs(x)){
    b[0][0] =  1/x*(a[1][1]*a[2][2]-a[1][2]*a[2][1])  ; 
    b[1][0] =  1/x*(a[1][2]*a[2][0]-a[1][0]*a[2][2])  ; 
    b[2][0] =  1/x*(a[1][0]*a[2][1]-a[1][1]*a[2][0])  ; 
    b[0][1] =  1/x*(a[0][2]*a[2][1]-a[0][1]*a[2][2])  ;
    b[1][1] =  1/x*(a[0][0]*a[2][2]-a[0][2]*a[2][0])  ; 
    b[2][1] =  1/x*(a[0][1]*a[2][0]-a[0][0]*a[2][1])  ; 
    b[0][2] =  1/x*(a[0][1]*a[1][2]-a[1][1]*a[0][2])  ; 
    b[1][2] =  1/x*(a[1][0]*a[0][2]-a[0][0]*a[1][2])  ; 
    b[2][2] =  1/x*(a[0][0]*a[1][1]-a[0][1]*a[1][0])  ;  
  }
  else{
    display_m(a) ;
    fprintf(stderr,"n'est pas inversible 1e-10<fabs(%g)\n", x) ;
  }
}

void sm_m(double a[3][3], double b[3][3], double c[3][3]){
  int i,j  ;
  double ma[3][3] ;
  init0_m(ma) ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) {  
      ma[i][j] =  a[i][j] +  b[i][j] ;
    }}
  cp_m(ma,c);
}


void pm_m(double a[3][3], double b[3][3], double c[3][3]){
  int i,j,k ;
  double ma[3][3] ;
  init0_m(ma) ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      for (k=0 ; k<3 ; k++ ) { 
	ma[i][j] += ( a[i][k] * b[k][j]) ;
      }}}
  cp_m(ma,c);
}

void pm3_m(double a1[3][3], double a2[3][3], double a3[3][3], double b[3][3]){
  double m[3][3] ;
  pm_m(a1,a2,m) ; pm_m(m,a3,b) ; 
}



double modvc_m(int k, double a[3][3]) {  
  /* module du k=(0..2)eme vector-colonne de la matrice a, k=0..2*/
  int i ;
  double x ;
  x=0. ;
  for (i=0 ; i<3 ; i++ ) { 
    x += ( a[i][k] * a[i][k]) ;
  }
  return(sqrt(x)) ;
}

void pmv_m(double a[3][3], double x, double y, double z, double *xr, double *yr, double *zr) {
  double x0, y0, z0 ; 

  x0=x ;
  y0=y ;
  z0=z ;
  *xr = (x0*a[0][0] + y0*a[0][1] + z0*a[0][2]) ;
  *yr = (x0*a[1][0] + y0*a[1][1] + z0*a[1][2]) ;
  *zr = (x0*a[2][0] + y0*a[2][1] + z0*a[2][2]) ; 
}

void pmv_v(double a[3][3], double u[3], double res[3]){
   /* pmv_m(a, u[0],u[1],u[2], &(res[0]), &(res[1]), &(res[2])) ;  */

  res[0] = (u[0]*a[0][0] + u[1]*a[0][1] + u[2]*a[0][2]) ;
  res[1] = (u[0]*a[1][0] + u[1]*a[1][1] + u[2]*a[1][2]) ;
  res[2] = (u[0]*a[2][0] + u[1]*a[2][1] + u[2]*a[2][2]) ; 
}


void pms_m(double a[3][3], double x) {
  int i, j ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      a[i][j] =  x*a[i][j] ;
    }}
}

void init0_m(double a[3][3]){
 int i, j ;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      a[i][j] = 0. ;
    }}
}

void init1_m(double a[3][3]){
  init0_m(a); a[0][0]=a[1][1]=a[2][2]=1;
}

void init0_im(int a[3][3]){
 int i,j;
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      a[i][j] = 0 ;
    }}
}


void traceless_m(double a[3][3],  double b[3][3]){
  int i  ;
  double x, ma[3][3] ;
  cp_m(a,ma) ;
  x = 1./3.*trace_m(a) ;
  for (i=0 ; i<3 ; i++ ) { 
    ma[i][i] =  a[i][i] - x  ; 
  }
  cp_m(ma,b) ;
}

void tracelessxy_m(double a[3][3],  double b[3][3]){
  double x, ma[3][3] ;
  cp_m(a,ma) ;
  x = 0.5*(a[0][0]+a[1][1]) ;
  ma[0][0] =  a[0][0] - x  ; 
  ma[1][1] =  -ma[0][0]  ;  
  cp_m(ma,b) ;
}


void convertword_m(char *word, double a[3][3]){
  int i,j, n ;  char** hw;
  ssplit(word,",", &hw,&n) ;   
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      a[i][j] = atof(hw[1+i+3*j]) ;
    }}
  ssplitfree(hw,n);
}

void ftnirpf_m(FILE *fo, double ma[3][3]){fprintf(fo ,"%g %g %g  %g %g %g ", ma[0][0],ma[1][1],ma[2][2], ma[1][0],ma[2][0],ma[2][1]);}
void ftnirps_m(char *buf,double ma[3][3]){sprintf(buf,"%g %g %g  %g %g %g ", ma[0][0],ma[1][1],ma[2][2], ma[1][0],ma[2][0],ma[2][1]);}

/* 2x2 matrice small functions */
void fm_m2(double a[2][2], double a00, double a10, double a01, double a11) {
  a[0][0]=a00 ; a[0][1]=a01 ; 
  a[1][0]=a10 ; a[1][1]=a11 ; 
}
void display_m2(double a[2][2]) {
  int i, j ;
  for (i=0 ; i<2 ; i++ ) { 
    for (j=0 ; j<2 ; j++ ) { 
      printf("%g ",a[i][j]);
    }
    printf("\n"); 
  }
}
void cp_m2(double a[2][2], double b[2][2]) {
  int i, j ;
  for (i=0 ; i<2 ; i++ ) { 
    for (j=0 ; j<2 ; j++ ) { 
      b[i][j] =  a[i][j] ;
    }}
}
void pms_m2(double a[2][2], double x) {
  int i, j ;
  for (i=0 ; i<2 ; i++ ) { 
    for (j=0 ; j<2 ; j++ ) { 
      a[i][j] =  x*a[i][j] ;
    }}
}
void diag_m2(double a[2][2], double b[2][2]){
  double det,tr,delta, lmin, lmax ;

  cp_m2(a,b) ;
 
  tr = a[0][0]+a[1][1]; det=a[0][0]*a[1][1]-a[0][1]*a[1][0];

  delta=tr*tr-4*det; 

  if(delta<-1e-10){ printf("[diag_m2] Matrice non diagonalisable delta=%g \n",delta) ;}

  if(tr<0.){ lmin=(tr-sqrt(delta))/2. ; lmax=tr-lmin;}
  else     { lmax=(tr+sqrt(delta))/2. ; lmin=tr-lmax;}
  
 /*  printf("[diag_m2] %g   %g %g \n", lmin, lmax); */
  
  fm_m2(b,lmax,0.,0.,lmin);  
}

double anglevp_m2(double a[2][2],double lam){
  double res;
  if( 1.e-20<fabs(a[0][1]) ){ res=atan( (lam-a[0][0])/a[0][1] ); return(res); }
  else{ return(0.5*M_PI); };
}


/* distance maximale dans un boite decrit par la matrice H, AA seulement pour matrice rectangulaire, ie H diagonale et positive */
double  dis2maxH_m(double a[3][3]){
  double x ;
  x = dmin( a[0][0], dmin(a[1][1], a[2][2]) ) ; return( 0.25*x*x) ;
}
