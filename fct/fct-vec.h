
#define Vinit(V,a,b,c) V.x1=a;V.x2=b;V.x3=c; 
#define Vtini(V,a,b,c) a=V.x1;b=V.x2;c=V.x3; 

#define Vir(V) V.x1,V.x2,V.x3 


#define Vs(V,W) ((V.x1)*(W.x1)+(V.x2)*(W.x2)+(V.x3)*(W.x3))  
#define Vsa(V)   Vs(V,V)

#define Vpc(V) (V.x1*V.x2*V.x3) 

int rien();
