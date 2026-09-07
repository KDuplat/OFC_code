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


/* distance on lattice */

inline int lat_bcvec(int ao, int no){
  int r; double mino; 
  
  mino=0.5*no; r=ao; if      (ao<-mino){ r += no;}
  /**/               else{ if(ao> mino){ r -= no;}; }; return(r);
}

void lat_vec(ivs *vo, ivs vao, ivs vbo, ivs vno){
  ivs vai,vbi,v;
  vai°=rfl(vao°,vno°);  
  vbi°=rfl(vbo°,vno°);  v°=vbi°-vai°;  (*vo)°=lat_bcvec(v°,vno°);
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
void lattice_defaultvalues(lats *lato){
  sprintf(lato->type,"nothing");

  Vinit(lato->dim ,0,0,0);  lato->t   =-1;
  Vinit(lato->dimu,0,0,0);  lato->unit= 0;
  
  lato->nbas=0; lato->bas=NULL;
  lato->nnei=0; lato->nei=NULL;
  
  lato->n    =0; lato->pt  =NULL;  lato->atomtype=0;
  lato->nbond=0; lato->bond=NULL;  lato->bondtype=0; 

  lato->nr   =0; lato->cor =NULL;
}


void  lattice_parameters(lats *lato){
  int i,n,nbas,nnei,unit,  a,b,c; ivs *va,*vb, dim;
  
  dim=lato->dim;
  
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
  lato->nr  =n   ; lato->cor  = (int   *)calloc(n   ,sizeof(int   ));
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
  
  for(i=0; i<n; i++){ lato->pt[i].no=i; lato->cor[i]=i;  }
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

void  lattice_pointshow(lats *lato, int modo){
  int i,p,a,  n,t,nnei,count; ivs v,dim; latpts *pt;
  
  t   = lato->t   ;
  n   = lato->n   ;
  dim = lato->dim ;
  nnei= lato->nnei; count=0; 
  
  for(i=0; i<n; i++){ pt=&( (lato->pt)[i] );
 
    if(modo==1 || (modo==2 && pt->ind==1) ){ count++;
    
      printf("%d %d  %d %d  %d %d %d  %g %g %g   %d  ", count, i,  pt->ind,pt->type,  Vir(pt->v),  Vir(pt->vr), pt->nnei);  for(p=0; p<nnei; p++){ printf("%d ", pt->nei[p]);}
      
      for(p=0; p<nnei; p++){ a=pt->nei[p]; if(0<=a){ lat_cn(t,&v,a,dim);  v=lato->pt[a].v;    printf("%d ", lat_dis2(pt->v,v,lato->dimu) );}else{printf("%d ",-1);};  }
      
      printf("\n");
    }
  }
}

void  lattice(lats *lato,char* typo, ivs dimo){
  
  lattice_defaultvalues(lato); sprintf(lato->type,"%s",typo);  (*lato).dim° = dimo°;
  
  lattice_parameters(lato);
  lattice_fill(lato);  
}

void  lattice_free(lats *lato){
  free(lato->bas);
  free(lato->nei);
  free(lato->pt );
  free(lato->cor);
}



void lattice_vr(lats *lato, vs vdeco, vs vao){
  /*NOTE: express vdeco in cell unit (like dim) */
  int i,n;  double fac;  latpts *pt;  
  n   =   lato->n    ;   lato->dec° =vdeco°;
  fac =1./lato->unit ;   lato->alat°=vao°  ;
  
  Bpt(i,n,pt,lato, /* does not work on the same line: macro + vector */  
      pt->vr°=vao°*(fac*pt->v° - vdeco°); 
      ); 
}


void lattice_reduce(lats *lato){
  int i,p,a,b,  n,nnei; latpts *pt; 
  
   nnei=lato->nnei; 
   n   =lato->n   ;  lato->cor = (int*) calloc(n, sizeof(int)); 
   
   a=0;  Bptval(i,n,pt,lato, lato->cor[a]=i; a++;  ); lato->nr=a;   /*cor is ordered*/      /* printf("%d %d %d \n",i,nr,p);  */
   
   for(i=0; i<lato->nr; i++){  
     a=lato->cor[i];   pt=&((lato->pt)[a]);   for(p=0; p<nnei; p++){  b=ibsearch('i',&(pt->nei[p]),lato->cor,lato->nr); if(b<0){ pt->nnei--; pt->nei[p]=-1;}; }; 
   } 
   
}

int  lattice_valbond(latpts *pto, int ao, int bo){
  if(bo<ao)             { return(-1);}
  else                  {
    if(  pto[ao].ind!=1){ return(-1);}
    else                {
      if(pto[bo].ind!=1){ return(-1);}
      else              { return( 1);}
    }
  }
}

void lattice_bond(lats *lato){
  int i,n,p,in,a; latpts *pt;  n=lato->n; free(lato->bond);
  
  a=0; Bbdval(i,n,pt,lato,p,in, a++; ); lato->nbond=a;  lato->bond=(latbds*) calloc(a, sizeof(latbds)); 
  
  a=0; Bbdval(i,n,pt,lato,p,in, 
	      lato->bond[a].type=lato->bondtype; lato->bond[a].a=i; lato->bond[a].b=in;   a++; 
	      );  
  
  /*check*/ a=0;    Bptval(i,n,pt,lato, a+=pt->nnei; );   a=a/2;  printf("%d %d \n",i,a); 
  if(a!=lato->nbond){  fprintf(stderr,"#lattice_bond: error in bond number (%d!=%d) \n",a,lato->nbond);  exit(EXIT_FAILURE);}  
  /* printf("%d %d \n",i,a); */
}


void lattice_bondshow(lats *lato){
  int i; latbds *bd;
  for(i=0; i<lato->nbond; i++){  bd=&(lato->bond[i]);  fprintf(stderr,"%d  %d  %d %d \n", i+1, bd->type, bd->a, bd->b);   };
}


void lattice_field(lats *lato, char *fo, void *vo){
  int i,n,ind, a; latpts *pt; 
   
  n=lato->n ; ind=-1; 
  
  if( STRCMP(fo,"pt.type") ){ ind=1;  a=*(int *)vo; Bpt(i,n,pt,lato, lato->pt[i].type=a;   );    };
  if( STRCMP(fo,"bd.type") ){ ind=1;  a=*(int *)vo; for(i=0; i<lato->nbond; i++){  lato->bond[i].type=a;};  };
  
  if(ind==-1){fprintf(stderr,"#lattice_field: field (%s) not implemented \n",fo);  exit(EXIT_FAILURE); }    
}


void lattice_lmpdat(lats* lato, lmpdats *lmpdato){
  int i,a,n; latpts *pt, ptmod;   latbds bd; 
  
  lmpdat_defaultvalues(lmpdato);  

  n=lato->n; 
  
  lmpdato->atomtypes=1;    lmpdato->xlo°= lato->alat° *(-lato->dec°             );
  lmpdato->bondtypes=1;    lmpdato->xhi°= lato->alat° *(-lato->dec° + lato->dim°);
  
  a=0;  Bptval(i,n,pt,lato, a++; );  

  lmpdato->natom=a;    lmpdato->atom = (latpts*)calloc(a, sizeof(latpts));
  
  a=0;  Bptval(i,n,pt,lato, ptmod=*pt;  ptmod.no=ibsearch('i',&(pt->no),lato->cor,lato->nr); lmpdato->atom[a]=ptmod; a++; );
  
  
  a=lato->nbond;  lmpdato->nbond=a; lmpdato->bond = (latbds*)calloc(a, sizeof(latbds)); 
  
  for(i=0; i<a; i++){ 
    bd=lato->bond[i]; 

    bd.a=ibsearch('i',&(bd.a),lato->cor,lato->nr);
    bd.b=ibsearch('i',&(bd.b),lato->cor,lato->nr);
    
    lmpdato->bond[i]=bd; 
  };
}


