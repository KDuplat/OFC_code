

/* colreadcar: I stopped at this stage because it is already apparent that  this function is slower than read_col2/colreadfull (factor 10 or more) */
/* on big files (100M), presumably because of the awk command */
/* still awk command  could be useful for small files  */

inline void colreadcar_treatcar(char *reso, char *caro, char *nocaro){
  int a,b,ind; 
  /**/                       ind=0; sprintf(reso,"^.")              ;
  a=strlen(  caro); if(0<a){ ind++; sprintf(reso,"^[%s]"   ,  caro); }
  b=strlen(nocaro); if(0<b){ ind++; sprintf(reso,"^([^%s])",nocaro); }; 
  
  if(ind==2){fprintf(stderr,"#colreadcar_treatcar: option not available\n");  exit(EXIT_FAILURE);}
}


int ncolreadcar(char *filo, char *caro, char *nocaro){
  int res; char disc[_NS],str[_NS];
  
  colreadcar_treatcar(disc,caro,nocaro);
  
  sprintf(str,"awk  '/%s/  {print NF;exit}'  %s  ", disc, filo);  ressys(&res,'i',str);   return(res);
}


void colreadcar(void* ro, char to, char *filo, int colo, char *caro, char *nocaro){
  int a; char name[_NS],disc[_NS],sys[_NS];

  a=ncolreadcar(filo,caro,nocaro);  if(a<colo){ fprintf(stderr,"#colreadcar: ncol<colo (%d,%d\n",a,colo);  exit(EXIT_FAILURE);};
  
  colreadcar_treatcar(disc,caro,nocaro);

  sprintf(name, "tmpcolreadcar_%d", getpid());
  
  sprintf(sys,"awk -F\"[\\t ]*\"  '/%s/ {print $%d}'   %s  > %s", disc, colo, filo, name); system(sys);
  printf("%s\n",sys);
}

