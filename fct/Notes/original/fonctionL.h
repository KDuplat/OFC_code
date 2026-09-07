#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"fonction.h" 

int multi_1000(double x) ;
void   get_str_nom(char* nom, char* s1, char* s2, char* s0, char *res) ;
double get_par_nom(char* nom, char* s1, char* s2, char* s0 ) ;


/*  pour le sc */
int no_coor(int x, int y, int z, int m ) ;
void coor_no(int* px, int *py, int *pz, int no, int m) ;
/* pour le bcc */
int no_coor1a(int x, int y, int z, int m) ;
void coor_no1a(int* px, int *py, int *pz, int no, int m) ;
int no_coor2a(int x, int y, int z, int m ) ;
void coor_no2a(int* px, int *py, int *pz, int no, int m) ;

/* autres */
char** list_file(char *nom, char* carac) ;
int ncol_file(char *nom);
void moyeqm(double* moy, double* var, double* max,double* col ) ;
void moyeqm2(double* col, double* moy, double* eqm, int n1, int n2) ;
void minmaxincol(double* col, double* rmin, double* rmax,  int n1, int n2);


int s_in(char* sa, char* sb);
int  s_replace(char* sa, char* sb, char* sc) ;
void interpol_lin(double* colx, double* coly, double x, double* res ) ;
