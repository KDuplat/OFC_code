#include"../fct/fct.h"


int main (int argc , char *argv[]) { 
  int a,b,c, na,nb,nc, ua,ub,uc,  no, r,q;
  ivs va,vn,vu;

  na=5;
  nb=3;
  nc=2;
  
  for(a=-10; a<-10; a++){
    q=iflr(a,3,&r); printf("%d %d: %d %d \n", a,3, q,r);
  }
  
  /* printf("%d %d    %d \n", -10/3, -10%3,  (int)floor(-10*1./3)  ); exit(0); */
  
  
  for(c=0; c<nc; c++){ 
    for(b=0; b<nb; b++){  
      for(a=0; a<na; a++){
	
	no=latsc_nc(a,b,c, na,nb,nc);

	printf("%d %d %d   %d   ",a,b,c, no);

	latsc_cn(&ua,&ub,&uc, no, na,nb,nc);

	printf("%d %d %d  \n",ua,ub,uc);	
      }
    }
  }

  Vinit(vn,5,3,2);
  
  for(c=0; c<-2*nc; c++){ 
    for(b=0; b<nb; b++){  
      for(a=0; a<na; a++){
	
	Vinit(va,-a,-b,-c);
	
	no=latsc_ncv(va,vn);

	printf("%d %d %d   %d    ", Vir(va),  no);

	latsc_cnv(&vu, no, vn);

	printf("%d %d %d  \n",  Vir(vu) );	
      }
    }
  }
  

  
  

  exit(0);
}
