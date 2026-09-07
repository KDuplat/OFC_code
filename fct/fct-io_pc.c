#include"fct.h"


inline int fopener(FILE **fo, char* namo , char* modo){
  *fo = fopen (namo, modo ) ;  if (*fo == NULL) { fprintf(stderr, "# fopener: Failed to open  <%s> \n", namo); return(-1);}
  /**/                         else             {                                                              return( 1);}
}


inline int elread(void* ro, char to, char *namo, char *ero){
  char str[_NS]; FILE *f;
  
  if( fopener(&f,namo,"r")==-1 ){ return(-1);}
  else                          {
    
    if(fgets(str,_NS,f)==NULL){ fprintf(stderr,"#elread: nothing to read in %s (%s)\n",namo,ero); return(-1); }  
    
    switch(to){
    case 'i': sscanf (str,"%d" , (int   *)ro);   break ;
    case 'd': sscanf (str,"%lg", (double*)ro);   break ;
    case 'c': sscanf (str,"%c" , (char  *)ro);   break ;
    case 's': sprintf((char  *)ro, "%s", str);   break ;
    default : fprintf(stderr,"#elread\n");  exit(EXIT_FAILURE);  
    }  
    fclose(f); return(1);
  }
}   

inline void discsys(char *ro, char *baso){time_t  t;  (void) time(&t);  sprintf(ro,"tttmmmppp_%s_%d_%d_%d",  baso,  getpid(),  (int)t, rand()); }

inline void rmsys (char *srco){ char sys[_NS];   sprintf(sys,"rm -f %s ", srco);   system(sys);}

inline void mrsys(char *srco, char *labelo, char *syso){
  char str[_NS];
  discsys(srco,labelo);  sprintf(str,"%s > %s ", syso,srco); system(str);    /*printf("#rmsys: <%s>\n\n",str);*/
}

inline int ressys    (void* ro, char to, char *syso    ){int a; char res[_NS];  mrsys(res,"ressys"    ,syso);   a=elread(ro,to,res,syso)     ; rmsys(res); return(a);}
inline int scolressys(char ***rso, int *nlo, char *syso){int a; char res[_NS];  mrsys(res,"scolressys",syso);   a=scolread(rso,nlo,res,1,"."); rmsys(res); return(a);}



inline int listfile(char ***rso, int *nlo, char *srco){  /* printf("%s\n",sys); */
  int a; char sys[_NS],dir[_NS],fil[_NS]; 
  
  dirfil(dir,fil,srco);
  sprintf(sys,"find %s -name \"%s\" -print  | sort -u  ", dir, fil);  a=scolressys(rso,nlo,sys);  return(a);
  /* future: to get files that contain a specific chain */
  /* sprintf(sys,"find %s -name \"%s\" -print -exec grep -L \"%s\" {} \\; | cat | sort -u  ", dir, fil, "[.*]");    scolressys(rso,nlo,sys); */
}

/*   */

inline int elreadposfull(void* ro, char to, char *namo, char *disco, char *separo, int ncolo){
  int a; char sys[_NS],res[_NS]; 

  sprintf(res, "tmpresreadposfull_%d", getpid());     /* printf("%s   a=%d \n",sys, a); */

  sprintf(sys,"awk '/%s/{print $0 > \"%s\"; n++}; END {print n+0}' %s", disco, res, namo);  ressys(&a,'i',sys);
  
  if(a<=0){ fprintf(stderr, "#elreadposfull: no (%d)      line  with <%s> in <%s> \n", a, disco,namo); return(-1); }
  if(2<=a){ fprintf(stderr, "#elreadposfull: several (%d) lines with <%s> in <%s> \n", a, disco,namo); return(-1); }
  
  sprintf(sys,"awk -F\"[%s]*\" '{print NF}' %s ",separo,res);   ressys(&a,'i',sys);
  
  if(a<ncolo){ fprintf(stderr, "#elreadposfull: field number is above max (%d>%d) (%s,%s)\n", ncolo,a,namo,disco);  return(-1);    }

  sprintf(sys,"awk -F\"[%s]*\" '{print $%d}' %s ",separo,ncolo,res);   ressys(ro,to,sys);
  
  sprintf(sys,"rm -f %s ", res);   system(sys);    return(1);
}    
    

