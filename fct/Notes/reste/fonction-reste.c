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

