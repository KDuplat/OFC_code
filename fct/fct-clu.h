/* Functions for cluster analysis */
/* inspired by archive-madison/alter/rog/gorp/clu.c but written as more portable, hopefully */
/* remarque :  tab_spar, les particules du cluster p vont de l'indice  tab_sclu[p].loc  à  tab_sclu[p].loc+tab_sclu[i].size-1   */

#define _CLUNCMAX 24

/* structures  */
typedef struct{ int no, lab ;} spar ;  

typedef struct{ int  lab, size,loc; } sclu ;

typedef struct{ 
  int nt,nv, n,nc; int  *data,*label,*class, **nlist;  spar* tab_spar ;  sclu* tab_sclu ;} sclua;

void cluster_allocation(sclua *clua);
int compare_spar_lab(const void *spara, const void *sparb); 
int compare_sclu_size(const void *sclua, const void *sclub); 
int cluster_label(sclua *clua);
int cluster_analyse(sclua *clua);

void cluster_listpart(sclua clua); 

