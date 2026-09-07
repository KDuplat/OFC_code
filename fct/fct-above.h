
/*____________________structures__________________*/


/* vec */
typedef struct{  int     x1; int     x2; int     x3; } ivs;  
typedef struct{  double  x1; double  x2; double  x3; }  vs; 


/* lattice */

#define LATNNEIMAX 20

typedef struct{   
  ivs  v  ;
  int nnei;
  int  nei[LATNNEIMAX];
  
  int no ;
  int ind;
  vs  vr ;
  int type;
} latpts  ;


typedef struct{   
  int a   ;
  int b   ;
  int type;
} latbds  ;


typedef struct{
  char type[10];  ivs  dim;
  
  int t   ; int unit;  ivs dimu; 
  int nbas; ivs *bas;
  int nnei; ivs *nei;
  
  int n     ; latpts *pt  ;  int atomtype;
  int nbond ; latbds *bond;  int bondtype; 
  
  int nr    ; int *cor;

  vs dec    ; vs alat;
  /* NOTE: dim : dimension in cell units                   */
  /* NOTE: dimu: dimension in unit used for pts position   */
  /* NOTE: ex: dimu = 2*dim for fcc, dimu=dim for sc       */
  /* NOTE: factor is unit                                  */
  /* So far: ...                                           */
  /* NOTE: all points have same type (atometype)           */
  /* NOTE: all bonds  have same type (bondtype)            */
} lats;


/* lammps */

typedef struct{   
  int atomtypes; int bondtypes;
  
  vs xlo; vs box;
  vs xhi;
  
  int natom; latpts *atom;            
  int nbond; latbds *bond;
} lmpdats  ;




/* lists */

typedef struct listmp{
  int    n             ;
  struct listmp *next  ;
} lis ;


typedef struct {
  int    n; double r2 ; double w;  
} lsics ;


typedef struct lisstmp{
  lsics     ic          ;
  struct lisstmp *next  ;
} lss ;


