#include<math.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

#include"fonction.h"

#define LECTURE 0 
#define ECRITURE 1

/* #define DEBUG   */

#ifdef DEBUG
#define DEB(_x_) _x_
#else
#define DEB(_x_) 
#endif
 

int max_i(int a, int b){
  if (b<a) { return(a) ;} 
  else    { return(b) ; }	
}	
int min_i(int a, int b){
  return(-max_i(-a,-b)) ;
}
double max_d(double a, double b){
  if (b<a) { return(a) ;} 	
  else{ return(b) ; }		
}
double min_d(double a, double b){
  return(-max_d(-a,-b)) ;
}


void  read_col2(double **col, char *nom, int ncol ) {

  /* the first element is the size of the table, */
  /*     i.e the number of numbers in a column   */
  
  int  i, n ;
  double n_l ;
  char *buf, *arg[10000] , *tmp ;
  FILE *f ;
  
  buf  = (char*) calloc(100001, sizeof(char) );
  
  /* Ouverture du fichier */
  f = fopen ( nom , "r" ) ;
  if (f == NULL) {
    fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", nom ) ;
    exit(EXIT_FAILURE) ;
  }
  /* (minimum) number of lines in the datafile   */  
  n_l=0 ;
  while (fgets(buf,100000,f) != NULL){
    if (  buf[0] != '*' &&  buf[0] != '#'  &&  buf[0] != '%' ) { /* printf("%g \n", n_l);  */
      n_l++  ;
    }
  }
  
  if(n_l==0){  (*col)   = calloc(( ((int)n_l )+1), sizeof(double));   (*col)[0] = 0;     }
  
  else{
    (*col)   = calloc(( ((int)n_l )+1), sizeof(double)) ;  (*col)[0] = n_l;   /*  printf("%g \n", n_l) ; */

    rewind(f) ; n=1;
    while (fgets(buf,100000,f) != NULL && n<(n_l+1)) {  
    
    if ( buf[0] != '*' &&  buf[0] != '#' &&  buf[0] != '%') {
      for ( tmp = strtok ( buf , " \t" ) , i = 0 ; 
            tmp != NULL ;
            tmp = strtok ( NULL , " \t" ) , i ++ )
	{arg[i] = tmp ;}
      arg[i] = NULL ;
      (*col)[n]=strtod(arg[ncol-1], NULL) ;
      n++ ;
    }
    } 
    fclose(f);
  }

  free(buf);
}

void read_colw2(char*** col, char *nom, int ncol ) {
  
  /* the first element is the size of the table, */
  /*     i.e the number of numbers in a column   */
  
  int  i, n ;
  double n_l ;
  char *buf, *arg[1000] , *tmp ;
  FILE *f ;

  buf  = (char*) calloc(10001, sizeof(char) );
  
  /* Ouverture du fichier */
  f = fopen ( nom , "r" ) ;
  if (f == NULL) {
    fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", nom ) ;
    exit(EXIT_FAILURE) ;
  }
  else{
    /* (minimum) number of lines in the datafile   */  
    n_l=0 ;
    while (fgets(buf,10000,f) != NULL){
      if ( buf[0] != '#' ) { /*  buf[0] != '*' AA modifie Madison, dec05 */
	n_l++  ;
      }
    } 
    
    
    rewind(f) ;    
    /* Dynamic allocation */  
    *col   = (char**) calloc(( ((int)n_l )+1), sizeof(char*)) ;
    
    (*col)[0]   = (char*) calloc(1000,sizeof(char)) ; sprintf((*col)[0],"%f",n_l);
    
    n=1;
    while (fgets(buf,10000,f) != NULL
	   && n<(n_l+1)) {
      if ( buf[0] != '#') {  /*  buf[0] != '*' AA modifie Madison, dec05  */
	for ( tmp = strtok ( buf , " \t" ) , i = 0 ;  
	      tmp != NULL ;
	      tmp = strtok ( NULL , " \t" ) , i ++ )
	  {arg[i] = tmp ;}
	arg[i] = NULL ;
	(*col)[n]   = (char*) calloc(1000,sizeof(char)) ;
	sprintf((*col)[n],"%s", arg[ncol-1] ) ;
	n++ ;
      }
    }
    fclose(f);
  }
  
  free(buf);
}

 

