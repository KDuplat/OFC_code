#include<math.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<time.h> 
#include<unistd.h>

#include"fonction.h"  
#include"fonctionL.h"  

#define eps 1e-10

/* remarque : fonction decomp_nom supprimée (voir ancien fichiers fonctionL.c )*/


int multi_1000(double x) {    
  int res ;
  char sbuf1[100],  sbuf2[100] ; 
  sprintf(sbuf1,"%f",x) ;
  sbuf2[0] = sbuf1[2] ; /* sbuf2[4] = sbuf1[7] ; */
  sbuf2[1] = sbuf1[3] ; /* sbuf2[5] =  sbuf1[8] ;  */
  sbuf2[2] = sbuf1[4] ; /* sbuf2[6] = sbuf1[9] ;  */
  sbuf2[3] = '\0' ;
  res = atoi(sbuf2) ;
  return(res) ;
}


void get_str_nom(char* nom, char* s1, char* s2, char* s0, char *res) {

  /* Si strlen(s0)==0 par exemple s0="",      */
  /*     on cherche la derni??re occurence de s1 dans nom, puis apr??s s1, la premi??re occurence de s2 */
  /*     on renvoie le nombre qui se trouve entre s1 et s2. */
  /* Si strlen(s0)!=0 , */
  /*     on cherche la derni??re occurence de s0 dans s1, puis apr??s celle-ci, la premi??re occurence de s1,  */
  /*     et ?? la suite de celle-ci, la premi??re occurence de s2. */
  /*     on renvoie le nombre qui se trouve entre s1 et s2. */
  /* Dans les deux cas, si s2 est vide (ie ""), on prend le nombre entre s1 et la fin de la chaine nom. */

  int i, j, k, len ;
  char* buf ;

  if ( strlen(s0)==0 ){  
    i=strlen(nom) ; 
    while ( strncmp(&nom[i], s1, strlen(s1))!=0 ) { i-- ; }  ;
  }
  else{
    i=strlen(nom); while ( strncmp(&nom[i], s0, strlen(s0))!=0 ) { i-- ; }  ;
    i=i+strlen(s0) ;
    while ( strncmp(&nom[i], s1, strlen(s1))!=0 ) { i++ ; }  ;
  }
  if (i==-1) { fprintf(stderr,"get_str_nom (fonctionL.c) : je n'ai pas trouv??  <%s> dans <%s> \n", s0, nom ) ; }
  
  if ( strlen(s2)==0 ) {
    j=strlen(nom)+1 ;
  }else{
    j=i ; 
    while ( strncmp(&nom[j], s2, strlen(s2))!=0 ) { j++ ; }  ;
    if (strlen(nom)<=j){ 
      fprintf(stderr,"get_str_nom (fonctionL.c):je n'ai pas trouv??  <%s> dans <%s> apr??s la derni??re occurence de <%s>\n", s2, nom ,s0) ; }
  }
  
  if ( (j-i-strlen(s1))<=0 ){
    fprintf(stderr,"get_str_nom (fonctionL.c):je n'ai rien trouv?? entre <%s> et <%s> dans <%s> apr??s la derni??re occurence de <%s>\n",s1,s2, nom,s0 ) ;  
  }

  
  len=(j-i-strlen(s1)); buf = (char*) calloc( len, sizeof(char));
  
  for (k=(i+strlen(s1)) ; k<=(j-1) ; k++) {buf[k-(i+strlen(s1))] = nom[k] ;} ;
  
  sprintf(res,"%s", buf) ;    free(buf) ;
}

double get_par_nom(char* nom, char* s1, char* s2, char* s0  ) {
  char buf[1000];
  get_str_nom(nom,s1, s2,s0,buf); return(atof(buf));
}
    


