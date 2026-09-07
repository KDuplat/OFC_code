#include<math.h>
#include<signal.h>
#include<stdio.h>  
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<time.h> 
#include<unistd.h>
#include <fftw3.h>
#include <zlib.h>
#include <sys/stat.h>


/* Macros */

#ifdef MAIN
#  define EXT
#  else
#  define EXT extern
#endif

#define Bl(u)  for(l=0;l<W.lcur;l++){ u ;} ;

#define Bn(u)  for(n=0;n<Par.N;n++){ u ;} ; /* Sweep the entire system */

#define Bbox(u) for(x2=Box.x2i;x2<=Box.x2a;x2++){for(x1=Box.x1i;x1<=Box.x1a;x1++){ u ;}} ; /* Sweep the box */

#define In(x1,x2)  0<=x1 && x1<Par.L1 && 0<=x2 && x2<Par.L2 /* Check if in the limits of the system */

#define InBox(x1,x2)  Box.x1i<=x1 && x1<=Box.x1a && Box.x2i<=x2 && x2<=Box.x2a /* Check if in the box */

#define No(x1,x2) (x1+ Par.L1*(x2)) /* Coordinates to number */

/* Structures*/
typedef struct{ 
    /* 
      zmmode : Threshold
      zmode  : Energy initialisation
      bcmode : Box boundary condition
      timedelta : Time step to save the time 
      savedelta : Time step to save the system
      shapetime : Number of shape taken
    */

    /* System construction parameters */
    int L1,L2,N,randseed,zmmode, zmode, initmode, bcmode;  

    u_int64_t navmax;
    double nu,redis,zmpar;
    int noise_switch, noise_mode, nb_noise; 
    

    /* Analysis parameters */
    int switchPDF, nbdata;
    u_int64_t timedelta, savedelta;
    int shapetime;
    char name[_NL], rac[_NL], path[_NL],pathrac[_NL], path_code[_NL]; 

  
} parst ; /* Parameters */

typedef struct{
    /*
      iHeap : Position in the heap
      mod : switch to know if modified in the ava
    */
    int top, iHeap, mod;
    long double zF, z,zm;

} sitest ; /* Site */

typedef struct{ int x1i,x1a,x2i,x2a;} boxst ; /* Box */

typedef struct{ long double value; int iSite;} heapst; /* Heap */

typedef struct{ clock_t start, end, start1, end1; double avgt, maxt; FILE *timefile;} timest; /* Time */

typedef struct{ int p; u_int64_t nsnap, snapshot[4];} snapst; /* Snapshots */


typedef struct{
    /* 
        plf, plb : Fourier transform
        itime : Initial time to make TF
        dtime : Time step to make TF
        nava : Number of avalanche taken to make the TF
        nl : Number of line to make the space average
        nbl : Number of line where the TF is done
        m : Counter of avalanche taken to make the TF
        ndata : Number of TF done
        temp_save_switch: switch to save temporary correlation function
        dt_save: Interval of time between two save
    */
    fftw_complex *tabi, *tabq, *tabr, **tabcorr;
    fftw_plan plf, plb;
    int64_t itime, dtime, nava, davg, endava, m; 
    int nl, ndata, nbl, sw; 
    double *var; double *avg; double **moycorr;
    FILE *ftfile;
    FILE *ftlenfile;
    FILE *ftcov;
} ftst; /* Fourier transform */

typedef struct{
    double border, max_site, max_sweep, sweep , tot, ei, ef, min, std;
    int cborder;
} enst; /* Energy */

typedef struct{
    int sw, nbins, nbins_fractal;
    double vmin, vmax, dz_2;
    u_int64_t savestat, savetrans, limtrans; 
    gzFile transfile, transfiletop, transfiletprop, transfileitime;
    u_int64_t *histos; /*size histogram*/
    u_int64_t *histot; /*toppling histogram*/
    u_int64_t *histotp; /*propagation time histogram*/
    u_int64_t *histoit; /*inter time histogram*/
    u_int64_t *histoit2; /*inter time histogram for avalanches above size 1*/
    u_int64_t *histo_fractal; /*Fractal size hitogram*/

    double *bins; /*bins for transiant time*/
    double *bins_fractal; /* bins for fractal dimension*/

    int switch_pos_histo;
    u_int64_t **pos_histo;
    u_int64_t **cm_histo;
} compst;   /* Histrogram analysis */

typedef struct{
    FILE *Shapefile10, *Shapefile100, *Shapefile1000;
    int c10, c100, c1000, sw;
} shapest; /* Avalanche shape */