void read_colw2free(char** col) {
  int  i, n ;
  n = atoi(col[0]);

  for(i=0;i<=n;i++){ free(col[i]); }
  
  free(col);
}



void read_colw2car(char*** col, char *nom, int ncol, char car) {
  
  int  i, n ;
  double n_l ;
  char buf[10000], *arg[1000] , *tmp ;
  FILE *f ;

  /* Ouverture du fichier */
  f = fopen ( nom , "r" ) ;
  if (f == NULL) {
    fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", nom ) ;
    exit(EXIT_FAILURE) ;
  }
  /* (minimum) number of lines in the datafile   */  
  n_l=0 ;
  while (fgets(buf,10000,f) != NULL){
    if ( buf[0] == car ) { n_l = n_l+1  ; }
  } 
  rewind(f) ;    
  /* Dynamic allocation */  
  *col   = (char**) calloc(( ((int)n_l )+1), sizeof(char*)) ;
  /* Fill the col[] */
  (*col)[0]   = (char*) calloc(1000,sizeof(char)) ;
  sprintf((*col)[0],"%f",n_l);
  n=1;
  while (fgets(buf,10000,f) != NULL
	 && n<(n_l+1)) {
    if ( buf[0] == car) {  /*  buf[0] != '*' AA modifie Madison, dec05  */
      for ( tmp = strtok ( buf , " \t" ) , i = 0 ;  
            tmp != NULL ;
            tmp = strtok ( NULL , " \t" ) , i ++ )
	{arg[i] = tmp ;}
      arg[i] = NULL ;
      (*col)[n]   = (char*) calloc(1000,sizeof(char)) ;
      sprintf((*col)[n],"%s", arg[ncol-1] ) ;
      n++ ;
    }
  }
  fclose(f);
} 

void free_colw2(char** col) {
  int i; double n ;
  n=atof(col[0]); for(i=1; i<=n; i++){ free(col[i]);}; free(col);
}




double*  mirror_col(double *col)
{
  int n ;
  double *tmp ;
  tmp   = calloc( ((int)col[0])+1 , sizeof(double)) ;
  tmp[0]= col[0];
  for (n=1 ; n<=((int)col[0]) ; n++ ){
    tmp[n] = col[((int)col[0])+1-n] ;
  }
  free(col);
  return(tmp);
}

int max_col(double *col, int n1, int n2)
{
  int i,n ;
  double x ;
  /* case 0-0 : all the table  */
  if (n1==0 && n2==0){
    x = col[1]; n=1;
    for (i=2 ; i<=((int)col[0]) ; i++) {
      if (col[i] > x ) { n=i ; x = col[i] ;}
    }
  } 
  else {
    x = col[n1]; n=n1;
    for (i=(n1+1) ; i<=n2 && i<=((int)col[0]); i++) {
      if (col[i] > x ) { n=i ; x = col[i] ;}
    }
  }
  return(n);
}

double* abs_col(double *col)
{
  int n ;
  double *abs ;
  abs   = calloc( (((int)col[0])+1) , sizeof(double)) ;  
  abs[0]= col[0];
  for (n=1 ; n<=((int)col[0]) ; n++ ){
    abs[n] = fabs(col[n])   ;
  }
  return(abs); 
}

double*  der_col(double *colY, double *colX)
{
  int n ;
  double *der ;
  der   = calloc( ((int)colY[0]) , sizeof(double)) ;  
  der[0]= (colY[0]-1);
  for (n=1 ; n<=((int)der[0]) ; n++ ){
    der[n] = (colY[n+1]- colY[n])/(colX[n+1]-colX[n])  ;
  }
  return(der); 
}