inline int ireadpos(int   * ro, char *namo, char *disco, int ncolo){ int a; a=elreadposfull(ro,'i',namo,disco," \t",ncolo); return(a); }
inline int dreadpos(double* ro, char *namo, char *disco, int ncolo){ int a; a=elreadposfull(ro,'d',namo,disco," \t",ncolo); return(a); }
inline int sreadpos(char  * ro, char *namo, char *disco, int ncolo){ int a; a=elreadposfull(ro,'s',namo,disco," \t",ncolo);  ro[strlen(ro)-1]='\0'; return(a);}

inline int ireadpossys(int   * ro, char *syso, int ncolo){ int a; char res[_NS]; mrsys(res,"ireadpossys",syso); a=elreadposfull(ro,'i',res,".*"," \t",ncolo); rmsys(res);                       return(a); }
inline int dreadpossys(double* ro, char *syso, int ncolo){ int a; char res[_NS]; mrsys(res,"dreadpossys",syso); a=elreadposfull(ro,'d',res,".*"," \t",ncolo); rmsys(res);                       return(a); }
inline int sreadpossys(char  * ro, char *syso, int ncolo){ int a; char res[_NS]; mrsys(res,"sreadpossys",syso); a=elreadposfull(ro,'s',res,".*"," \t",ncolo); rmsys(res); ro[strlen(ro)-1]='\0'; return(a);}



/* columns of data */

inline int colreadfull_vl(char co, char modo, char *caro, int lo){
  /* test whether the first character of the line is valid (and the  line acceptable) */
  /* put 'y','.' to take any line  */
  int i, ind;   ind=0; i=0;

  switch(modo){ 
  case 'y': ind=0; if(caro[0]=='.'){return(1);}; while(ind==0 && i<lo){ if(caro[i]==co){ind=1;}; i++;};   break ; 
  case 'n': ind=1;                               while(ind==1 && i<lo){ if(caro[i]==co){ind=0;}; i++;};   break ;      
  }
  return(ind);
}

int  colreadpre(long int *nlo, int *nfo,  char *namo, char *separo, char modo, char *caro, int linemaxo) {
  int  i, a,nf,ncar;  long int  nl;
  char *str, *buf;   FILE *f ;  
  
  ncar=strlen(caro);  buf=(char*) calloc(linemaxo, sizeof(char)); 
  
  if( fopener(&f,namo,"r")==-1){ fprintf(stderr,"#colreadpre: file <%s> not found \n", namo); return(-1);   } 
  
  /* number of valid lines*/  
  nl=0 ;
  while( fgets(buf,linemaxo,f) != NULL){
    if ( colreadfull_vl(buf[0],modo,caro,ncar)==1) { nl++; }
  }
  *nlo=nl;  
  
  if(nl==0){  fprintf(stderr,"#colreadpre: no valid line in <%s> ('%c',%s) \n", namo,modo,caro);  return(-1);  }
  else     {    /* printf("#nl=%ld \n", nl);   */
    
    /*check line length and number of fields (first valid line)*/
    rewind(f); i=0;
    while( i==0 && fgets(buf,linemaxo,f) != NULL){
      if ( colreadfull_vl(buf[0],modo,caro,ncar)==1) { i++; };   
    } /* printf("#firstvalidline=<%s>\n", buf); */   
    
    a=strlen(buf);   if( linemaxo-10<=a ){ fprintf(stderr,"#colreadfull: the line might be too short (%d,%d) \n", linemaxo,a); return(-1); }  
    
    nf=0;
    for ( str  = strtok(buf, separo) , i=0 ; 
	  str != NULL                      ;
	  str  = strtok(NULL,separo) , i++ ){
      nf++;  /* printf("<%s> ",str) ;  */
    }
    *nfo=nf;  fclose(f);  return(1);
  }

}


