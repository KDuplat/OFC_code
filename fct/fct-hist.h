#define  PUT(u,v,h,a,n,min,istep)        a=(int)floor((u-min)*istep);if(0<=a&&a<n){h->tab[a]+=v;h->in++;}else{h->out++;}
#define SPUT(u,v,h,a,n,min,istep,i1,i2)  if(i1==i2){PUT(u,v,h,a,n,min,istep);}

#define  BPUT(i,no,u,v,h,a,n,min,istep)        for(i=0;i<no;i++){PUT(u,v,h,a,n,min,istep);} 
#define BSPUT(i,no,u,v,h,a,n,min,istep,i1,i2)  for(i=0;i<no;i++){SPUT(u,v,h,a,n,min,istep,i1,i2);} 

#define HIST_DECL int i,n,a;double min,istep;min=ho->min;istep=ho->istep;n=ho->n

typedef struct{
  double *tab;    
  int n; double min;  double max; 
  double step;  double istep;
  long int in;  long int out; 
  double moy;
}hists ;


inline void hist_init   (hists *ho, int hno, double mino, double maxo);
       void hist_initdat(hists *ho, int hno, double *colo, int no, int modo);
inline void hist_free(hists *ho);
void hist_inith(hists *ho, hists *hao);
void hist_copy (hists *hao, hists *ho);

void hist_pr(hists *ho);


inline void hist_add  (hists *ho, double uo                             );
inline void hist_addw (hists *ho, double uo, double wo                  );
inline void hist_adds (hists *ho, double uo,            int a1o, int a2o);
inline void hist_addws(hists *ho, double uo, double wo, int a1o, int a2o);

void hist_adddat  (hists *ho,double *colo,int no                          );
void hist_addwdat (hists *ho,double *colo,int no,double *wo               );
void hist_addsdat (hists *ho,double *colo,int no,           int *so,int io);
void hist_addwsdat(hists *ho,double *colo,int no,double *wo,int *so,int io);


void hist_inith(hists *ho, hists *hao);
void hist_cp (hists *hao, hists *ho);
void hist_op(hists *ho, hists *hoa, char co, hists *hob);

void hist_nor(hists *hso, char modo, double moyo, hists *ho);


void hist_x(hists *ho, double **xo);

void hist_save(hists *ho, char *namo, char *como );
void hist_load( hists *ho, char *namo, int ncolo);

void hist_savemspecial(int ntabo, hists **tabo,char *namo, char *como);