double*  der2_col(double *colY, double *colX)
{
  int n ;
  double *der2 ;
  printf("coucoua\n");
  der2   = calloc( (-1+(int)colY[0]) , sizeof(double)) ;  
  der2[0]= (colY[0]-2);
  printf("der2[0]=%d \n", (int)der2[0]);
  for (n=1 ; n<=((int)der2[0]) ; n++ ){
    der2[n] = (  ((colY[n+2]-colY[n+1])/(colX[n+2]-colX[n+1])  
	       -   (colY[n+1]-colY[n])/(colX[n+1]-colX[n]))
	       / ( ((colX[n+2]-colX[n+1])/2)-((colX[n+1]-colX[n])/2))  ) ;
      }
  printf("coucouac\n");
  return(der2); 
}

int seek_peak(double *colY, double* colX)
{
  int n,i,n_p ;
  double s1,s2 ;
  n_p = 0; n=0 ;
  for (i=1 ; i<=(-2+(int)colY[0]) ; i++ ){
    s2 = (colY[i+2]-colY[i+1])/(colX[i+2]-colX[i+1]) ;  
    s1 = (colY[i+1]-colY[i])  /(colX[i+1]-colX[i])   ;
    if (s1>0 && s2<0){
      printf("I have found one peak point \n");
      n=i+2 ;
      n_p ++ ;
    }
  }
  if (n_p >1) {
    printf("Warning Something is not perfect\n") ;
    printf("I have found %d peak points \n",n_p) ;
  }  
  return(n) ; 
}

int read_mcol_sandwich(char *name, char* word, char *beg, char *end, char *name_t){
  
  int n ;
  char buf[10000];
  FILE *f_s, *f_t ;
  
  /* Ouverture du fichier source*/
  f_s = fopen ( name , "r" ) ;
  if (f_s == NULL) {
    fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", name ) ;
    exit(EXIT_FAILURE) ;
  }
  /* Ouverture du fichier temporaire */
  f_t = fopen ( name_t , "w" ) ;
  if ( strlen(word) <= 1) {
    printf("I seek from the beginning of the file \n")  ; 
  }  
  if ( strlen(word) > 1) {
    printf("I seek after <%s>\n", word)  ;
    while ( fgets(buf,10000,f_s) != NULL
	    && strncmp(buf,word,strlen(word))!=0) 
      {}
  }
  while (fgets(buf,10000,f_s) != NULL
	 && strncmp(buf,beg,strlen(beg))!=0 ) 
    {   
    }
  n=0 ;
  while (fgets(buf,10000,f_s) != NULL
	 && strncmp(buf,end,strlen(end))!= 0) 
    {
      /*   fprintf(stderr, "<%s>", buf ) ; */
      fprintf(f_t,"%s",buf);
      n++;
    }
  if (n==0) {
    printf("I didn't find anything between\n");
    printf("<%s> and <%s> \n", beg, end );
    printf("in <%s> \n", name);
  }
  fclose(f_s);
  fclose(f_t);
  return(n);
}