double get_par_nomold(char* nom, char* s1, char* s2, char* s0  ) {

  /* Si strlen(s0)==0 par exemple s0="",      */
  /*     on cherche la dernière occurence de s1 dans nom, puis après s1, la première occurence de s2 */
  /*     on renvoie le nombre qui se trouve entre s1 et s2. */
  /* Si strlen(s0)!=0 , */
  /*     on cherche la dernière occurence de s0 dans s1, puis après celle-ci, la première occurence de s1,  */
  /*     et à la suite de celle-ci, la première occurence de s2. */
  /*     on renvoie le nombre qui se trouve entre s1 et s2. */
  /* Dans les deux cas, si s2 est vide (ie ""), on prend le nombre entre s1 et la fin de la chaine nom. */

  int i, j, k ;
  double x ;
  char* buf ;

  if ( strlen(s0)==0 ){  
    i=strlen(nom) ; 
    while ( strncmp(&nom[i], s1, strlen(s1))!=0 ) { i-- ; }  ;
  }
  else{
    i=strlen(nom); while ( strncmp(&nom[i], s0, strlen(s0))!=0 ) { i-- ; }  ;
    i=i+strlen(s0) ;
    while ( strncmp(&nom[i], s1, strlen(s1))!=0 ) { i++ ; }  ;
  }
  if (i==-1) { fprintf(stderr,"get_par_nom (fonctionL.c) : je n'ai pas trouvé  <%s> dans <%s> \n", s0, nom ) ; }
  
  if ( strlen(s2)==0 ) {
    j=strlen(nom)+1 ;
  }else{
    j=i ; 
    while ( strncmp(&nom[j], s2, strlen(s2))!=0 ) { j++ ; }  ;
    if (strlen(nom)<=j){ 
      fprintf(stderr,"get_par_nom (fonctionL.c):je n'ai pas trouvé  <%s> dans <%s> après la dernière occurence de <%s>\n", s2, nom ,s0) ; }
  }
  
  if ( (j-i-strlen(s1))<=0 ){
    fprintf(stderr,"get_par_nom (fonctionL.c):je n'ai rien trouvé entre <%s> et <%s> dans <%s> après la dernière occurence de <%s>\n",s1,s2, nom,s0 ) ;  
  }
  buf = (char*) calloc( (j-i-strlen(s1)) , sizeof(char)) ;
  for (k=(i+strlen(s1)) ; k<=(j-1) ; k++) {buf[k-(i+strlen(s1))] = nom[k] ;} ;
  x = atof(buf) ;				   
  free(buf) ;
  return(x) ;
}


