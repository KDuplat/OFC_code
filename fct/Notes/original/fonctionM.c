#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<time.h>
#include<unistd.h>
#include<limits.h>

#include"fonction.h"
#include"fonctionL.h"
#include"fonctionM.h"


#define IS3 0.577350269   /*1/sqrt(3.)*/


/* RGN Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */


/* general */
void splitword(char *word, char *sep, char ***res){
  int i,n,lmax; char *w,*tmp;

  lmax = 1000;     tmp = (char*) calloc(lmax+1, sizeof(char));
  
  if(lmax<strlen(word)){ fprintf(stderr,"stderr: function splitword %d<strlen(word) word  too long \n",lmax) ; exit(0) ; }
  else{
    
    sprintf(tmp,"%s",word); w = strtok(tmp,sep); n=0 ; while (w != NULL){ n++; w=strtok(NULL, sep) ; } ;
    
    sprintf(tmp,"%s",word) ;
    *res = (char**) calloc( n+1, sizeof(char*) ) ; 
    (*res)[0]= (char*) calloc(17, sizeof(char) ) ; sprintf((*res)[0],"%d", n) ;
    
    w = strtok(tmp,sep);
    i= 0 ;
    while (w != NULL){
      i++;
      (*res)[i]= (char*) calloc(strlen(w)+1, sizeof(char) );  sprintf((*res)[i],"%s",w) ;
      w = strtok (NULL, sep) ;
    }
  }
  
  free(tmp);
}

void splitwordfree(char **res){
  int i,a ;
  a=atoi(res[0]);  for (i=0; i<=a; i++){ free(res[i]);}; free(res);
}




/* Random number generator  */


void init_genrand(unsigned long s){
  mt[0]= s & 0xffffffffUL;
  for (mti=1; mti<N; mti++) {
    mt[mti] = (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
    mt[mti] &= 0xffffffffUL;
  }
}


/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void){
  unsigned long y;
  static unsigned long mag01[2]={0x0UL, MATRIX_A}; /* mag01[x] = x * MATRIX_A  for x=0,1 */

  if (mti >= N) { /* generate N words at one time */
    int kk;
	
    if (mti == N+1)   /* if init_genrand() has not been called, */
      init_genrand(5489UL); /* a default initial seed is used */

    for (kk=0;kk<N-M;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (;kk<N-1;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];
    
    mti = 0;
  }

  y = mt[mti++];

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);
  
  return y;
}


/* generates a random number on [0,1)-real-interval */
/* double genrand_real2(void){ I changed the name */
double rand01(void) { return(genrand_int32()*(1.0/4294967296.0)); }
double rand01i(void) { return(genrand_int32()*(1.0/4294967295.0)); }
double rand11i(void) { return(-1.+(2.*genrand_int32()*(1.0/4294967295.0)) ) ; }
int    randin(int n){ return( 1+(int)floor(n*rand01()) ) ;}    /*  in [[1;n]] */

void randvectoronsphere(double r, double u[3]){ /* see Frenkel-Smit, the module is given by r */ 
  double u1, u2, usq, uh ;
  usq=2. ;
  while(1.<=usq ){
    u1  = rand11i() ;
    u2  = rand11i() ;
    usq = u1*u1 + u2*u2 ;
  }
  uh = 2.*sqrt(1.-usq);
  u[0] = r*u1*uh ;
  u[1] = r*u2*uh ;
  u[2] = r*(1.-2.*usq) ;
}

void randvectorinsphere(double r, double u[3]){ /* moi */
  double x,y,z ;
  
  do{
    x=r*rand11i() ;
    y=r*rand11i() ;
    z=r*rand11i() ;
  }while( r*r < x*x+y*y+z*z ) ;
    
  u[0] = x ;
  u[1] = y ;
  u[2] = z ;
}  


void randmatrixrotation(double phi, double u[3][3]){
  int i,j ;
  double v[3], m[3][3];
    
  randvectoronsphere(1., v);
  
  init0_m(u) ; 
  
  init0_m(m) ;  m[0][0]=m[1][1]=m[2][2]=cos(phi);     sm_m(u,m,u);
  
  init0_m(m) ; 
  
  for (i=0 ; i<3 ; i++ ) {  for (j=0 ; j<3 ; j++ ) {  m[i][j] =  (1-cos(phi))*v[i]*v[j] ;}} ;   sm_m(u,m,u);
  
  init0_m(m) ; 
  
  fm_m(m,0,v[2],-v[1],-v[2],0,v[0],v[1],-v[0],0);  pms_m(m,sin(phi));     sm_m(u,m,u); 
}



/* for other functions see  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html */

double randG(void){
  double u1, u2 ;
  static double v1, v2, s;
  static int phase = 0;
  double x;

  if(phase == 0) {
    do {
      u1 = rand01(); 
      u2 = rand01(); 

      v1 = 2 * u1 - 1;
      v2 = 2 * u2 - 1;
      s = v1 * v1 + v2 * v2;
    } while(s >= 1 || s == 0);
    
    x = v1 * sqrt(-2*log(s)/s );
  } else{
    x = v2 * sqrt(-2*log(s)/s );
  }
  
  phase = 1 - phase ;  return(x);
}


