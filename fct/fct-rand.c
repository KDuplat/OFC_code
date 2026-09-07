#include"fct.h"

/*____________valid but not updated nor checked______________________*/


/* here, rather than in fct-rand.h to avoid warning */
static unsigned long _RAND_mt[_RAND_N]; /* the array for the state vector  */
static int _RAND_mti=_RAND_N+1; /* mti==N+1 means mt[N] is not initialized */



void init_genrand(unsigned long s){
  _RAND_mt[0]= s & 0xffffffffUL;
  for (_RAND_mti=1; _RAND_mti<_RAND_N; _RAND_mti++) {
    _RAND_mt[_RAND_mti] = (1812433253UL * (_RAND_mt[_RAND_mti-1] ^ (_RAND_mt[_RAND_mti-1] >> 30)) + _RAND_mti); 
    _RAND_mt[_RAND_mti] &= 0xffffffffUL;
  }
}


/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void){
  unsigned long y;
  static unsigned long mag01[2]={0x0UL, _RAND_MATRIX_A}; /* mag01[x] = x * _RAND_MATRIX_A  for x=0,1 */

  if (_RAND_mti >= _RAND_N) { /* generate N words at one time */
    int kk;
	
    if (_RAND_mti == _RAND_N+1)   /* if init_genrand() has not been called, */
      init_genrand(5489UL); /* a default initial seed is used */

    for (kk=0;kk<_RAND_N-_RAND_M;kk++) {
      y = (_RAND_mt[kk]&_RAND_UPPER_MASK)|(_RAND_mt[kk+1]&_RAND_LOWER_MASK);
      _RAND_mt[kk] = _RAND_mt[kk+_RAND_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    for (;kk<_RAND_N-1;kk++) {
      y = (_RAND_mt[kk]&_RAND_UPPER_MASK)|(_RAND_mt[kk+1]&_RAND_LOWER_MASK);
      _RAND_mt[kk] = _RAND_mt[kk+(_RAND_M-_RAND_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
    }
    y = (_RAND_mt[_RAND_N-1]&_RAND_UPPER_MASK)|(_RAND_mt[0]&_RAND_LOWER_MASK);
    _RAND_mt[_RAND_N-1] = _RAND_mt[_RAND_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];
    
    _RAND_mti = 0;
  }

  y = _RAND_mt[_RAND_mti++];

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
int    randin(int n){ return( 1+(int)floor(n*rand01()) ) ;}    /*  in [[1;n]]   */
int    randim(int n){ return(   (int)floor(n*rand01()) ) ;}    /*  in [[0;n-1]] */
int    randsign(void){if(rand01()<0.5){return(-1);}else{return(1);} ;}    /*  in [[0;n-1]] */

/* random variables */
int randPoisson(double lambda) {
  int res = 0; double x,lim;  
  lim = exp(-lambda); x = rand01(); 
  while (x > lim) { res++; x *= rand01(); }; return(res); }
/* from internet checked on 2 example (lam=3,5) good */


/**/

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


double randbondvec(double v[3], double lmax2,double l) { /* average length bond =1, see _RAND_IS3*/
  double x,y,z,u,nor;
  
  nor= l*_RAND_IS3;

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
