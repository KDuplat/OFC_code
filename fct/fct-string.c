#include"fct.h"


/*operation on strings only, from general to more specialized */

inline void scolalloc(char ***ro, int no, int nso){
  int i;
  
  *ro = (char** ) calloc(no, sizeof(char*) );
  
  for(i=0; i<no; i++){
    (*ro)[i] = (char*) calloc(nso,sizeof(char)); 
  }
}

inline void scolfree(char** so,int no) {
  int i; 
  for(i=0; i<no; i++){ free(so[i]);}; if(0<no){free(so);}
}


inline void strmncat(char *reso, int maxo, char* srco){
  int lr,ls,  a;
  lr=strlen(reso);
  ls=strlen(srco); a=ls+lr+1;

  if(a<maxo){ strncat(reso,srco,ls); }
  else{   
    fprintf(stderr,"#strmncat: the destination (%s) string is too short (%d=>%d) \n", reso, a, maxo); exit(EXIT_FAILURE); 
  }  
}


inline void dirfil(char *diro, char *filo, char *srco){
  /* Identify directory and file name in a complete file name */
  int i,l;

  l=strlen(srco);    i=l-1; while(1<=i && srco[i]!='/') { i-- ;}
  
  if (i==0){  sprintf(diro,"./"       ) ;
    /**/      sprintf(filo ,"%s", srco) ;
  }
  else{       strncpy(diro,  srco    , i  );  diro[i]='\0';
    /**/      strncpy(filo, &srco[i+1],l-i); 
  }
  /* fprintf(stderr,"dirfil: <%s><%s>\n", diro, filo);   */
}


int sreplace(char* srco, char* ro, char* no, char *reso){
  int i, inc, ls,lr, c ;
  char str[_NL], buf[_NS] ;

  ls = strlen(srco) ;    
  lr = strlen(ro  ) ;   i = 0; str[0]='\0'; c=0;
  
  while ( i<ls){
    if ( strncmp(&srco[i],ro,lr)==0 ){ sprintf(buf,"%s",no     ); inc=lr; c++; }
    else                             { sprintf(buf,"%c",srco[i]); inc=1 ;      };  /* printf("%d/%d: %s \n", i,ls, buf);   */
    
    strmncat(str,_NL,buf);  i += inc;
  }
  
  sprintf(reso,"%s",str); return(c);
}



int  sgetbetween(char* nom, char* s1, char* s2, char* s0, char *reso) {
  /* original  get_str_nom */
  /* Si strlen(s0)==0 par exemple s0="",      */
  /*     on cherche la dernièrere occurence de s1 dans nom, puis après s1, la première occurence de s2 */
  /*     on écrit dans reso la chaine qui se trouve entre s1 et s2. */
  /* Si strlen(s0)!=0 , */
  /*     on cherche la dernièree occurence de s0 dans s1, puis après celle-ci, la première occurence de s1,  */
  /*     et à la suite de celle-ci, la première occurence de s2. */
  /*     on écrit dans reso... */
  /* Dans les deux cas, si s2 est vide (ie ""), on prend le nombre entre s1 et la fin de la chaine nom. */

  int i,j,k, len;  char* buf ;
  
  if ( strlen(s0)==0 ){  
    i=strlen(nom) ; 
    while ( strncmp(&nom[i], s1, strlen(s1))!=0 ) { i-- ; }  ;
  }
  else{
    i=strlen(nom); while ( strncmp(&nom[i], s0, strlen(s0))!=0 ) { i-- ; }  ;
    i=i+strlen(s0) ;
    while ( strncmp(&nom[i], s1, strlen(s1))!=0 ) { i++ ; }  ;
  }
  if (i==-1) { fprintf(stderr,"#sgetbetween: je n'ai pas trouvé  <%s> dans <%s> \n", s0, nom); return(-1); }
  
  if ( strlen(s2)==0 ) {
    j=strlen(nom)+1 ;
  }else{
    j=i ; 
    while ( strncmp(&nom[j], s2, strlen(s2))!=0 ) { j++ ; }  ;
    if (strlen(nom)<=j){ 
      fprintf(stderr,"#sgetbetween:je n'ai pas trouvé  <%s> dans <%s> après la dernière occurence de <%s>\n", s2, nom ,s0); return(-1);}
  }
  
  if ( (j-i-strlen(s1))<=0 ){
    fprintf(stderr,"#sgetbetween: je n'ai rien trouvé entre <%s> et <%s> dans <%s> après la dernière occurence de <%s>\n",s1,s2, nom,s0);  return(-1);  
  }

  
  len=(j-i-strlen(s1)); buf = (char*) calloc( len+1, sizeof(char));
  
  for (k=(i+strlen(s1)) ; k<=(j-1) ; k++) {buf[k-(i+strlen(s1))] = nom[k] ;} ;  buf[len]='\0';
  
  sprintf(reso,"%s", buf);  free(buf); return(1);
}

inline int  igetbetween(char* nom, char* s1, char* s2, char* s0, int   *reso) {
  char buf[_NS];
  if( sgetbetween(nom,s1, s2,s0,buf)==1 ){ *reso=atoi(buf); return( 1);}
  else                                   {                  return(-1);}
}
inline int  dgetbetween(char* nom, char* s1, char* s2, char* s0, double *reso) {
  char buf[_NS];
  if( sgetbetween(nom,s1, s2,s0,buf)==1 ){ *reso=atof(buf); return( 1);}
  else                                   {                  return(-1);}
}
 

  
/*operation involving strings and other objects */