double read_word_after_vestige(char *name, char *word, char *word_before, int n ){  
  int i,j, m ;
  double res ;
  char res_word[100];
  char buf[10000];
  FILE *f;
  
  /* Ouverture du fichier source*/
  f = fopen ( name , "r" ) ;
  if (f == NULL) {
    fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", name ) ;
    exit(EXIT_FAILURE) ;
  } 

  m = 1 ;
  if ( strlen(word) <= 1) {
   /*  printf("I seek from the beginning of the file \n")  ;  */
  }  
  if ( strlen(word) > 1) {
    printf("I seek after <%s>\n", word)  ;
    while ( fgets(buf,10000,f) != NULL
	    && strncmp(buf,word,strlen(word))!=0) 
      {}
  } 
  while (fgets(buf,10000,f) != NULL
	  && strncmp(buf,word_before,strlen(word_before))!=0 )
    {}
  i=strlen(word_before) ; 
  while ( buf[i] == ' '){i++;}
  j=0 ;
  while ( buf[i] != ' '){
    res_word[j]=buf[i] ;
    i++; j++ ;
  }
  fclose(f);
  res = atof(res_word);
  return(res);
}
void read_word_after(char *name, char *word_before, char** res ){ 

  /* AA word_before doit être en début de ligne */
  int i,j ;
  char  res_word[1000], tmp[500],  line[10000], sbuf[500], labb[500] ;
  FILE *f ;
  
  j=0 ;
  for (i=0 ; i<strlen(word_before) ; i++ ){
    if (  ((int)'a'<=word_before[i]  &&  word_before[i]<=(int)'z') ||  ((int)'0'<=word_before[i]  &&  word_before[i]<=(int)'9') ){
      labb[j]=word_before[i] ;
      j++ ;
    }
  }
  labb[j]='\0' ;

  sprintf(tmp, "pmtread_word_after_%s_%d",  labb, getpid() ) ;   DEB( printf("<%s>\n", tmp) ) ;
  
  sprintf(sbuf, "grep '%s' %s > %s ", word_before, name, tmp  ) ; DEB( printf("<%s>\n", sbuf) ) ; system(sbuf) ; DEB( printf("<%s>\n", sbuf) ) ;
  
  f = fopen (tmp, "r" ) ;

  if ( fgets(line,10000,f) !=NULL){
    
    DEB( printf("<%s>", line) ) ;

    i=strlen(word_before) ; 
    j=0 ;
    
    while ( line[i] != ' ' && line[i] != '\n' && line[i] != '\t' ){
      res_word[j] = line[i] ;  DEB( printf("|%c|\n",line[i]  ); ) ;
      i++; j++ ;
    }
    
    res_word[j]='\0'; /* AA important ! */  DEB( printf("[read_word_after]<%s>\n",res_word ) ;) ;
 
     sprintf(*res,"%s",res_word);   DEB( printf("[read_word_after]<%s>\n",*res ) ;) ;

   /*  for (k=0 ; k<strlen(res_word) ; k++ ){ */
/*       *res[k]=res_word[k] ; */
/*     } */
  }
  else{ fprintf(stderr,"Je n'ai pas trouvé <%s> dans <%s> \n",word_before, name ) ; }  
  
  fclose(f) ;
 /*  sprintf(sbuf, "rm -f  %s \n", tmp  ) ;  system(sbuf) ; */
  
  DEB( printf("[read_word_after] fin\n") ;) ;
}

void read_number_after(char *name, char *word_before, double *res ){ 

  /* AA word_before doit être en début de ligne */
  int i,j ;
  char  res_word[1000], tmp[500],  line[10000], sbuf[500], labb[500] ;
  FILE *f ;
  
  j=0 ;
  for (i=0 ; i<strlen(word_before) ; i++ ){
    if (  ((int)'a'<=word_before[i]  &&  word_before[i]<=(int)'z') ||  ((int)'0'<=word_before[i]  &&  word_before[i]<=(int)'9') ){
      labb[j]=word_before[i] ;
      j++ ;
    }
  }
  labb[j]='\0' ;

  sprintf(tmp, "pmtread_word_after_%s_%d",  labb, getpid() ) ;   DEB( printf("<%s>\n", tmp) ) ;
  
  sprintf(sbuf, "grep '%s' %s > %s ", word_before, name, tmp  ) ; DEB( printf("<%s>\n", sbuf) ) ; system(sbuf) ; DEB( printf("<%s>\n", sbuf) ) ;
  
  f = fopen (tmp, "r" ) ;

  if ( fgets(line,10000,f) !=NULL){
    
    DEB( printf("<%s>", line) ) ;

    i=strlen(word_before) ; 
    j=0 ;
    
    while ( line[i] != ' ' && line[i] != '\n' && line[i] != '\t' ){
      res_word[j] = line[i] ;  DEB( printf("|%c|\n",line[i]  ); ) ;
      i++; j++ ;
    }
    
    res_word[j]='\0'; /* AA important ! */  DEB( printf("<%s>\n",res_word ) ;) ;
 
    *res = atof(res_word);
  }
  else{ fprintf(stderr,"Je n'ai pas trouvé <%s> dans <%s> \n",word_before, name ) ; }  
  
  fclose(f) ;
  sprintf(sbuf, "rm -f  %s \n", tmp  ) ;  system(sbuf) ;

  
   
}