int  colreadfull(int **rio, double **rdo, char ***rso, char **rco, char to, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo) {
  int  i, n,a,nf,ncar;  long int  nl;
  char *str, *buf;   FILE *f        ;      /* *rio=NULL;  *rdo=NULL; **rso=NULL; *rco=NULL; */
  
  ncar=strlen(caro);  buf=(char*) calloc(linemaxo, sizeof(char)); 

  if( colreadpre(&nl, &nf, namo,separo,modo,caro,linemaxo)!=1){ return(-1);}
  else {
    
    switch(to){
    case 'i': *rio = (int   *) calloc(nl, sizeof(int   ));  break;
    case 'd': *rdo = (double*) calloc(nl, sizeof(double));  break;
    case 's': *rso = (char** ) calloc(nl, sizeof(char*) );  break;
    case 'c': *rco = (char* )  calloc(nl, sizeof(char)  );  break;
    default : fprintf(stderr,"#colreadfull:switch 1\n");  exit(EXIT_FAILURE);  
    }  
    
    fopener(&f,namo,"r");  *nlo=nl; n=0;
    
    while (n<nl && fgets(buf,linemaxo,f) != NULL) {  
      if ( colreadfull_vl(buf[0],modo,caro,ncar)==1) {   
	for ( str  = strtok(buf, separo) , i=0      ; 
	      str != NULL               && i<ncolo-1;
	      str  = strtok(NULL,separo) , i++      ){
	   /* printf("<%s> ",str) ;  */
	}
	
	switch(to){
	case 'i': (*rio)[n]=atoi(str); break;
	case 'd': (*rdo)[n]=atof(str); break;
	case 'c': (*rco)[n]=str[0]   ; break;
	case 's': 
	  /**/    a=strlen(str);  if( str[a-1]=='\n'){ str[a-1]='\0'; };     
	  /**/    (*rso)[n] = (char*) calloc(a+1,sizeof(char)); 
	  /**/    sprintf((*rso)[n],"%s",str);  break ; 
	default : fprintf(stderr,"#colreadfull:switch 2\n");  exit(EXIT_FAILURE);  
	}  
	n++ ;
      }
    } 
    
    fclose(f);  free(buf); return(1);
  }/*colreadpre==1*/
}


inline int icolreadfull(   int **rio, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo) {                   double **rd=NULL; char ***rs=NULL; char **rc=NULL; return( colreadfull(rio,rd,rs,rc,'i',nlo,namo,ncolo,separo,modo,caro, linemaxo)); }  
inline int dcolreadfull(double **rdo, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo) { int    **ri=NULL;                   char ***rs=NULL; char **rc=NULL; return( colreadfull(ri,rdo,rs,rc,'d',nlo,namo,ncolo,separo,modo,caro, linemaxo)); }  
inline int scolreadfull( char ***rso, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo) { int    **ri=NULL; double **rd=NULL;                  char **rc=NULL; return( colreadfull(ri,rd,rso,rc,'s',nlo,namo,ncolo,separo,modo,caro, linemaxo)); }  
inline int ccolreadfull( char  **rco, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo) { int    **ri=NULL; double **rd=NULL; char ***rs=NULL;               ; return( colreadfull(ri,rd,rs,rco,'c',nlo,namo,ncolo,separo,modo,caro, linemaxo)); }  


inline int icolread(int    **rio, int *nlo, char *namo, int ncolo            ){ return(icolreadfull(rio,nlo, namo,ncolo," \t",'n',"#*%",_COLREADLINEMAX));}
inline int dcolread(double **rdo, int *nlo, char *namo, int ncolo            ){ return(dcolreadfull(rdo,nlo, namo,ncolo," \t",'n',"#*%",_COLREADLINEMAX));}
inline int scolread(char  ***rso, int *nlo, char *namo, int ncolo, char *caro){ return(scolreadfull(rso,nlo, namo,ncolo," \t",'y', caro,_COLREADLINEMAX));}
inline int ccolread(char   **rco, int *nlo, char *namo, int ncolo            ){ return(ccolreadfull(rco,nlo, namo,ncolo," \t",'n',"#*%",_COLREADLINEMAX));}