void sreplacefile(int no, char **outo, char **ino, char *scro, char *reso){
  /* Better than sed because can  include blanck space (and probably other special caracters) without any problem */
  /* To include a carriage return: "rien\\netnouvelleligne" */
  int i; char bufi[_NL],bufo[_NL],str[_NL],res[_NL],sys[_NL];

  discsys(res,"sreplacefile"); sys[0]='\0';

  for(i=0; i<no; i++){  
    
    sreplace(outo[i],"*","\\*",bufo);
    sreplace(bufo   ,"+","\\+",bufo);
    sreplace(bufo   ,"/","\\/",bufo);
    sreplace(bufo   ,"(","\\(",bufo);
    sreplace(bufo   ,")","\\)",bufo);
    sreplace(bufo   ,"[","\\[",bufo);
    sreplace(bufo   ,"]","\\]",bufo);   
    
    sreplace(ino[i],"&","\\\\&",bufi);       /* printf("#sreplacefile: %d %d    <%s> <%s> \n", i, no, bufo,bufi);  */
    
    sprintf(str,"awk  '{gsub(/%s/,\"%s\")}; {print}'  ", bufo,bufi);    strmncat(sys,_NL,str); 
    
    if(i==0           ){ strmncat(sys,_NL,scro );}
    if(2<=no && i<no-1){ strmncat(sys,_NL," | ");}
  }
  
  sprintf(str," > %s; mv %s  %s; ", res, res, reso); strmncat(sys,_NL,str);     /* printf("%s \n", sys);   */

  system(sys); rmsys(res); 
}


void sreplacefileone(char *outo, char *ino, char *scro, char *reso){
  char **in, **out;
  scolalloc(&in ,1,_NS); sprintf(in [0],"%s",ino );
  scolalloc(&out,1,_NS); sprintf(out[0],"%s",outo); sreplacefile(1,out,in,scro,reso);
  scolfree(in ,1);
  scolfree(out,1);
}




/* ___________________valid but not updated nor checked __________________________________*/


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


void ssplit(char *srco, char *separo, char ***reso, int *no){
  /* To revise, I am not sure it works when separo is located at the end or the beginning of srco */
  int i,n; char *w,tmp[_NL]; 
  
  if(_NL+5<strlen(srco)){ 
    fprintf(stderr,"#ssplit: function split %d<strlen(srco) word  too long (treating <%s>)\n",_NL,srco) ; exit(0) ; }
  else {
    
    sprintf(tmp,"%s",srco); w = strtok(tmp,separo); n=0 ; while (w != NULL){ n++; w=strtok(NULL, separo);} ; *no=n;
    
    *reso = (char**) calloc( n, sizeof(char*) ) ; 
    
    sprintf(tmp,"%s",srco) ;
    
    sprintf(tmp,"%s",srco) ;
    w = strtok(tmp,separo);
    i = 0 ;
    while (w != NULL){
      i++;
      (*reso)[i]= (char*) calloc(strlen(w)+1, sizeof(char) );  sprintf((*reso)[i],"%s",w) ;
      w = strtok (NULL, separo) ;
    }
  }
 
}

void ssplitfree(char **reso, int no){ int i;  for (i=0; i<no; i++){ free(reso[i]);}; free(reso);}


void removestarcolword(char **colw){
  int l,i ;   char buf[1000];            /* printf("%d \n", atoi(colw[0]) ); */

  for (l=1 ; l<=atoi(colw[0]) ; l++ ) {   

    if (colw[l][0]=='*'){
      i = 1 ; while(colw[l][i]!='*'){ buf[i-1]=colw[l][i]; i++ ; } ; buf[i-1]='\0' ; sprintf(colw[l],"%s",buf); /*  fprintf(stderr,"ap <%s>\n",buf);   */
    }}
}

int Nfromword(char *word){ 
  int i,res, npart, nnumber; char **part, **number;
  
  ssplit(word   ,"-"   ,&part  ,&npart  ); 
  ssplit(part[1],"ABCD",&number,&nnumber);

  res=0 ; for(i=0 ; i<nnumber; i++){ res += atoi(number[i]) ;};  ssplitfree(part,npart);  ssplitfree(number,nnumber); return(res);
}


void writestarword_m(FILE *f, double a[3][3]) {
  fprintf(f,"*%g,%g,%g,%g,%g,%g,%g,%g,%g*\n", a[0][0],a[1][0],a[2][0], a[0][1],a[1][1],a[2][1], a[0][2],a[1][2],a[2][2]) ;  
}


char *zstrncat( char *dst, const char *src, size_t len ){
	char *dp = dst;
	const char *sp = src;
	int cc;
	
	if (len == 0) return(dst);
	
	len--;	
	/* 	 Locate the end of the current string. */
	cc = 0; while ((*dp)&&(cc < len)) { dp++; cc++; }
	/* 	 If we have no more buffer space, then return the destination */
	if (cc >= len) return(dst);
	/* 	 While we have more source, and there's more char space left in the buffer */
	while ((*sp)&&(cc < len)){
	  cc++; *dp = *sp;  dp++; sp++;
	}
	/*  Terminate dst, as a gaurantee of string ending. */  
	*dp = '\0';
	
	return(dst);
}

