#include"fct.h"


void lmpdat_defaultvalues(lmpdats *lmpdato){
  
  lmpdato->atomtypes=1;  lmpdato->xlo°=0.; 
  lmpdato->bondtypes=1;  lmpdato->xhi°=0.;
  
  lmpdato->natom=1; lmpdato->atom=NULL;            
  lmpdato->nbond=1; lmpdato->bond=NULL;
}


void lmpdat_prfull(lmpdats *lmpdato,   FILE *fo){
  int i;
  vs xlo, xhi; latpts *pt; latbds *bd;

  xlo°=(lmpdato->xlo)°;  
  xhi°=(lmpdato->xhi)°;
  
  fprintf(fo,"# lmpdat_prfull \n\n");
  
  fprintf(fo,"%d\t atoms\n", lmpdato->natom);
  fprintf(fo,"%d\t bonds\n", lmpdato->nbond);
  
  fprintf(fo,"%d\t atom types\n" , lmpdato->atomtypes);
  fprintf(fo,"%d\t bond types\n\n",lmpdato->bondtypes);
 
  fprintf(fo,"%g %g xlo xhi\n", xlo.x1, xhi.x1);
  fprintf(fo,"%g %g ylo yhi\n", xlo.x2, xhi.x2);
  fprintf(fo,"%g %g zlo zhi\n", xlo.x3, xhi.x3);
	  
  fprintf(fo,"\nAtoms\n\n");
  
  for (i=0; i<lmpdato->natom; i++) {
    
    pt = &( (lmpdato->atom)[i] ); fprintf(fo,"%d 0 %d %g %g %g \n",pt->no+1, pt->type, Vir(pt->vr) );
  }
  
  fprintf(fo,"\nBonds\n\n");
 
  for (i=0; i<lmpdato->nbond; i++) { 
    
    bd = &( (lmpdato->bond)[i] );  fprintf(fo,"%d %d %d %d\n", i+1, bd->type, bd->a+1, bd->b+1); 
  } 
  
}

void lmpdat_prf(lmpdats *lmpdato, char *filo){
  FILE *f;
  
  f=fopen(filo,"w");   lmpdat_prfull(lmpdato, f);    fclose(f);
}


void lmpdat_prrasmol(lmpdats *lmpdato,   char *filo){
  int i;  latpts *pt; FILE *f;

  f=fopen(filo,"w");  fprintf(f,"%d\n\n", lmpdato->natom);
  
  for (i=0; i<lmpdato->natom; i++) {
    pt = &( (lmpdato->atom)[i] ); fprintf(f,"%d %g %g %g \n",pt->type, Vir(pt->vr) );
  }
  
  fclose(f);
}




/* distance in space, fast and dirty       */
/* points are assumed to be inside the box, see lattice */



inline double bcvec(double ao, double no){
  double r; double mino; 
  
  mino=0.5*no; r=ao; if      (ao<-mino){ r += no;}
  /**/               else{ if(ao> mino){ r -= no;}; }; return(r);
}

void vec(vs *vo, vs vao, vs vbo, vs vno){
  vs v;
  v°=vbo°-vao°;  (*vo)°=bcvec(v°,vno°);
}

inline double dis2(vs vao, vs vbo, vs vno){ vs v; vec(&v,vao,vbo,vno); return(Vsa(v));}

void lmp_checkbondlength(lmpdats *lmpdato){ 
  int i,a,b; double r,min,max; vs va,vb,box; latbds *bd; 

  min=1e10; max=0;
  

  box° = lmpdato->xhi° - lmpdato->xlo°;

  for(i=0; i<lmpdato->nbond; i++){ 
    
    bd=&(lmpdato->bond[i])  ; 
    
    a=bd->a; va=lmpdato->atom[a].vr;
    b=bd->b; vb=lmpdato->atom[b].vr;
    
    r=sqrt(dis2(va,vb,box));  min=dmin(r,min);  max=dmax(r,max);  /* printf("%d  %d %d   %g \n", i, a,b,r); */
  }    

  printf("#lmp_checkbondlength:%g %g \n", min,max);
}