inline void icolprfull(int    *rio, int nlo, FILE *fo, char *pro) { int i; for(i=0; i<nlo; i++){  fprintf(fo,"%s%d\n", pro, rio[i]) ;}; }
inline void dcolprfull(double *rdo, int nlo, FILE *fo, char *pro) { int i; for(i=0; i<nlo; i++){  fprintf(fo,"%s%g\n", pro, rdo[i]) ;}; }
inline void scolprfull(char  **rso, int nlo, FILE *fo, char *pro) { int i; for(i=0; i<nlo; i++){  fprintf(fo,"%s%s\n", pro, rso[i]) ;}; }
inline void ccolprfull(char   *rco, int nlo, FILE *fo, char *pro) { int i; for(i=0; i<nlo; i++){  fprintf(fo,"%s%c\n", pro, rco[i]) ;}; }


inline void icolpr(int    *rio, int nlo){  icolprfull(rio,nlo,stdout,""); }
inline void dcolpr(double *rdo, int nlo){  dcolprfull(rdo,nlo,stdout,""); }
inline void scolpr(char  **rso, int nlo){  scolprfull(rso,nlo,stdout,""); }
inline void ccolpr(char   *rco, int nlo){  ccolprfull(rco,nlo,stdout,""); }



int dcolsavemfull(int ntabo, double **tabo, int no, char *namo, char *como, int indpreco){
  int i,j; double u; FILE *f;
  
  if( fopener(&f,namo,"w") == -1){ return(-1);}
  else                           { fprintf(f,"%s", como);    
    for(i=0; i<no; i++){ 
      for(j=0; j<ntabo; j++){   /* printf("%d %d  %d %d   \n",i,j, no, ntabo);  */

	if(tabo[j]!=NULL){ u=tabo[j][i] ;}
	else             { printf("ici\n"); u=-0.        ;};  /* printf("%d %d  %d %d  %g \n",i,j, no, ntabo, tabo[j][i]);  */
 
	if(indpreco!=12){ fprintf(f,"%.12g ",u); }
	else            { fprintf(f,"%g "   ,u); }
	
      } 
      fprintf(f,"\n");
    }
    fclose(f); return(1);
  }
}


int dcolsavem(int ntabo, double **tabo, int no, char *namo, char *como){
  int a; a=dcolsavemfull(ntabo,tabo, no,namo, como,-1); return(a);
}


int ncolfile(char *namo){
  int nf; long int nl; 
  if   ( colreadpre(&nl, &nf, namo," \t",'n',"#*%",_COLREADLINEMAX)==1){ return(nf);}
  else                                                                 { return(-1);}
}



/*_______________valid but not updated nor checked__________________________*/


void copysmallfile(char *filea, char *fileb){ /* when a system call is impossible (condor) */
  int i,nl,nw;  char **tmp, *line; FILE *f ;
  
  f=fopen(filea,"r");

  if ( f == NULL) {   fprintf(stderr, "Impossible d'ouvrir le fichier <%s> \n", filea ); exit(EXIT_FAILURE) ; }
  else{
    
    nw=5000;  line=(char*) calloc(nw+1, sizeof(char)); 

    nl=0 ; while (fgets(line,nw,f) != NULL){ nl++ ;};
    
    tmp=(char**) calloc(nl+1, sizeof(char*));   for(i=0;i<=nl;i++){ tmp[i]=(char*) calloc(nw+1, sizeof(char));} ; 
    
    rewind(f); i=0 ; while (fgets(tmp[i],nw,f) != NULL){ i++ ; } ; fclose(f);
     
    f=fopen(fileb,"w"); for(i=0 ; i<=nl; i++){ fprintf(f,"%s", tmp[i]);};  fclose(f);
    
    for(i=0; i<=nl; i++){  free(tmp[i]);}; free(tmp);    free(line);
  }
  
}





void writebuffer(char *buf, int *lcur, int lmax, FILE* f, char *string){
  int l,ls ;
  
   
  ls = strlen(string) ;    l=*lcur+ls+10 ; /*"securite"*/
  
  if(lmax<=l){ 
    fprintf(f,"%s%s",buf,string) ;  fflush(f);    buf[0]='\0';  
  }else{
    /* strncat(buf,string,lmax- (*lcur) -1);  */ 
    zstrncat(buf, string, lmax ); 
  }
  *lcur=strlen(buf);  
}