/*  pour le sc */
int no_coor(int x, int y, int z, int m ){
  return(1+x+ y*m + z*m*m ) ;
}
void coor_no(int* px, int *py, int *pz, int no, int m) {
  int r, q ;
  q =  (int) floor( (no-1)/(m*m) ) ;  *pz =  q ; r = no -1 - q*m*m ; 
  q =  (int) floor( r/m  )         ;  *py =  q ; r = r  - q*m   ; 
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


/* autres */

char** list_file(char *nom, char* carac){
  int i,  n ;
  char** tab_file ;
  char sbufa[500],sbufb[500],sbufc[500],sbufs[500], name[500] ;
  time_t  t1 ;
  
  (void) time(&t1) ;  
  sprintf(name,"pmtcolefile_%d",  (int)t1 ) ;

  /* Séparation du répertoire et du nom du fichier */
  /*  sprintf(sbufc, "%s", pathm() ) ; */  
  i = strlen(nom)-1 ; while ( nom[i]!='/' && 1<=i) { i-- ; }
  
  if (i==0){
    sprintf(sbufa,"./") ;
    sprintf(sbufb,"%s", nom) ;
  }
  else{
    strncpy(sbufa,  nom     , i             ) ; sbufa[i]='\0' ;
    strncpy(sbufb, &nom[i+1], strlen(nom)-i ) ; 
  }
  fprintf(stderr,"[list_file]: <%s><%s>\n", sbufa, sbufb ) ;  

  /* sprintf(sbufs,"find %s -name \"%s\" -print -exec grep \"grepstring\" {} \\; | cat > tmp_colefile",sbufa,sbufb);system(sbufs) ; */
  
  if (strlen(carac)==0){ sprintf(sbufc,"Chaine impossible a trouver asdfghjklmnbvcxz") ; }
  else{                  sprintf(sbufc,"%s", carac) ; }

  /* carac ne marche pas, il faut se restreindre au fichier déjà terminés */

  sprintf(sbufs,"find %s -name \"%s\" -print -exec grep -I \"%s\" {} \\; | cat > %s",sbufa,sbufb,sbufc, name); 
  
  /* note madison, je ne comprends plus comment marche cette fonction */
  /* reponse: faire la liste des fichiers puis selectionner seulement ceux aui contiennent la chaine de caractere sbufc (* par defaut), au LPTL permettait de selectionner les fichiers dont le calcul etait acheve car il se terminait par "*Temps de calcul =..." a la derniere ligne   */

 /*  printf("<%s>", sbufs ) ;  */  system(sbufs) ;
  
  read_colw2(&tab_file, name, 1 ) ;  tab_file[0][strlen(tab_file[0])-1]='\0' ; n = (int) atoi(tab_file[0]) ;
  
  for (i=1 ; i<=n; i++ ) { tab_file[i][strlen(tab_file[i])-1]='\0' ;  /*  printf("liste_file <%s>\n",  tab_file[i]) ;*/ } 

  sprintf(sbufs, "rm -f %s\n", name )  ;  system(sbufs) ;  
  return(tab_file) ;
}

int ncol_file(char *nom){
  
  int i, ncol ;
  char buf[10000] ;
  FILE *f ;
  
  f = fopen ( nom , "r" ) ;
 
  if (f == NULL) { fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", nom ) ; exit(EXIT_FAILURE) ;
  }
  i=0 ;
  
  sprintf(buf,"#\n") ;
    
  /*  printf("<%s>\n", buf  ) ;AAAAA changer 4 avril 2007  */
 /*  while ( buf[0]=='*' || buf[0]=='#' || buf[0]=='%') {   fgets(buf,10000,f) ; printf("<%s>\n", buf); }   */
  
  i=0; while (fgets(buf,10000,f) != NULL && (buf[0]=='*' ||  buf[0]=='#' || buf[0]=='%') ) {  i++ ;  ;} ;
  
  ncol = 0 ;
  while( buf[i]!='\n' && i<strlen(buf) ){
    while (buf[i]!=' ' && buf[i]!='\t'){ i++ ; }
    ncol ++ ;
    while( buf[i]==' ' || buf[i]=='\t'){ i++ ;}
  }
  
  fclose(f);
  return(ncol) ;

}

/* deprecated, ne plus utiliser */
void moyeqm(double* moy, double* eqm, double* max, double* col ) {
  int i, n; double x ;
  
  n = (int)col[0];  *moy = 0.; *eqm = 0.; *max =col[1] ;

  for (i=1 ; i<=n; i++ ) { 
    x = col[i] ;
    *moy += x ;  
    if(*max<x) { *max=x ; } 
  }                      
  *moy = *moy/n  ; 
  
 for (i=1 ; i<=n; i++ ) { 
   x = col[i]- *moy ;
   *eqm +=  x*x ;
 }                      
 *eqm = *eqm/n  ; 
} 

void moyeqm2(double* col, double* moy, double* eqm, int n1, int n2) {
  int i, n, nf ; double x ;
  
  nf = (int)col[0];  *moy = 0.; *eqm = 0.;  n = n2-n1+1 ;

  for (i=n1 ; i<=n2; i++ ) {  x = col[i]      ; *moy += x   ; } ; *moy = *moy/n  ; 
  for (i=n1 ; i<=n2; i++ ) {  x = col[i]-*moy ; *eqm += x*x ; } ; *eqm = *eqm/n  ; 
  /*better to avoid difference between two large number, as in previous function */
} 


/*Madison */
void minmaxincol(double* col, double* rmin, double* rmax,  int n1, int n2){
  int i, nf ; double x,min, max ;
  
  nf = (int)col[0] ; min=1e100 ; max=-1e100 ;
  
  for (i=n1 ; i<=n2; i++ ) { 
    x = col[i] ;
    if(x<min){min=x;}
    if(max<x){max=x;}
  }
  *rmin=min ;
  *rmax=max ;
}

int s_in(char* sa, char* sb){
  int i, j, a,b ; 
  a = strlen(sa) ;  b = strlen(sb) ;   

  i = 0 ; while ( i<=a-b && strncmp(&sa[i],sb,b)!=0 ){ i++ ; } ; j=i ;  

  if (j==a-b+1){ return(-1) ;}
  else         { return(0)  ;}
}



int s_replace(char* sa, char* sb, char* sc ){

  int i, j, a,b,c ;
  char buf[1000] ;

  a = strlen(sa) ;    b = strlen(sb) ;   c = strlen(sc) ; 

  i = 0 ; while ( i<=a-b && strncmp(&sa[i],sb,b)!=0 ){ i++ ; } ; j=i ;  

  if (j==a-b+1){  fprintf(stderr,"je n'ai pas trouvé <%s> dans <%s> \n", sb , sa) ; return(-1) ;}
  else{
    buf[0]='\0' ;  strncat(buf,sa,j) ;  strncat(buf,sc,c) ;  strncat(buf,&sa[j+b],a-j-b) ;  sprintf(sa,"%s",buf) ; return(0) ;
  }
}


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
