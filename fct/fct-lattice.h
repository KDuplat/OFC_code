
/* */

#define LATSC   0
#define LATBCC  1
#define LATFCC  2

#define Bpt(i,n,pt,lato,A)    for(i=0;i<n;i++){pt=&((lato->pt)[i]);A;} 
#define Bptval(i,n,pt,lato,A) Bpt(i,n,pt,lato,if(pt->ind==1){A;})

#define Bbd(i,n,pt,lato,p,in,A)     Bpt(i,n,pt,lato,for(p=0;p<lato->nnei;p++){in=pt->nei[p];A;};)
#define Bbdval(i,n,pt,lato,p,in,A)  Bbd(i,n,pt,lato,p,in,if(lattice_valbond(lato->pt,i,in)==1){A;};)


inline int ifl (int ao, int bo);
inline int iflr(int ao, int bo, int *ro);

inline int lat_bcvec(int ao, int no);
void lat_vec(ivs *vo, ivs vao, ivs vbo, ivs vno);
inline int lat_dis2(ivs vao, ivs vbo, ivs vno);

inline int  latsc_nc (ivs vo, ivs vno);
inline int  latfcc_nc(ivs vo, ivs vno);

inline void latsc_cn (ivs *vo, int no, ivs vno);
inline void latfcc_cn(ivs *vo, int no, ivs vno);


inline int  latsc_nc_c (int ao, int bo, int co, int nao, int nbo, int nco);
inline int  latfcc_nc_c(int ao, int bo, int co, int nao, int nbo, int nco);

inline void latsc_cn_c (int *ao, int *bo, int *co, int no, int nao, int nbo, int nco);
inline void latfcc_cn_c(int *ao, int *bo, int *co, int no, int nao, int nbo, int nco);


inline int  lat_nc(int to,ivs  vo, ivs vno);
inline void lat_cn(int to,ivs *vo, int no, ivs vno);



void  lattice_parameters(lats *lato);
void  lattice_fill(lats *lato);

void  lattice_pointshow(lats *lato,int modo);

void  lattice(lats *lato,char *typo, ivs dimo);
void  lattice_free(lats *lato);


void lattice_vr(lats *lato, vs vdeco, vs ao);

void lattice_reduce(lats *lato);

void lattice_reduceessai(lats *lato);

int  lattice_valbond(latpts *pto, int ao, int bo);
void lattice_bond(lats *lato);
void lattice_bondshow(lats *lato);

void lattice_field(lats *lato, char *fo, void *vo);

void lattice_lmpdat(lats* lato, lmpdats *lmpdato);