int replace(char *name, char* old, char* new, char *name_replaced){
  int  i, l, n ;
  char* ion ;
  char  old_buf[10000] , tmp[1000];
  char* new_buf, sbuf[500];
  FILE *f, *f_s ;
 
  sprintf(sbuf,"cp %s tmpreplace\n", name) ; system(sbuf) ;
  
  /* Ouverture du fichier */
  f = fopen ("tmpreplace", "r" ) ;
  if (f == NULL) {
    fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", name ) ;
    exit(EXIT_FAILURE) ;
  }
  f_s = fopen (name_replaced , "w" ) ;
  n = 0;
  while (fgets(old_buf,10000,f) != NULL ){
    new_buf = calloc(10000,sizeof(char));
    strncpy(new_buf, old_buf, strlen(old_buf));
    while  ( strstr(new_buf, old) != NULL) {
      ion = strstr(new_buf, old) ;
      strncpy(old_buf, new_buf, strlen(new_buf));
      new_buf = calloc(10000,sizeof(char));
      l = strlen(old_buf) - strlen(ion) ;
      strncat(new_buf,old_buf,l);
      strncat(new_buf,new,strlen(new));
      for (i=0 ; i<=(strlen(ion)-strlen(old)) ; i++) {
	tmp[i]=old_buf[i+l+strlen(old)];
      }
    strncat(new_buf,tmp,strlen(tmp));
    }
    fprintf(f_s, "%s",new_buf) ; 
  }
  fclose(f);
  fclose(f_s);
  sprintf(sbuf,"rm tmpreplace\n" ) ; system(sbuf) ;
  return (n);
}

void     display_col(double *col) {
  int i ;
  printf("The number of elements is : %d \n", (int) col[0]) ;
  for (i=1 ; i<= ( (int) col[0]) ; i++) {
    printf("%e \n", col[i]);
  }
}

void     display_tab(double** tab) {
  int i,j ;
  printf("The number of rows  is : %d \n", (int) tab[0][0]) ;
  printf("%d \n", (int) tab[0][0]);
  for (i=1 ; i<= ( (int) tab[0][0]) ; i++) {
    printf("%d   ",i);
    for (j=1 ; j<=( (int) tab[i][0]) ; j++) {
      printf("%f  ", tab[i][j]);
    }
    printf("\n");
  }
}

/* fonction ensta projet Gueydan */

void envoyer_entier(FILE *f , int n) {
  char mes[10] ;
  sprintf(mes, "%d", n) ;
  fprintf(f, "%s\n", mes) ; /* chaine suivie de \n */
  fflush(f) ;
}

int  recevoir_entier(FILE *f) {
  int i ;
  char mes[10] ;
  fscanf(f , "%s", mes) ;
  i=atoi(mes) ;
  return(i) ;
}


