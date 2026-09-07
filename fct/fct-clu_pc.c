
#include"fct.h"

/* Functions for cluster analysis */
/* inspired by archive-madison/alter/rog/gorp/clu.c but written as more portable, hopefully */
/* remarque :  tab_spar, les particules du cluster p vont de l'indice  tab_sclu[p].loc  à  tab_sclu[p].loc+tab_sclu[i].size-1   */


void cluster_allocation(sclua *clua){
  int nt,i,nv ;  nt=clua->nt;  

  nv=clua->nv; if (nv > _CLUNCMAX){ printf("[cluster_init] %d >  _CLUNCMAC=%d : too many neighbors  \n", nv,_CLUNCMAX); exit(0); }

  clua->data  = (int*)  calloc(nt, sizeof(int ) );
  clua->label = (int*)  calloc(nt, sizeof(int ) );
  clua->class = (int*)  calloc(nt, sizeof(int ) );
  clua->nlist = (int**) calloc(nt, sizeof(int*) );   for (i=0; i<nt; i++) { clua->nlist[i] =(int*)calloc(clua->nv, sizeof(int)) ;}  
   
  /* pour cluster */
  clua->tab_spar  = (spar*)  calloc( 1, sizeof(spar) );
  clua->tab_sclu  = (sclu*)  calloc( 1, sizeof(sclu) );
}



int cluster_label(sclua *clua) {
  int nt,nv,   i,j, k, mini, nu[_CLUNCMAX], la[_CLUNCMAX];

  nt=clua->nt;
  nv=clua->nv;
  
  /* Initialisation */
  for (i=0; i<nt; i++ ) { 
    if (clua->data[i]>0){  clua->label[i]=i ; clua->class[i]=i ;  } 
    else                {  clua->label[i]=0 ;               }
  }

  for (i=0 ; i<nt ; i++ ) { 
    
    if ( clua->label[i] !=0 ){ 
      
      mini=clua->label[i] ; 
      
      for (j=0 ; j<nv ; j++ ) { 
	nu[j] = clua->nlist[i][j]  ;
	la[j] = clua->label[nu[j]] ;
	
	if ( nu[j]<i && la[j]!=0 ){  k = la[j] ; while( k!=clua->class[k] ){ k=clua->class[k] ;} ; la[j]=k  ;   }
	
	if ( nu[j]<i && la[j]!=0 && la[j]<mini){ mini=la[j]; }
	
      }/*nv*/
      
      clua->label[i]=mini ;
      for (j=0 ; j<nv ; j++ ) {
	if ( nu[j]<i && la[j]!=0 ){  if ( mini<clua->class[la[j]] ){ clua->class[la[j]]=mini ; }  }
      } 
    }
  }
  
  /* classification finale et résultat */
  for (i=0 ; i<nt ; i++ ) { 
    if (clua->label[i] !=0){ 
      k = clua->label[i] ;  while( k!=clua->class[k] ){ k=clua->class[k] ; } ; clua->label[i] = k ; 
    }
  }
  return(1) ;  
}
int compare_spar_lab(const void *spara, const void *sparb){
  if ( (*(spar*)spara).lab < (*(spar*)sparb).lab  ) { return(-1) ; }
  else{ return(1) ; }
}

int compare_sclu_size(const void *sclua, const void *sclub){
  if ( (*(sclu*)sclua).size < (*(sclu*)sclub).size  ) { return(1) ; }
  else{ return(-1) ; }
}


int cluster_analyse(sclua *clua){
  int i,j, n,nt, nc, i1,i2,l  ;  nt=clua->nt; 

  
  free(clua->tab_spar) ;
  free(clua->tab_sclu) ;

  /* nombre de particules, allocation */
  n = 0 ;  for ( i=0 ; i<nt ; i++){   if ( clua->label[i]!=0 ){ n++ ; };  }; clua->n=n;
  
  clua->tab_spar  = (spar*)  calloc( n+1, sizeof(spar) ) ;
  
  if (n==0){ return(0) ; }
  else     {
    
    n=0 ; 
    for ( i=0 ; i<nt ; i++){
      if ( clua->label[i]!=0 ){ 
	n++ ; 
	clua->tab_spar[n].no  =  i        ;
	clua->tab_spar[n].lab =  clua->label[i] ;
      }
    }
  
    qsort( &(clua->tab_spar[1]), n, sizeof(spar), compare_spar_lab ) ;
    
    /* nombre de clusters */
    nc=1 ;
    for ( i=2 ; i<=n ; i++){
      if ( clua->tab_spar[i].lab != clua->tab_spar[i-1].lab ){ nc ++ ;   }
    }

    clua->tab_sclu  = (sclu*)  calloc( nc+1, sizeof(sclu) );  clua->tab_sclu[0].lab = nc ;  
    
    j=1 ;
    clua->tab_sclu[j].lab   = clua->tab_spar[1].lab;
    clua->tab_sclu[j].size  = 1 ;
    clua->tab_sclu[j].loc   = 1 ;

    for ( i=2 ; i<=n ; i++){
      if   ( clua->tab_spar[i].lab == clua->tab_spar[i-1].lab ){   (clua->tab_sclu[j].size) ++ ; } 
      else{
	j++ ;
	clua->tab_sclu[j].lab   = clua->tab_spar[i].lab;
	clua->tab_sclu[j].size  = 1 ;
	clua->tab_sclu[j].loc   = i ;
      }
    }
    
    qsort( &(clua->tab_sclu[1]), nc, sizeof(sclu), compare_sclu_size );      

    /* 22-01-01: addition: replace label with rank of cluster size, see cluster_listpart */ 
    for (l=1 ; l<=nc; l++ ) {
      i1 =      clua->tab_sclu[l].loc;
      i2 = i1 + clua->tab_sclu[l].size;
      for (i=i1; i<i2; i++){  clua->label[ clua->tab_spar[i].no ]=l; };   /*fprintf(stderr,"l=%d no=%d \n", l, clua->tab_spar[i].no); */
    }/* fin addition*/
    
   clua->nc=nc;   return(nc) ;
    
  }/*n=0*/
}
/* remarque :  tab_spar, les particules du cluster p vont de l'indice  tab_sclu[p].loc  à  tab_sclu[p].loc+tab_sclu[i].size-1   */



void cluster_listpart(sclua clua){
  int l, i,i1,i2; 

 for (l=1 ; l<=clua.nc; l++ ) {
    i1=     clua.tab_sclu[l].loc ;
    i2=i1 + clua.tab_sclu[l].size;
    fprintf(stderr,"Cluster %d: ", l);   for (i=i1; i<i2; i++){ fprintf(stderr,"%d ", clua.tab_spar[i].no); };    fprintf(stderr,"\n"); 
  }
}

