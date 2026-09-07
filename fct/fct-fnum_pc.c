#include"fct.h"

/* ___________________valid but not updated nor checked __________________________________*/


void interpol_lin(double* colx, double* coly, double x, double* res ){ 
 
  int i, n ;  
  double xmin, xmax  ; 

  n    = (int) colx[0] ; 
  xmin =       colx[1] ; 
  xmax =       colx[n] ; 
  
  *res = -1e100 ; 

  if ( x<xmin || xmax<x ){
    fprintf(stderr,"# Attention: x<xmin || xmax<x dans interpol_lin ") ; 
    fprintf(stderr,"# x   =%g  # xmin=%g  # xmax=%g \n", x,  xmin, xmax) ;
  
    if(x<xmin){ *res=coly[1];} 
    if(xmax<x){ *res=coly[n];} 
  }
  else{ 
    /* colx rangé de manière monotone */ 
    for (i=1 ; i<=n-1; i++ ) { 
      if (  (colx[i]<=x && x<=colx[i+1]) || (colx[i+1]<=x && x<=colx[i])  ){ 
	*res = coly[i] +  (coly[i+1]- coly[i])/(colx[i+1]- colx[i])*(x-colx[i]) ; 
      } 
    } 
  } 
} 
