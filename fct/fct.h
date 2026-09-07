#include<limits.h>
#include<math.h>
#include<signal.h>
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>
#include<unistd.h>


/* #define DEBUG   */

#ifdef DEBUG
#define DEB(_x_) _x_
#else
#define DEB(_x_) 
#endif


#define _PI  3.14159265359

#define LECTURE  0 
#define ECRITURE 1

#define _NS 1000 
#define _NL 100000 
#define _COLREADLINEMAX 100000


#define STRCMP(A,B) (strncmp(A,B,imax(strlen(A),strlen(B)))==0) 


#include"fct-above.h"
#include"fct-cal.h"
#include"fct-clu.h"
#include"fct-fnum.h"
#include"fct-hist.h"
#include"fct-io.h"
#include"fct-lmp.h" 
#include"fct-lattice.h" 
#include"fct-list.h" 
#include"fct-matrix.h" 
#include"fct-nbi.h" 
#include"fct-process.h" 
#include"fct-rand.h" 
#include"fct-string.h" 
#include"fct-vec.h" 


/* Global variables */
char GBfct_compare_getbetween_s1[_NL];
char GBfct_compare_getbetween_s2[_NL];
char GBfct_compare_getbetween_s0[_NL];
