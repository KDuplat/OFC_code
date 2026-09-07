#include"fct.h"


/*Comments : */
/* Normalization: it is only when I print the histogram that I normalize the results */
/*                the histogram data is itself never normalized                      */ 
/*                except with  his_nor,  which produces an new histogram              */


/* Operation involving one histogram only*/

inline void hist_init(hists *ho, int no, double mino, double maxo){
  
  ho->n     = no  ;   ho-> step = (maxo-mino)/no;    ho->in  = 0 ;
  ho->min   = mino;   ho->istep = no/(maxo-mino);    ho->out = 0 ;  
  ho->max   = maxo;                                  ho->moy = 0.;   
  
  ho->tab = (double*)calloc(no,sizeof(double));  
}


inline void hist_free(hists *ho){ free(ho->tab); }

void hist_inith(hists *ho, hists *hao){   hist_init(ho, hao->n, hao->min, hao->max); }

void hist_copy (hists *hao, hists *ho){ 
  int i,n;
  hist_inith(ho,hao);  n=ho->n; for(i=0; i<n; i++){ ho->tab[i] = hao->tab[i]; }; 
}

void hist_pr(hists *ho){
  int i,n;
  fprintf(stderr,"# %d %g %g \n", ho->n,  ho->min,  ho->max);
  
  n=ho->n; for(i=0; i<n; i++){  fprintf(stderr,"%d %g \n",i, ho->tab[i]); }; 
}


void hist_initdat(hists *ho, int hno, double *colo, int no, int modo){
  double min,max, u,ua,ub;
  
  dcolminmax(colo,no,&min,&max);
  
  ua=ub=-1.; if(modo==1){ ua=min; ub=max; } 
  /**/       if(modo==2){ u=dmax(fabs(min),fabs(max));   ua=-u; ub=u;   /* fprintf(stderr,"#hist_inidat (2): u=%g \n",u);  */ };
  
  hist_init(ho,hno,ua,ub);
}


/* add one element */
inline void hist_add  (hists *ho, double uo                             ){ int a;  PUT(uo, 1,ho,a,ho->n,ho->min,ho->istep        ); }
inline void hist_addw (hists *ho, double uo, double wo                  ){ int a;  PUT(uo,wo,ho,a,ho->n,ho->min,ho->istep        ); }
inline void hist_adds (hists *ho, double uo,            int a1o, int a2o){ int a; SPUT(uo, 1,ho,a,ho->n,ho->min,ho->istep,a1o,a2o); }
inline void hist_addws(hists *ho, double uo, double wo, int a1o, int a2o){ int a; SPUT(uo,wo,ho,a,ho->n,ho->min,ho->istep,a1o,a2o); }


/* add col of elements  */

void hist_adddat  (hists *ho,double *colo,int no                          ){ HIST_DECL; BPUT (i,no,colo[i],    1,ho,a,n,min,istep         );}
void hist_addwdat (hists *ho,double *colo,int no,double *wo               ){ HIST_DECL; BPUT (i,no,colo[i],wo[i],ho,a,n,min,istep         );}
void hist_addsdat (hists *ho,double *colo,int no,           int *so,int io){ HIST_DECL; BSPUT(i,no,colo[i],    1,ho,a,n,min,istep,so[i],io);}
void hist_addwsdat(hists *ho,double *colo,int no,double *wo,int *so,int io){ HIST_DECL; BSPUT(i,no,colo[i],wo[i],ho,a,n,min,istep,so[i],io);}



/* derived function from file */
inline void hist_adddatf(hists *ho, char *namo, int ncolo){
  int n; double *col;  
  dcolread(&col,&n,namo,ncolo);  hist_adddat(ho,col, n); free(col);
}

inline void hist_addwdatf(hists *ho, char *namo, int ncolo, int ncolow){
  int n; double *col, *colw;  
  dcolread(&col ,&n,namo,ncolo );  
  dcolread(&colw,&n,namo,ncolow);   hist_addwdat(ho,col,n,colw); free(col);  free(colw); 
}


/* Operation involving several histograms */

void hist_op(hists *ho, hists *hoa, char co, hists *hob){
  int na,nb,n,  i; long int ina, inb; double u; double *t,*ta,*tb;
  
  na=hoa->n;  t = ho ->tab; ina=hoa->in;
  nb=hob->n;  ta= hoa->tab; inb=hob->in;
  n =ho ->n;  tb= hob->tab; 

  if(n!=na || n!=nb || na!=nb){ fprintf(stderr, "#hist_op: incompatible histograms (%d,%d,%d)\n",na,nb,n); exit(EXIT_FAILURE); };
  
  switch(co){
  case '+': 
    ho->in = ina      + inb     ;
    ho->out= hoa->out + hob->out;   for(i=0; i<n; i++){ t[i] = ta[i] + tb[i];}; 
    break;
   case '/': 
     if(ina!=inb){ fprintf(stderr, "#hist_op: strange division (%ld,%ld)\n",ina,inb); exit(EXIT_FAILURE); };
    
     for(i=0; i<n; i++){ 
       u=tb[i];  if(0.<fabs(u)){ t[i] = ta[i]/u;}else{ t[i]=-0.;}; 
     } 
     break;
     
  default : fprintf(stderr,"#hist_op:operation not implemented\n");  exit(EXIT_FAILURE);  
  }
  
  /* a vérifier */
}