double randbondvec(double v[3], double lmax2,double l) { /* average length bond =1, see IS3*/
  double x,y,z,u,nor;
  
  nor= l*IS3;

  do{
    x = nor*randG() ;
    y = nor*randG() ;
    z = nor*randG() ;
    
    u = x*x+y*y+z*z ;
  }while( lmax2<u ) ;
  
  v[0] = x ;
  v[1] = y ;  
  v[2] = z ;
  
  return(u) ;
}


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
  int i,j ;  char** hw ;
  splitword(word,",", &hw) ;   
  for (i=0 ; i<3 ; i++ ) { 
    for (j=0 ; j<3 ; j++ ) { 
      a[i][j] = atof(hw[1+i+3*j]) ;
    }}
  splitwordfree(hw);
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
  x = min_d( a[0][0], min_d(a[1][1], a[2][2]) ) ; return( 0.25*x*x) ;
}


/* liste d'entiers */
void add_li(lis **list, int n){  /*add at the beginning */
  lis *new;
  
  if((new = (lis*)malloc(sizeof(lis))) == NULL) { perror("malloc failed "); exit(1); }
  new->n  =  n  ;
  new->next = *list;
  *list =new ;
}

void del_li(lis **list, int n){ 
  lis *lcur, *lpre, *ltmp;
  
  lpre = NULL;  for(lcur=*list; (lcur!=NULL) && (lcur->n!=n); lpre=lcur,lcur=lcur->next ){ ;} ;
  
  if(lcur != NULL){
    if(lpre == NULL){  ltmp = lcur->next;   free(lcur);  *list=ltmp ; }
    else{              lpre->next = lcur->next;  free(lcur);  }
  }
  else{printf("Pb del_li  je n'ai pas trouve  %d \n", n) ; exit(1);}
}

void empty_li(lis** liste){
  lis *lcur, *ltmp ;
  lcur=*liste ;  *liste=NULL ; while(lcur!=NULL){ ltmp=lcur->next ; free(lcur); lcur=ltmp;} ; 
}

void display_li(lis* liste){
 lis *li ;
 for( li=liste; li!=NULL; li=li->next) { fprintf(stdout, "n %d \n",li->n); fflush(stdout); }
}
int cardinal_li(lis* liste){
  int i ; lis *li ; i=0 ; for( li=liste; li!=NULL; li=li->next) { i++ ; } ; return(i-1) ;
}
void file_li(lis* liste,char *buf){
  FILE *f ; lis *li ;
  f= fopen(buf,"w") ; for( li=liste; li!=NULL; li=li->next) {fprintf(f, "n %d \n",li->n) ;} ; fclose(f) ;
}
int in_li(int m, lis* liste){
  lis *lcur ; 
  for(lcur=liste; (lcur!=NULL) && (lcur->n!=m); lcur=lcur->next ){ ;} ;
  if(lcur == NULL){ return(1) ; } else            { return(0) ; }
}

/* liste de structure  */
void add_ls(lss **list, lsics ic){  /*add at the beginning */
  lss *new;
  
  if((new = (lss*)malloc(sizeof(lss))) == NULL) { perror("malloc failed "); exit(1); }
  new->ic  =  ic  ;
  new->next = *list;
  *list =new ;
}

void del_ls(lss **list, lsics ic){ 
  lss *lcur, *lpre, *ltmp;
  
  lpre = NULL;  for(lcur=*list; (lcur!=NULL) && (lcur->ic.n!=ic.n); lpre=lcur,lcur=lcur->next ){ ;} ;
  
  if(lcur != NULL){
    if(lpre == NULL){  ltmp = lcur->next;   free(lcur);  *list=ltmp ; }
    else{              lpre->next = lcur->next;  free(lcur);  }
  }
  else{printf("Pb del_li  je n'ai pas trouve  %d \n", ic.n) ; exit(1);}
}

void empty_ls(lss** liste){
  lss *lcur, *ltmp ;
  lcur=*liste ;   *liste=NULL ; while(lcur!=NULL){ ltmp=lcur->next ; free(lcur); lcur=ltmp;} ; 
}

void display_ls(lss* liste){
  lss *li ; for( li=liste; li!=NULL; li=li->next) { fprintf(stdout, "ic %d %g  \n",li->ic.n, li->ic.r2/* ,li->ic.w  */  ); fflush(stdout); }
}
int cardinal_ls(lss* liste){
  int i ; lss *li ; i=0 ; for( li=liste; li!=NULL; li=li->next) { i++ ; } ; return(i) ;
}
void file_ls(lss* liste,char *buf){
  FILE *f ; lss *li ;
  f= fopen(buf,"w") ; for( li=liste; li!=NULL; li=li->next) {fprintf(f,"ic %d %g \n",li->ic.n, li->ic.r2/* ,li->ic.w*/);} ; fclose(f) ;
}
int in_ls(lsics ic, lss* liste){
  lss *lcur ; 
  for(lcur=liste; (lcur!=NULL) && (lcur->ic.n!=ic.n); lcur=lcur->next ){ ;} ;
  if(lcur == NULL){ return(1) ; } else             { return(0) ; }
}

