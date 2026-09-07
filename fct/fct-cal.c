#include"fct.h"


/* avoir void dtaballoc(double ***tabo, int nlo, int nro){ */
/*   int i; tabo  = (double***) calloc( nlo, sizeof(double**) ); for (i=0; i<nlo;i++){ tabo[i]=(double**)calloc(nro,sizeof(double*)) ;} */
/* } */


inline int imax(int a, int b){ if (b<a){ return(a);}else{ return(b);}  }
inline int imin(int a, int b){ if (b<a){ return(b);}else{ return(a);}  }

inline double dmax(double a, double b){ if (b<a){ return(a);}else{ return(b);}  }
inline double dmin(double a, double b){ if (b<a){ return(b);}else{ return(a);}  }


/* basic */

inline void icolminmax(int* colo, int no, int *mino, int *maxo){
  int i; double u,min,max;
  min =  INT_MAX; 
  max = -INT_MAX; 
  for(i=0; i<no; i++){  u = colo[i];  if(u<min){min=u;};  if(max<u){max=u;};};   
  *mino=min;
  *maxo=max;
}

inline void dcolminmax(double* colo, int no, double *mino, double *maxo){
  int i; double u,min,max;
  min =  1e20; 
  max = -1e20; 
  for(i=0; i<no; i++){  u = colo[i];  if(u<min){min=u;};  if(max<u){max=u;};};   
  *mino=min;
  *maxo=max;
}


inline void dcolmoy(double* colo, int no, double *moyo){
  int i; double moy ;
  moy=0;   for(i=0; i<no; i++){ moy += colo[i];}; moy /=no; *moyo=moy;
}

inline void dcolmoyvar(double* colo, int no, double *moyo, double *varo){
  int i; double moy,var,u;
  moy=0;   for(i=0; i<no; i++){                moy += colo[i];}; moy/=no; *moyo=moy;
  var=0;   for(i=0; i<no; i++){ u=colo[i]-moy; var +=u*u;     }; var/=no; *varo=var;    /* printf("%g %g    %d\n",moy,var,no); */
}


/* derived */
inline int    icolmin(int*    colo, int no){ int    u,v;  icolminmax(colo,no,&u,&v); return(u);}
inline int    icolmax(int*    colo, int no){ int    u,v;  icolminmax(colo,no,&u,&v); return(v);}
inline double dcolmin(double* colo, int no){ double u,v;  dcolminmax(colo,no,&u,&v); return(u);}
inline double dcolmax(double* colo, int no){ double u,v;  dcolminmax(colo,no,&u,&v); return(v);}


/*  */

inline void dcolzero(double *ao, double nao){ int i; for(i=0; i<nao; i++){ ao[i]=0.;};}


inline void icolcopy(int    *ao, int    nao, int    *bo){ int i; for(i=0; i<nao; i++){ bo[i]=ao[i];};}
inline void dcolcopy(double *ao, double nao, double *bo){ int i; for(i=0; i<nao; i++){ bo[i]=ao[i];};}

inline void icoldupl(int *ao, int nao, int **bo, int *nbo){ 
  *bo=(int*)calloc(nao,sizeof(int)); icolcopy(ao,nao,*bo); *nbo=nao;
}

inline void icoljoint(int *ao, int nao, int *bo, int nbo, int **reso, int *nreso){
  *nreso=nao+nbo; *reso=(int*)calloc(*nreso,sizeof(int));   icolcopy(ao,nao,*reso      );
  /**/                                                      icolcopy(bo,nbo,*reso + nao);
} 

inline void icoldiscard(int disco, int **ao, int *nao){
  int i,a,m,n;  n=*nao; a=0;
  for(i=0; i<n; i++){  m=(*ao)[i]; if(m!=disco){ (*ao)[a]=m; a++;};}; *nao=a;
}


void icoluniq(int **reso, int *nreso){
  int i,a,m,n; n=*nreso;
  
  if(2<=n){
    tqsort('i',*reso,n);   /* icolpr(*reso,nao); */   
    
    a=1; for(i=1; i<n; i++){  m=(*reso)[i]; if(m!=(*reso)[a-1]){ (*reso)[a]=m; a++;};}; *nreso=a;    
  }
}


/*  */


int intcompare   (const void *ao, const void *bo) {  return( *(const int*)ao - *(const int *)bo );}
/* this is only correct if it doesn't overflow */



int doublecompare(const void *ao, const void *bo) {  
  int res; double a,b,u,sum, eps;
  a = *(const double*)ao;
  b = *(const double*)bo;  sum=a+b; eps=1e-10;

  if(0.<fabs(sum)){
    u=(a-b)/sum; 
    if       (u> eps){ res= 1;}
    else{ if (u<-eps){ res=-1;} 
      /**/else       { res= 0;};}
  }
  else{ 
    if(fabs(a)<eps){res=0;}/* case a=-b=0*/ 
    else{
      if(a>b){res= 1;}
      else   {res=-1;}
    }
  }/* printf("%g %g : %d \n",a,b,res); */

  return(res);
}


void tqsort(char co, void *ao, int no){
  switch(co){
  case 'i':   qsort( (int   *)ao, no, sizeof(int   ),    intcompare);  break;
  case 'd':   qsort( (double*)ao, no, sizeof(double), doublecompare);  break;
  default : fprintf(stderr,"#iqsort: type not implemented\n");  exit(EXIT_FAILURE);     
  }   
}


int ibsearch (char co, const void *keyo, const void *baso, size_t no){
  int *iptr; double *dptr; int ind,res; ptrdiff_t diff;
  
  res=-1; diff=-1; ind=0;
  
  switch(co){
  case 'i': iptr = bsearch(keyo,baso,no,sizeof(int   ),   intcompare); if (iptr!=NULL){ diff=iptr-(int   *)baso; ind=1;};  break;
  case 'd': dptr = bsearch(keyo,baso,no,sizeof(double),doublecompare); if (dptr!=NULL){ diff=dptr-(double*)baso; ind=1;};  break;
  default : fprintf(stderr,"#ibsearch: type not implemented\n");  exit(EXIT_FAILURE);     
  }  
  
  if( ind==1){
    if( 0<=diff && diff<INT_MAX){  res = (int)diff; }
    else{ fprintf(stderr,"#ibsearch: pb %ld \n",diff);  exit(EXIT_FAILURE); }
  }
  
  return(res);
}


int compare_getbetween(const void *coa,const void *cob) {
  double na,nb ;
  dgetbetween(*(char**)coa, GBfct_compare_getbetween_s1, GBfct_compare_getbetween_s2, GBfct_compare_getbetween_s0, &na);
  dgetbetween(*(char**)cob, GBfct_compare_getbetween_s1, GBfct_compare_getbetween_s2, GBfct_compare_getbetween_s0, &nb);
  if (na>nb){ return( 1);} 
  else      { return(-1);}
}


void qsort_getbetween(char **listo, int no, char *s1o, char *s2o, char *s0o){
  
  sprintf(GBfct_compare_getbetween_s1,"%s",s1o);
  sprintf(GBfct_compare_getbetween_s2,"%s",s2o);
  sprintf(GBfct_compare_getbetween_s0,"%s",s0o);
  
  qsort( listo, no, sizeof(char*), compare_getbetween);
}

