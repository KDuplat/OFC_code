#include"fct.h"

/*nbi structure (two ints) */

inline int nbismallerthan(nbis a,  nbis b){
  int an1, bn1;
  an1=a.n1;
  bn1=b.n1;
  
  if (   an1<bn1 || (an1==bn1 && (a.n2<b.n2)) ){return( 1);}
  else                                         {return(-1);}
}


void  swapnbi(nbis *tab, int a, int b){nbis nbi;  nbi=tab[a];  tab[a]=tab[b];  tab[b]=nbi;}

int siftnbi(nbis *tab, int start, int count) {
  int root, child ;
  root=start ;
  while ( (2*root+1)< count) {
    child = 2*root + 1 ;
    if ( (child < count-1) &&  (nbismallerthan(tab[child],tab[child+1])==1) ){ child++ ; }
    if ( nbismallerthan(tab[root],tab[child])==1) { swapnbi(tab, root, child) ;  root = child ;}
    else{ return(0) ;} 
  }
  return(0) ; 
}

void heapsortnbi(nbis *tab, int count) {
  int start, end ;
  start = count/2 -1 ;   end = count-1 ;
  while (0<=start){ siftnbi(tab, start, count) ;                     start-- ;}
  while (0<end)   { swapnbi(tab, end,0)        ; siftnbi(tab,0,end) ; end--   ;}
}