/*nbi structure (two ints) */

inline int nbismallerthan(nbis a,  nbis b){
  int an1, bn1;
  an1=a.n1;
  bn1=b.n1;
  
  if (   an1<bn1 || (an1==bn1 && (a.n2<b.n2)) ){return( 1);}
  else                                         {return(-1);}
}


void  swapnbi(nbis *tab, int a, int b){nbis nbi;  nbi=tab[a];  tab[a]=tab[b];  tab[b]=nbi;}

int siftnbi(nbis *tab, int start, int count) {
  int root, child ;
  root=start ;
  while ( (2*root+1)< count) {
    child = 2*root + 1 ;
    if ( (child < count-1) &&  (nbismallerthan(tab[child],tab[child+1])==1) ){ child++ ; }
    if ( nbismallerthan(tab[root],tab[child])==1) { swapnbi(tab, root, child) ;  root = child ;}
    else{ return(0) ;} 
  }
  return(0) ; 
}

void heapsortnbi(nbis *tab, int count) {
  int start, end ;
  start = count/2 -1 ;   end = count-1 ;
  while (0<=start){ siftnbi(tab, start, count) ;                     start-- ;}
  while (0<end)   { swapnbi(tab, end,0)        ; siftnbi(tab,0,end) ; end--   ;}
}



/* tres divers */
void removestarcolword(char **colw){
  int l,i ;   char buf[1000];            /* printf("%d \n", atoi(colw[0]) ); */

  for (l=1 ; l<=atoi(colw[0]) ; l++ ) {   

    if (colw[l][0]=='*'){
      i = 1 ; while(colw[l][i]!='*'){ buf[i-1]=colw[l][i]; i++ ; } ; buf[i-1]='\0' ; sprintf(colw[l],"%s",buf); /*  fprintf(stderr,"ap <%s>\n",buf);   */
    }}
}

int Nfromword(char *word){
  int i, res ;
  char **part, **number ;
  
  splitword(word,"-",&part) ; 
  splitword(part[1],"ABCD",&number) ;

  res=0 ;
  for(i=1 ; i<= (int)atoi(number[0]); i++){ res += atoi(number[i]) ; } ;
  
  return(res) ;
}


void writestarword_m(FILE *f, double a[3][3]) {
  fprintf(f,"*%g,%g,%g,%g,%g,%g,%g,%g,%g*\n", a[0][0],a[1][0],a[2][0], a[0][1],a[1][1],a[2][1], a[0][2],a[1][2],a[2][2]) ;  
}


void copysmallfile(char *filea, char *fileb){ /* when a system call is impossible (condor) */
  int i,nl,nw;  char **tmp, *line; FILE *f ;
  
  f=fopen(filea,"r");

  if ( f == NULL) {   fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", filea ); exit(EXIT_FAILURE) ; }
  else{
    
    nw=5000;  line=(char*) calloc(nw+1, sizeof(char)); 

    nl=0 ; while (fgets(line,nw,f) != NULL){ nl++ ;};
    
    tmp=(char**) calloc(nl+1, sizeof(char*));   for(i=0;i<=nl;i++){ tmp[i]=(char*) calloc(nw+1, sizeof(char));} ; 
    
    rewind(f); i=0 ; while (fgets(tmp[i],nw,f) != NULL){ i++ ; } ; fclose(f);
     
    f=fopen(fileb,"w"); for(i=0 ; i<=nl; i++){ fprintf(f,"%s", tmp[i]);};  fclose(f);
    
    for(i=0; i<=nl; i++){  free(tmp[i]);}; free(tmp);    free(line);
  }
  
}



char *zstrncat( char *dst, const char *src, size_t len ){
	char *dp = dst;
	const char *sp = src;
	int cc;

	if (len == 0) return(dst);

	len--;
	
	/* 	 Locate the end of the current string. */
	cc = 0; while ((*dp)&&(cc < len)) { dp++; cc++; }
	
	/* 	 If we have no more buffer space, then return the destination */
	if (cc >= len) return(dst);

	/* 	 While we have more source, and there's more char space left in the buffer */
	while ((*sp)&&(cc < len)){
	  cc++; *dp = *sp;  dp++; sp++;
	}
	/*  Terminate dst, as a gaurantee of string ending. */  
	*dp = '\0';

	return(dst);
}



void writebuffer(char *buf, int *lcur, int lmax, FILE* f, char *string){
  int l,ls ;
  
   
  ls = strlen(string) ;    l=*lcur+ls+10 ; /*"securite"*/
  
  if(lmax<=l){ 
    fprintf(f,"%s%s",buf,string) ;  fflush(f);    buf[0]='\0';  
  }else{
    /* strncat(buf,string,lmax- (*lcur) -1);  */ 
    zstrncat(buf, string, lmax ); 
  }
  *lcur=strlen(buf);  
}






