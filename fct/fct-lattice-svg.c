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


/*sc: (simple) cubic lattice */
inline int latsc_nc(int ao, int bo, int co, int nao, int nbo, int nco){
  int a,b,c;
  a = rfl(ao,nao);
  b = rfl(bo,nbo);
  c = rfl(co,nco);   return( a + b*nao + c*nao*nbo); 
  /*This numbering is valid only within the box, periodic conditions are then implied if coor are outside */
}

inline void latsc_cn(int *ao, int *bo, int *co, int no, int nao, int nbo, int nco){
  int r,q; 
  q=iflr(no,nao*nbo,&r); *co=q;
  q=iflr(r ,nao    ,&r); *bo=q;
  /**/                   *ao=r;
}

/* fcc lattice */
inline int latfcc_nc(int ao, int bo, int co, int nao, int nbo, int nco){
  int ai,bi,ci, a,b,c, ar,br,cr, ind,dec, res;
  
  ai = rfl(ao,2*nao); a=iflr(ai,2,&ar);
  bi = rfl(bo,2*nbo); b=iflr(bi,2,&br);
  ci = rfl(co,2*nco); c=iflr(ci,2,&cr); ind=ar+br*2+cr*4;   

  switch(ind){
  case 0  : dec= 0; break ; /*(0,0,0)*/
  case 6  : dec= 1; break ; /*(0,1,1)*/
  case 5  : dec= 2; break ; /*(1,0,1)*/
  case 3  : dec= 3; break ; /*(1,1,0)*/
  default : dec=-1; fprintf(stderr,"#latfcc_nc: %d (%d %d %d )\n",ind,ar,br,cr);  exit(EXIT_FAILURE);  
  }  
  
  res = 4*latsc_nc(a,b,c, nao,nbo,nco) + dec;    return(res);
}

inline void latfcc_cn(int *ao, int *bo, int *co, int no, int nao, int nbo, int nco){
  int nsc,rsc, a,b,c; ivs v;

  nsc=iflr(no,4,&rsc);    latsc_cn(&a,&b,&c, nsc, nao,nbo,nco);
  
  switch(rsc){
  case 0  : Vinit(v,0,0,0); break; 
  case 1  : Vinit(v,0,1,1); break;
  case 2  : Vinit(v,1,0,1); break;
  case 3  : Vinit(v,1,1,0); break; 
  default : fprintf(stderr,"#latfcc_cn: %d \n",rsc);  exit(EXIT_FAILURE);  
  }
  
  *ao= 2*a + v.x1;
  *bo= 2*b + v.x2;
  *co= 2*c + v.x3;
}


inline int latsc_ncv (ivs vo, ivs vno){  return( latsc_nc (vo.x1,vo.x2,vo.x3, vno.x1,vno.x2,vno.x3) ) ;}
inline int latfcc_ncv(ivs vo, ivs vno){  return( latfcc_nc(vo.x1,vo.x2,vo.x3, vno.x1,vno.x2,vno.x3) ) ;}

inline void latsc_cnv (ivs *vo, int no, ivs vno){ latsc_cn ( &(vo->x1), &(vo->x2), &(vo->x3), no, vno.x1,vno.x2,vno.x3); }
inline void latfcc_cnv(ivs *vo, int no, ivs vno){ latfcc_cn( &(vo->x1), &(vo->x2), &(vo->x3), no, vno.x1,vno.x2,vno.x3); }








/*________________________________________________________________________________*/
/* lptl */

/*  pour le sc */
int no_coor(int x, int y, int z, int m ){
  return(1+x+ y*m + z*m*m ) ;
}
void coor_no(int* px, int *py, int *pz, int no, int m) {
  int r, q ;
  q =  (int) floor( (no-1)/(m*m) ) ;  *pz =  q ; r = no -1 - q*m*m ; 
  q =  (int) floor( r/m  )         ;  *py =  q ; r = r  - q*m      ; 
  *px = r  ; 
}

/* pour le bcc */

int no_coor1a(int x, int y, int z, int m ){
  int nb ; 
  if (x%2==1){ x = (x-1)/2+1 ;  y  = (y-1)/2+1 ; z  = (z-1)/2+1 ;  nb = 1 + (x-1)+(y-1)*m+(z-1)*m*m ; return(  2*nb    ) ; }
  else       { x =     x/2+1 ;  y  =     y/2+1 ; z  =     z/2+1 ;  nb = 1 + (x-1)+(y-1)*m+(z-1)*m*m ; return( (2*nb)-1 ) ; }
}

void coor_no1a(int* px, int *py, int *pz, int no, int m){
  
  int nb, q, r ;
  
  nb = (int) floor(no/2.) + (no%2) ;
  
  q =  (int) floor( (nb-1)/(m*m) ) ;  *pz = 1 + q ; r = nb - q*m*m ; 
  q =  (int) floor( (r-1)/m     )  ;  *py = 1 + q ; r = r  - q*m   ; 
  *px = r  ; 

  /*  printf("%d : %d %d %d \n", nb, *px,  *py,  *pz ) ; */

  *px = (2*(*px)) - 1 - (no%2) ; 
  *py = (2*(*py)) - 1 - (no%2) ; 
  *pz = (2*(*pz)) - 1 - (no%2) ; 
  
}

int no_coor2a(int x, int y, int z, int m ){
  return(1 + ((x-(z%2))/2) +(((y-(z%2))/2)*m) + (z*m*m) ) ;
}

void coor_no2a(int* px, int *py, int *pz, int no2, int m){

  int q, r ;  
  q =  (int) floor((no2-1)/(m*m)) ;  *pz = q               ; r = no2 -1 - (q*m*m) ; 
  q =  (int) floor(      r/m    ) ;  *py = (2*q)+((*pz)%2) ; r = r  - (q*m)    ;
  *px = (2*r) + ((*pz)%2) ;
}
