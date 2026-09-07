#include"fct.h"


/*____________________________valid but not updated nor checked_________________*/


/* liste d'entiers */
void add_li(lis **list, int n){  /*add at the beginning */
  lis *new;
  
  if((new = (lis*)malloc(sizeof(lis))) == NULL) { perror("malloc failed "); exit(1); }
  new->n  =  n  ;
  new->next = *list;
  *list =new ;   
}

void del_li(lis **list, int n){ 
  lis *lcur, *lpre, *ltmp;
  
  lpre = NULL;  for(lcur=*list; (lcur!=NULL) && (lcur->n!=n); lpre=lcur,lcur=lcur->next ){ ;} ;
  
  if(lcur != NULL){
    if(lpre == NULL){  ltmp = lcur->next;   free(lcur);  *list=ltmp ; }
    else{              lpre->next = lcur->next;  free(lcur);  }
  }
  else{printf("Pb del_li  je n'ai pas trouve  %d \n", n) ; exit(1);}
}

void empty_li(lis** liste){
  lis *lcur, *ltmp ;
  lcur=*liste ;  *liste=NULL ; while(lcur!=NULL){ ltmp=lcur->next ; free(lcur); lcur=ltmp;} ; 
}

void display_li(lis* liste){
 lis *li ;
 for( li=liste; li!=NULL; li=li->next) { fprintf(stdout, "n %d \n",li->n); fflush(stdout); }
}
int cardinal_li(lis* liste){
  int i ; lis *li ; i=0 ; for( li=liste; li!=NULL; li=li->next) { i++ ; } ; return(i-1) ;
}
void file_li(lis* liste,char *buf){
  FILE *f ; lis *li ;
  f= fopen(buf,"w") ; for( li=liste; li!=NULL; li=li->next) {fprintf(f, "n %d \n",li->n) ;} ; fclose(f) ;
}
int in_li(int m, lis* liste){
  lis *lcur ; 
  for(lcur=liste; (lcur!=NULL) && (lcur->n!=m); lcur=lcur->next ){ ;} ;
  if(lcur == NULL){ return(1) ; } else            { return(0) ; }
}

/* liste de structure  */
void add_ls(lss **list, lsics ic){  /*add at the beginning */
  lss *new;
  
  if((new = (lss*)malloc(sizeof(lss))) == NULL) { perror("malloc failed "); exit(1); }
  new->ic  =  ic  ;
  new->next = *list;
  *list =new ;
}

void del_ls(lss **list, lsics ic){ 
  lss *lcur, *lpre, *ltmp;
  
  lpre = NULL;  for(lcur=*list; (lcur!=NULL) && (lcur->ic.n!=ic.n); lpre=lcur,lcur=lcur->next ){ ;} ;
  
  if(lcur != NULL){
    if(lpre == NULL){  ltmp = lcur->next;   free(lcur);  *list=ltmp ; }
    else{              lpre->next = lcur->next;  free(lcur);  }
  }
  else{printf("Pb del_li  je n'ai pas trouve  %d \n", ic.n) ; exit(1);}
}

void empty_ls(lss** liste){
  lss *lcur, *ltmp ;
  lcur=*liste ;   *liste=NULL ; while(lcur!=NULL){ ltmp=lcur->next ; free(lcur); lcur=ltmp;} ; 
}

void display_ls(lss* liste){
  lss *li ; for( li=liste; li!=NULL; li=li->next) { fprintf(stdout, "ic %d %g  \n",li->ic.n, li->ic.r2/* ,li->ic.w  */  ); fflush(stdout); }
}
int cardinal_ls(lss* liste){
  int i ; lss *li ; i=0 ; for( li=liste; li!=NULL; li=li->next) { i++ ; } ; return(i) ;
}
void file_ls(lss* liste,char *buf){
  FILE *f ; lss *li ;
  f= fopen(buf,"w") ; for( li=liste; li!=NULL; li=li->next) {fprintf(f,"ic %d %g \n",li->ic.n, li->ic.r2/* ,li->ic.w*/);} ; fclose(f) ;
}
int in_ls(lsics ic, lss* liste){
  lss *lcur ; 
  for(lcur=liste; (lcur!=NULL) && (lcur->ic.n!=ic.n); lcur=lcur->next ){ ;} ;
  if(lcur == NULL){ return(1) ; } else             { return(0) ; }
}
