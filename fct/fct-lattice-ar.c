#include"fct.h"


inline int ifl(int ao, int bo){  int q  ; q=(int) floor(ao*1./bo); return(q)      ; }

inline int rfl(int ao, int bo){  
  int q,r;   q=(int)floor(ao*1./bo);   r=ao-q*bo; 
  
  if(0<=r && r<bo){ return(r);}
  else            { fprintf(stderr, "#rfl:  pb (%d %d    %d %d) \n",ao,bo,q,r); exit(EXIT_FAILURE); }
}


inline int iflr(int ao, int bo, int *ro){
  int r,q;  q =  (int) floor( ao*1./bo);  r=ao-q*bo;            
  
  if(0<=r && r<bo){  *ro=r ; return(q); }
  else            {  *ro=-1; fprintf(stderr, "#iflr:  pb (%d %d %d   %d %d) \n",ao,bo,*ro,q,r); exit(EXIT_FAILURE); }
}


/* distance */

inline int lat_bcvec(int ao, int no){
  int r; double mino; 
  
  mino=0.5*no; r=ao; if      (ao<-mino){ r += no;}
  /**/               else{ if(ao> mino){ r -= no;}; }; return(r);
}

void lat_vec(ivs *vo, ivs vao, ivs vbo, ivs vno){
  ivs vai,vbi,v;
  vai°=rfl(vao°,vno°);  vbi°=rfl(vbo°,vno°);  v°=vbi°-vai°;  (*vo)°=lat_bcvec(v°,vno°);
}

inline int lat_dis2(ivs vao, ivs vbo, ivs vno){ ivs v;    lat_vec(&v,vao,vbo,vno); return(Vsa(v));}



/*sc: (simple) cubic lattice */

inline int latsc_nc(ivs vo, ivs vno){  
  ivs v;
  v° = rfl(vo°,vno°);   return( v.x1 + vno.x1*(v.x2 + vno.x2*v.x3) ); 
  /*This numbering is valid only within the box, periodic conditions are then implied if coor are outside */
}

inline void latsc_cn(ivs *vo, int no, ivs vno){
  int r,q, na,nanb;
  
  na=vno.x1;   nanb=na*vno.x2;
  
  q=iflr(no,nanb,&r); vo->x3=q;
  q=iflr(r ,na  ,&r); vo->x2=q;
  /**/                vo->x1=r;
}

/* fcc lattice */
inline int latfcc_nc(ivs vo, ivs vno){
  int ind,dec,res; ivs vi,v,vr; 
  
  vi° = rfl(vo°,2*vno°);   v° = iflr(vi°,2,&(vr°));   ind=vr.x1 + 2*vr.x2 + 4*vr.x3 ; 
  
  switch(ind){
  case 0  : dec= 0; break ; /*(0,0,0)*/
  case 6  : dec= 1; break ; /*(0,1,1)*/
  case 5  : dec= 2; break ; /*(1,0,1)*/
  case 3  : dec= 3; break ; /*(1,1,0)*/
  default : dec=-1; fprintf(stderr,"#latfcc_nc: %d (%d %d %d )\n", ind, Vir(vr) );  exit(EXIT_FAILURE);  
  }  
  
  res = 4*latsc_nc(v,vno) + dec;    return(res);
}

inline void latfcc_cn(ivs *vo, int no, ivs vno){
  int nsc,rsc; ivs v,vdec;
  
  nsc=iflr(no,4,&rsc);    latsc_cn(&v, nsc, vno);
  
  switch(rsc){
  case 0  : Vinit(vdec,0,0,0); break; 
  case 1  : Vinit(vdec,0,1,1); break;
  case 2  : Vinit(vdec,1,0,1); break;
  case 3  : Vinit(vdec,1,1,0); break; 
  default : fprintf(stderr,"#latfcc_cn: %d \n",rsc);  exit(EXIT_FAILURE);  
  }

  (*vo)°= 2*v° + vdec°;
}