void hist_nor(hists *hso, char modo, double moyo, hists* ho){
  int n,i; double step,u,v,sum;  double *t;
  
  hist_copy(hso,ho);  n=ho->n   ;
  /**/                t=ho->tab ;
  /**/             step=ho->step;

  if(hso->in <= 0){
    fprintf(stderr,"#hist_nor: histogram is empty, no normalization \n");
  }
  else{
    /* bin volume  */
    switch(modo){
    case 'n': u=ho->istep;                  for(i=0; i<n; i++){                              t[i] *= u; }; break;
    case 'c': v=      M_PI*step*step     ;  for(i=0; i<n; i++){  u=v*(pow(i+1,2)-pow(i,2));  t[i] /= u; }; break;
    case 's': v=4./3.*M_PI*step*step*step;  for(i=0; i<n; i++){  u=v*(pow(i+1,3)-pow(i,3));  t[i] /= u; }; break;                
      /**/
    default : fprintf(stderr,"#hist_nor:normalization not implemented\n");  exit(EXIT_FAILURE);  
    }
  
    /*normalize so that the average value is moyo */
    u=0 ; for(i=0; i<n; i++){ u    += t[i];};  sum=u;
    
    if(0<fabs(moyo)){
      if(fabs(sum)>0){ u=moyo*n/sum;} 
      else           { u=1; fprintf(stderr,"#hist_nor: strange histogram not empty but sum zero \n");} 
    }else            { u=1/(sum*step);     }

    /**/; for(i=0; i<n; i++){ t[i] *= u  ;};
    }

}


/* save, load and print */

void hist_x(hists *ho, double **xo){
  int i, n; double min,step;
  
  n    = ho->n   ;
  min  = ho->min ;  if(0<n){ step = ((ho->max)-min)/n;}else{fprintf(stderr,"#hist_x: pbn=%d<0 \n",n); exit(1);}  
  
  *xo = (double*) calloc(n, sizeof(double));    for(i=0; i<n; i++){  (*xo)[i] = min + (i+0.5)*step; }
}


void hist_save(hists *ho, char *namo, char *como ){
  char com[_NL]; double **tab;
  
  sprintf(com,"# %d %g %g %ld %ld \n#h->: n min max in out \n", ho->n, ho->min, ho->max, ho->in, ho->out); strmncat(com,_NL,como);
  
  tab = (double**) calloc(1, sizeof(double*));  tab[0]=ho->tab;  dcolsavem(1,tab,ho->n,namo,com);     free(tab);
}


void hist_load( hists *ho, char *namo, int ncolo){
  int a,n,ntmp; long int in,out;  double min,max;  double *tmp; FILE *f;
  
  a=fopener(&f,namo,"r");  

  if(a==1){
    fscanf(f,"# %d %lg %lg %ld %ld \n", &n,&min,&max, &in,&out); fclose(f);        dcolread(&tmp,&ntmp, namo, ncolo); 
    
    if(n!=ntmp){ fprintf(stderr, "#hist_load: data in <%s> is unvalid (%d!=%d) \n",namo,n,ntmp); exit(EXIT_FAILURE); };
  
    ho->in =in ;
    ho->out=out;  dcolcopy(tmp,ntmp,ho->tab); free(tmp); 
  }
  
  else{ dcolzero(ho->tab,ho->n); }
}



/* not completed */

void hist_initandload(char *namo, int ncolo, hists *ho){
  int n,neff; long int in,out;  double min,max; FILE *f;
  
  fopener(&f,namo,"r"); fscanf(f,"# %d %lg %lg %ld %ld \n", &n,&min,&max, &in,&out);   hist_init(ho,n,min,max);   ho->in =in ;
  /**/                                                                                                            ho->out=out;

  dcolread(&(ho->tab),&neff, namo, ncolo); 

  if(n!=neff){ fprintf(stderr, "#hist_load: data in <%s> is unvalid (%d!=%d) \n",namo,n,neff); exit(EXIT_FAILURE); };

  fclose(f);
}



void hist_savemspecial(int ntabo, hists **tabo,  char *namo, char *como){
  /* all hists with same x axis */
  int i,n; char com[_NL]; double *colx, **tab; hists* h;

  h=tabo[0]; n=h->n; 
  
  hist_x(h, &colx);   sprintf(com,"# %d %g %g %ld %ld \n#h->: n min max in out \n", h->n, h->min, h->max, h->in, h->out);
  
  strmncat(com,_NL,como); 

  tab = (double**) calloc(1+ntabo, sizeof(double*));  tab[0]=colx;  for(i=1; i<=ntabo; i++){  tab[i]=((*tabo)[i-1]).tab; }; 
  
  dcolsavem(1+ntabo,tab,n,namo,com);     free(tab); free(colx);
}