typedef struct{
    sitest *init_Site;
    heapst *init_Heap;
    double init_zF, init_StackzF;
    double *dev;
    int noise_switch, noise_mode, nb_noise, nloop, cloop, nbava; 
} noisest;  /* Add noise */

typedef struct{
    int sw, sw_boxlist;
    gzFile boxfile;

} fracst; /* Fractal structure */

typedef struct{
  int sw, id, nb_ava, maxnb;
  int *prop;
  FILE *file;
} topst; /* Avalanche propagation */


typedef struct{
    sitest *Site_tempo; /*To save the state of the sys*/
    int sw; /* Switch to do prediction */
    int size_th[2]; /* size lim for the classes */
    int nb_snap; /* Number of snap per classes */

    sitest  *Site_s1; /*To save the state of the sys before an ava of S = 1*/
    int N_ava_s1; /* Time since s1*/
    int Nin_s1; /*Save the epicenter*/
    long double zF_s1;
    long double stack_zF_s1;
    int sw_s1;
    int s1;
    u_int64_t Nav_s1; /* Number of the avalanche*/

    sitest *Site_s2;
    int N_ava_s2; 
    int Nin_s2; 
    long double zF_s2;
    long double stack_zF_s2;
    int sw_s2; 
    int s2;  /* Size of the avalanche */
    u_int64_t Nav_s2; 

    int Nava_last_large; /*Time since last large ava*/
    int mean_t; /* average time since last large ava */

    int c1, c2, c3;

    FILE *files1, *files2, *files3;

}predst; /* Prediction */

typedef struct{
    sitest *Site;
    sitest *Site2;
    boxst    Box;
    long double zF;
    int S;
    u_int64_t Nav;
    FILE  *file_histo;
    gzFile file1, file2, file3, file4;

    int *id_ava;
    int sw;
    int count;
    int nb_ava;
    double delta;


    double *bins;
    int nbins;
    double sum_dz;
    int nbtry;

    double *dz_class;
    double dz_delta;
    int *count_class;
    int nbclass;
    FILE **class_file;

    int nbmod;
    int *modifsys;
    int ttop;
    int sw_histo;
    int n_cm;
    
}randomst;  /* Largest avalanche */

typedef struct{

  
    int nbins_avasize;
    int nbins_area;
    u_int64_t navsave;
    double delta;
    int sw;

    int len_arr;
    int c_snap;
    double sum_dz;
    int *sizes;
    int *id;
    u_int64_t **histos;
    double *bins_avasize;
    double *bins_area;
    
}patchst;   /* Patch analysis */


/* Global variables*/

EXT parst    Par;   /* Parameters */
EXT sitest  *Site;  /* System */
EXT sitest  *Site2; /* Parallel system */
EXT boxst    Box;   /* Sweep box */
EXT timest   Time;  /* Simulation time */
EXT snapst   Snap;  /* Snapshots */
EXT heapst *Heap;   /* Minheap*/
EXT ftst Ft;    /* Correlation */
EXT enst Energy;    /* Energy quantitied */
EXT compst Comp;    /* Compact data */
EXT shapest Shape;  /* Avalanche shape */
EXT noisest Noise;  /* Noise in configurations */
EXT fracst Fractal; /* Fractality */
EXT topst Top;  /* Avalanche propagation */
EXT predst Prediction;  /* Prediction */
EXT randomst Random;    /* Largest avalanche */
EXT patchst Patch;  /* Patch analysis */



EXT u_int64_t    Nav;
EXT int64_t Llim;
EXT int64_t Ulim;
EXT long double   zF,zFinc ; 
EXT long double StackzF;
EXT int *Modifsys;
EXT int Hcount;
EXT int Nbmod; /*couter of toppled site*/
EXT int S;
EXT FILE *F1;
EXT gzFile F2;
/* EXT FILE *InitfileB; */




/* Functions specific to aggfct */

void parameters(int argco,  char* nameo);
void memoryallocation(void) ; 
void initialize(void); 


void coorno(int *x1, int *x2, int n); 
double newzm(int n); 
long double newz(double zm);
int bcx(int x); 
void boxdefine(boxst *box, int nin, int x1p, int x1n, int x2p, int x2n);
void boxprint(void); 

void oneavalancheB(void);  
void oneavalancheEn(void);  

void printsitemode(int mode);
void printsite(void);

void add_noise(void);
inline void Pcond(int x2, int *x2_s);



