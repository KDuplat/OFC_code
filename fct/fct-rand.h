#define _RAND_IS3 0.577350269   /*1/sqrt(3.)*/


/* RGN Period parameters */  
#define _RAND_N 624
#define _RAND_M 397
#define _RAND_MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define _RAND_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define _RAND_LOWER_MASK 0x7fffffffUL /* least significant r bits */


void init_genrand(unsigned long s) ;
unsigned long genrand_int32(void) ;

double rand01 (void) ;
double rand01i(void) ;
double rand11i(void) ;
int    randin (int n);
int    randim (int n);
int    randsign(void);
int    randPoisson(double lambdao);
double randG  (void) ;

void   randvectoronsphere(double r, double u[3]) ;
void   randvectorinsphere(double r, double u[3]) ;
double randbondvec(double v[3], double lmax2,double l);
void   randmatrixrotation(double phi, double u[3][3]);