/* _c: for components, but probably not needed, let's wait and see  */
/* FUT: use fonction as an argument? */
inline int latsc_nc_c(int ao, int bo, int co, int nao, int nbo, int nco){ 
  ivs v,vn; 
  Vinit(v , ao, bo, co);
  Vinit(vn,nao,nbo,nco);  return( latsc_nc(v,vn) );  
}

inline int latfcc_nc_c(int ao, int bo, int co, int nao, int nbo, int nco){
  ivs v,vn; 
  Vinit(v , ao, bo, co);
  Vinit(vn,nao,nbo,nco);  return( latfcc_nc(v,vn) );   
}


inline void latsc_cn_c(int *ao, int *bo, int *co, int no, int nao, int nbo, int nco){
  ivs v,vn; 
  Vinit(vn, nao,nbo,nco); latsc_cn(&v,no,vn); Vtini(v,*ao,*bo,*co);
}

inline void latfcc_cn_c(int *ao, int *bo, int *co, int no, int nao, int nbo, int nco){
 ivs v,vn; 
 Vinit(vn, nao,nbo,nco); latsc_cn(&v,no,vn); Vtini(v,*ao,*bo,*co);
}


/* wrapper function */

inline int lat_nc(int to,ivs vo, ivs vno){  
  int r;
  switch(to){
  case LATSC : r=latsc_nc (vo,vno); break;
  case LATFCC: r=latfcc_nc(vo,vno); break;
  default: fprintf(stderr,"#lat_ncv: this type of lattice (%d) is not implemented \n",to);  exit(EXIT_FAILURE); 
  } 
  return(r);
}


inline void lat_cn(int to,ivs *vo, int no, ivs vno){
  switch(to){
  case LATSC : latsc_cn (vo,no,vno); break;
  case LATFCC: latfcc_cn(vo,no,vno); break;
  default: fprintf(stderr,"#lat_cnv: this type of lattice (%d) is not implemented \n",to);  exit(EXIT_FAILURE); 
  } 
}


/* lattice functions */

void  lattice_parameters(lats *lato){
  int n,nbas,nnei,unit,  a,b,c; ivs *va,*vb, dim;

  lato->t=-1;  dim=lato->dim;
  
  if( STRCMP(lato->type,"sc" ) ){ lato->t=LATSC ; }
  if( STRCMP(lato->type,"fcc") ){ lato->t=LATFCC; }
  
  switch(lato->t){
  case LATSC :  nbas=1; nnei= 6; unit=1; break;
  case LATFCC:  nbas=4; nnei=12; unit=2; break;
  default: fprintf(stderr,"#lattice_parameters: this type of lattice (%s) is not implemented \n",lato->type);  exit(EXIT_FAILURE); 
  }
  
  lato->unit=unit;   lato->dimu°=unit*dim°;
  
  Vtini(dim,a,b,c);   n=nbas*a*b*c;

  lato->n   =n   ; lato->pt   = (latpts*)calloc(n   ,sizeof(latpts));
  lato->nbas=nbas; lato->bas  = (ivs   *)calloc(nbas,sizeof(ivs   ));
  lato->nnei=nnei; lato->nei  = (ivs   *)calloc(nnei,sizeof(ivs   ));

  va = lato->bas;
  vb = lato->nei;
  
  if(lato->t==LATSC){
    Vinit(va[0], 0, 0, 0); 
   
    Vinit(vb[0],  1,0,0);  Vinit(vb[1], 0, 1,0);   Vinit(vb[2], 0,0, 1);
    Vinit(vb[3], -1,0,0);  Vinit(vb[4], 0,-1,0);   Vinit(vb[5], 0,0,-1);
  }


  if(lato->t==LATFCC){
    Vinit(va[0], 0, 0, 0); 
    Vinit(va[1], 1, 1, 0);  Vinit(va[ 2], 1, 0, 1);   Vinit(va[ 3],0, 1, 1);
    
    Vinit(vb[0], 1, 1, 0);  Vinit(vb[ 1], 1, 0, 1);   Vinit(vb[ 2],0, 1, 1);
    Vinit(vb[3],-1, 1, 0);  Vinit(vb[ 4],-1, 0, 1);   Vinit(vb[ 5],0,-1, 1);
    Vinit(vb[6], 1,-1, 0);  Vinit(vb[ 7], 1, 0,-1);   Vinit(vb[ 8],0, 1,-1);
    Vinit(vb[9],-1,-1, 0);  Vinit(vb[10],-1, 0,-1);   Vinit(vb[11],0,-1,-1);
  }
  
}

