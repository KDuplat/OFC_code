#include"../prog/fonction.h"
#include"../prog/fonctionL.h"
#include"../prog/fonctionM.h" 

#include"../fct/fct.h"

/* not necessarily updated, name functions may have changed since tested */


int main (int argc , char *argv[]) { 
  int i,a,a1,a2,a3,a4; double u; char c;  char str[_NS]; 
  int *coli; double *col; char **cols;
 
  a=0  ;               elread(&a  ,'i',"rien",""); 
  u=-1 ;               elread(&u  ,'d',"rien","");
  c='z';               elread(&c  ,'c',"rien",""); 
  sprintf(str,"ini");  elread(&str,'s',"rien","");
    
  printf("%d  %g  %c  %s  \n",a,u,c,str  ); 

  a1=colread_vl('1', 'y', "#*%", 3);
  a2=colread_vl('#', 'y', "#*%", 3);
  a3=colread_vl('*', 'y', "#*%", 3);
  a4=colread_vl('%', 'y', "#*%", 3); printf("%d %d %d %d \n",a1,a2,a3,a4);
  
  a1=colread_vl('1', 'n', "#*%", 3);
  a2=colread_vl('#', 'n', "#*%", 3);
  a3=colread_vl('*', 'n', "#*%", 3);
  a4=colread_vl('%', 'n', "#*%", 3); printf("%d %d %d %d \n",a1,a2,a3,a4);
  
  
  coldread(&col ,&a,"dat",7    ); for(i=0; i<5; i++){  printf("double %g  \n", col [i]) ;}; free(col );
  coliread(&coli,&a,"dat",1    ); for(i=0; i<5; i++){  printf("int    %d  \n", coli[i]) ;}; free(coli);
  colsread(&cols,&a,"dat",2,"*"); for(i=0; i<a; i++){  printf("stra  <%s> \n", cols[i]) ;}; colsfree(cols,a); 
  colsread(&cols,&a,"dat",2,"#"); for(i=0; i<a; i++){  printf("strb  <%s> \n", cols[i]) ;}; colsfree(cols,a); 
  
  colsreadfull(&cols, &a,"dat",7," \t",'n',"#*%",_COLREADLINEMAX);  for(i=0; i<5; i++){  printf("str <%s>  \n", cols[i]) ;}; 
  
  /*test error message */
  if(1==0){
    coldread(&col ,&a,"dat",9    ); for(i=0; i<5; i++){  printf("double %g  \n", col [i]);}; 
    colsread(&cols,&a,"dat",2,"$"); for(i=0; i<a; i++){  printf("dollar  <%s> \n", cols[i]) ;}; colsfree(cols,a); 
  }

  
  elreadposfull(&u,'d',"dat","DISC"," \t",3);

  exit(0);
}



int main (int argc , char *argv[]) { 
  int i,a;
  char **in, **out;
  
  a=5;
  
  scolalloc(&in ,a,_NS);
  scolalloc(&out,a,_NS);
  

  for(i=0; i<a; i++){ 
    sprintf(in [i],"in%d" ,i);
    sprintf(out[i],"out%d",i);
  }

  sreplacefile(a, in, out,"essaiin","essaiout");

  exit(0);
}


/**/