pid_t lancerprocessus(char *arg[20], FILE **f_entree, FILE **f_sortie, int* fd_B ) {

  /* Permet, d'un processus père,  de lancer un processus en redirigeant éventuellement */
  /* son entrée et/ou  sa sortie standard dans des tuyaux également reliés au père      */
  /* on peut récupérer  a partir du dernier argument le numéro de descripteur  du coté  */
  /* du tuyau ou le père lit (ceci pour pouvoir utiliser un select)                     */
  pid_t pid ;
  int tuyau_entree[2], tuyau_sortie[2] ;
  
  /* Création éventuelles des tuyaux */
  if (*f_entree != stdin) {
    if ( pipe(tuyau_entree) == -1 ){
      perror("Erreur dans l'ouverture du tuyau d'entree ") ; 
      exit(EXIT_FAILURE) ;
    }
  }
  
  if (*f_sortie != stdout) {
    if ( pipe(tuyau_sortie) == -1 ){
      perror("Erreur dans l'ouverture du tuyau de sortie ") ; 
      exit(EXIT_FAILURE) ;
    }
  }

  pid = fork() ;
  switch(pid)
    {
    case -1 : /* erreur */
      perror( "Erreur  dans le  fork");
      exit(EXIT_FAILURE) ;
      
    case 0 :  /*processus fils */
      
       DEB ( fprintf(stderr, "[fils] Je suis le fils \n") ) ;


      /* Si l'entrée standard est redirigée */
      
      if (*f_entree != stdin) {
	/* Ouverture du tuyau entree en lecture */
	close( tuyau_entree[ECRITURE] ) ;
	  *f_entree = fdopen (tuyau_entree[LECTURE], "r") ;
	  if (  *f_entree == NULL){
	    perror("[fils] Erreur dans le fdopen de f_entree ") ; 
	    exit(EXIT_FAILURE) ;
	  }
	   DEB ( fprintf(stderr, "[fils] Ouverture du tuyau entree en lecture \n") ) ;
	
	  /* Redirection de l'entrée standard  */
	  close(STDIN_FILENO) ;
	  if ( dup2(tuyau_entree[LECTURE],STDIN_FILENO) == -1 )
	    {
	      perror("[fils] Erreur dans le dup2 du tuyau_entree ") ;
	    }
	  close(tuyau_entree[LECTURE]) ;
	  DEB ( fprintf(stderr, "[fils] Redirection de l'entrée standard (tuyau_entree) \n" ) );
	}
      
	/* Si la sortie  standard est redirigée */
      
      if (*f_sortie != stdout) {
	/* Ouverture du tuyau de sortie en écriture */
	close( tuyau_sortie[LECTURE] ) ;
	*f_sortie = fdopen (tuyau_sortie[ECRITURE], "w") ;
	if (  *f_sortie == NULL){
	  perror("[fils] Erreur dans le fdopen de f_sortie ") ; 
	  exit(EXIT_FAILURE) ;
	}
	 DEB (fprintf(stderr, "[fils] Ouverture du tuyau_sortie en écriture \n" ) ) ; 
	  
	/* Redirection la sortie standard (tuyau de sortie) */
	close(STDOUT_FILENO) ;
	if ( dup2(tuyau_sortie[ECRITURE],STDOUT_FILENO) == -1 )
	  {
	    perror("[fils] Erreur dans le dup2 du tuyau_sortie ") ;
	  }
	close(tuyau_sortie[ECRITURE]) ;
	 DEB (fprintf(stderr, "[fils] Redirection la sortie standard (tuyau_sortie) \n" ) ) ;
      }
 
      /* Recouvrement par  */
      if ( execvp(arg[0], arg) == -1 )
	{
	  perror("[fils] Erreur dans le excvp de predico3 ") ;
	  exit(EXIT_FAILURE) ;
	}
      break ;

    
    default:  /* processus père */

     
       DEB (fprintf(stderr, "[Père] je suis le père \n" ) ) ;
      
      /* Si l'entrée standard du fils  est redirigée et le père écrit dedans */
      
      if (*f_entree != stdin) {
	/* Ouverture du tuyau d'entrée  en écriture */
	close( tuyau_entree[LECTURE] ) ;
	*f_entree = fdopen (tuyau_entree[ECRITURE], "w") ;
	if (  *f_entree == NULL){
	  perror("[Père] Erreur dans le fdopen de f_entree ") ; 
	  exit(EXIT_FAILURE) ;
	}
	DEB (fprintf(stderr, "[Père] Ouverture du tuyau d'entree  en écriture \n" ) ) ;
      }

      /* Si la sortie  standard du fils  est redirigée et le père lit dedans  */
      
      if (*f_sortie != stdout) {
	/* Ouverture du tuyau B en lecture */
	close( tuyau_sortie[ECRITURE] ) ;
	*f_sortie = fdopen (tuyau_sortie[LECTURE], "r") ;
	if ( fd_B != NULL )  *fd_B = tuyau_sortie[LECTURE] ;
	if (  *f_sortie == NULL){
	  perror("[Père] Erreur dans le fdopen de f_sortie  ") ; 
	  exit(EXIT_FAILURE) ;
	}
	DEB (fprintf(stderr, "[Père] Ouverture du tuyau sortie en ecriture \n" ) ) ;
      }

      
      
    } /* fin du fork*/ 
   DEB ( fprintf(stderr, "[Père] je renvoie le pid, c'est <%d> \n", (int) pid )  );
  return (pid) ;
}
      
