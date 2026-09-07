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




/* autres */


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