void  lattice_fill(lats *lato){
  int i,a,p, n,t,nnei; ivs v, pos,dim, *nei; latpts *pt;
  
   t   = lato->t   ;
   n   = lato->n   ;
   dim = lato->dim ;
   nnei= lato->nnei;
   nei = lato->nei ; /*lattice field, not latpts*/

   for(i=0; i<n; i++){ pt=&( (lato->pt)[i] );
     
     pt->nnei=nnei; pt->ind=1;     lat_cn(t,&v,i,dim); pt->v=v;   
     
     /*Check*/ 
     a=lat_nc(t,v,dim); /*printf("%d %d %d   %d %d \n",Vir(iv), a,no);*/ if(a!=i){ printf("#lattice_fill:Pb here\n"); exit(1);}
     
     for(p=0; p<nnei; p++){   pos° = v° + nei[p]°;  pt->nei[p]=lat_nc(t,pos,dim); }
   }
}

void  lattice_show(lats *lato){
  int i,p, n,t,nnei; ivs v,dim; latpts *pt;
  
  t   = lato->t   ;
  n   = lato->n   ;
  dim = lato->dim ;
  nnei= lato->nnei;
  
  for(i=0; i<n; i++){ pt=&( (lato->pt)[i] );
     
    printf("%d  %d  %d %d %d  %g %g %g   %d  ", i, pt->ind, Vir(pt->v),  Vir(pt->vr), pt->nnei);  for(p=0; p<nnei; p++){ printf("%d ", pt->nei[p]);}
    
    for(p=0; p<nnei; p++){  lat_cn(t,&v,pt->nei[p],dim);  printf("%d ", lat_dis2(pt->v,v,lato->dimu) );}
    
    printf("\n");
  }
}

void  lattice(lats *lato,char* typo, ivs dimo){
  
  sprintf(lato->type,"%s",typo);  (*lato).dim° = dimo°;
  
  lattice_parameters(lato);
  lattice_fill(lato);  
}

void  lattice_free(lats *lato){
  free(lato->bas);
  free(lato->nei);
  free(lato->pt);
}



void lattice_vr(lats *lato, vs vdeco, vs ao){
  /*NOTE: express vdeco in cell unit (like dim) */
  int i,n;  double fac;  latpts *pt;  
  
  n   =   lato->n    ;
  fac =1./lato->unit ;
  
  for(i=0; i<n; i++){   
    pt=&( (lato->pt)[i] );  pt->vr° =  ao° *( fac * pt->v°  -  vdeco°);  
  }
}

void lattice_point(lats *lato, int **icolo, int *no){ /*only valid points */
  int i,n,a; latpts *pt;  n=lato->n    ;
  
  a=0;  Bvalidpt(i,n,                a++;);  *no=a;  *icolo = (int*) calloc(a, sizeof(int)); 
  a=0;  Bvalidpt(i,n, (*icolo)[a]=i; a++;);
}

int  lattice_validbond(latpts *pto, int ao, int bo){
  if(bo<ao){              return(-1);}
  else{
    if(  pto[ao].ind!=1){ return(-1);}
    else{
      if(pto[bo].ind!=1){ return(-1);}
      else              { return( 1);}
    }
  }
}

void lattice_colbond(lats *lato, int **ao, int **bo, int *no){
  int i,n,p,in,a; latpts *pt;  n=lato->n ;
  
  a=0; Bvalidbond(i,n,in,p,  a++; );   *no=a;  *ao=(int*) calloc(a, sizeof(int)); 
  /**/                                         *bo=(int*) calloc(a, sizeof(int)); 
  
  a=0; Bvalidbond(i,n,in,p,  (*ao)[a]=i; (*bo)[a]=in; a++; ); 
}
