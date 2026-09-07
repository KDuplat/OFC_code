#include<math.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>

int max_i(int a, int b) ;
int min_i(int a, int b) ;
double max_d(double a, double b) ;
double min_d(double a, double b) ;


/* double*  read_col(char *nom, int ncol ) ; unsafe */
void     read_col2(double **col, char *nom, int ncol ) ;
/* char**  read_col_word(char *nom, int ncol ) ; unsafe */
void  read_colw2(   char*** col, char *nom, int ncol ) ;
void  read_colw2car(char*** col, char *nom, int ncol, char car) ;

void read_colw2free(char** col);


void free_colw2(char** col) ;
double*  mirror_col(double *col) ;
int      max_col(double *col, int n1, int n2);
double*  abs_col(double *col);
double*  der_col(double *colY, double* colX);
double*  der2_col(double *colY, double* colX);
int      seek_peak(double *colY, double* colX);

int read_mcol_sandwich(char *name, char *word, char *beg, char *end, char *name_t);
double  read_word_after_vestige(char *name, char *word, char *word_before, int n);
void read_word_after(char *name, char *word_before, char **res ) ;
void read_number_after(char *name, char *word_before, double *res ) ;
int replace(char *name, char* old, char* new, char *name_replaced);

void     display_col(double* col)  ; 
void     display_tab(double** tab)  ; 

/* fonctions ensta projet Gueydan */

/* fonctions de  communication  */ 
void envoyer_entier(FILE *f , int n) ;     
int recevoir_entier( FILE *f) ;

/* autre */
void associer_fonction_signal( int i , struct sigaction *act,void (*traitesignal)(void) ) ;
pid_t lancerprocessus(char *arg[10], FILE **f_entree, FILE **f_sortie, int* fd_B ) ;

