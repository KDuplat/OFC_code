
#include<sys/types.h>

#define Bbox_rd(u) for(x2=Random.Box.x2i;x2<=Random.Box.x2a;x2++){for(x1=Random.Box.x1i;x1<=Random.Box.x1a;x1++){ u ;}} ; /* Sweep the box */

void periodic_cm_rd(int *x1_cm,int *x2_cm);

void random_toppling(int nin, long double fct_zF, u_int64_t fct_Nav, sitest *Sys, int s);
/* void random_toppling_nostop(int nin, long double fct_zF, long double fct_stack_zF, u_int64_t fct_Nav, sitest *Sys, int s, gzFile file, FILE **f_histo);*/
void random_toppling_nostop(int nin, long double fct_zF, long double fct_stack_zF, u_int64_t fct_Nav, sitest *Sys, int s, gzFile file, int *sc_out, double *smean_out);

void random_toppling_in_patch(long double fct_zF, sitest *Sys);

void patch_analysis(double dz, int nin);

int intComparator ( const void * first, const void * second );