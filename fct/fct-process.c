#include"fct.h"


/* New functions 2010 May */

int pidcommand(int *pido,char *stro){
  int a; char sys[_NS],res[_NS];
  
  *pido=-1;       mrsys(res,"pidcommand","ps aux");  


  sprintf(sys," sed  -n '/[0-9][0-9][\\ ]*%s$/p' %s",stro,res);  /* printf("<%s>\n",sys); exit(0); */

  a=ireadpossys(pido,sys,2);   rmsys(res); 

  if(a==-1 || *pido<0) { fprintf(stderr, "#proc_pidcommand: could not find the pid for <%s> \n",stro) ;};

  return(a);
}


void syspid(char *syso, int pido){   char sys[_NS]; sprintf(sys,"%s %d",syso,pido); system(sys);}



/* fonction ensta projet Gueydan            */
/* pour plus de détails, voir :             */
/* /media/black/backup_lpmcnpc220/detcheve/archives/archive-jussieu/detcheverry/Archive-old/archive-ensta/ArchivesDetcheve/2_annee/DetcheverryGil */


void envoyer_entier(FILE *f, int n) {
  char mes[10] ;
  sprintf(mes, "%d", n) ;
  fprintf(f, "%s\n", mes) ; /* chaine suivie de \n */
  fflush(f) ;
}


int  recevoir_entier(FILE *f) {
  int i ;
  char mes[10] ;
  fscanf(f , "%s", mes) ;
  i=atoi(mes) ;
  return(i) ;
}
pid_t lancerprocessus(char *arg[20], FILE **f_entree, FILE **f_sortie, int* fd_B ) {

  /* Permet, d'un processus père,  de lancer un processus en redirigeant éventuellement */
  /* son entrée et/ou  sa sortie standard dans des tuyaux également reliés au père      */
  /* on peut récupérer  a partir du dernier argument le numéro de descripteur  du coté  */
  /* du tuyau ou le père lit (ceci pour pouvoir utiliser un select)                     */
  pid_t pid ;
  int tuyau_entree[2], tuyau_sortie[2] ;
  
  /* Création éventuelles des tuyaux */
  if (*f_entree != stdin) {
    if ( pipe(tuyau_entree) == -1 ){
      perror("Erreur dans l'ouverture du tuyau d'entree ") ; 
      exit(EXIT_FAILURE) ;
    }
  }
  
  if (*f_sortie != stdout) {
    if ( pipe(tuyau_sortie) == -1 ){
      perror("Erreur dans l'ouverture du tuyau de sortie ") ; 
      exit(EXIT_FAILURE) ;
    }
  }

  pid = fork() ;
  switch(pid)
    {
    case -1 : /* erreur */
      perror( "Erreur  dans le  fork");
      exit(EXIT_FAILURE) ;
      
    case 0 :  /*processus fils */
      
       DEB ( fprintf(stderr, "[fils] Je suis le fils \n") ) ;


      /* Si l'entrée standard est redirigée */
      
      if (*f_entree != stdin) {
	/* Ouverture du tuyau entree en lecture */
	close( tuyau_entree[ECRITURE] ) ;
	  *f_entree = fdopen (tuyau_entree[LECTURE], "r") ;
	  if (  *f_entree == NULL){
	    perror("[fils] Erreur dans le fdopen de f_entree ") ; 
	    exit(EXIT_FAILURE) ;
	  }
	   DEB ( fprintf(stderr, "[fils] Ouverture du tuyau entree en lecture \n") ) ;
	
	  /* Redirection de l'entrée standard  */
	  close(STDIN_FILENO) ;
	  if ( dup2(tuyau_entree[LECTURE],STDIN_FILENO) == -1 )
	    {
	      perror("[fils] Erreur dans le dup2 du tuyau_entree ") ;
	    }
	  close(tuyau_entree[LECTURE]) ;
	  DEB ( fprintf(stderr, "[fils] Redirection de l'entrée standard (tuyau_entree) \n" ) );
	}
      
	/* Si la sortie  standard est redirigée */
      
      if (*f_sortie != stdout) {
	/* Ouverture du tuyau de sortie en écriture */
	close( tuyau_sortie[LECTURE] ) ;
	*f_sortie = fdopen (tuyau_sortie[ECRITURE], "w") ;
	if (  *f_sortie == NULL){
	  perror("[fils] Erreur dans le fdopen de f_sortie ") ; 
	  exit(EXIT_FAILURE) ;
	}
	 DEB (fprintf(stderr, "[fils] Ouverture du tuyau_sortie en écriture \n" ) ) ; 
	  
	/* Redirection la sortie standard (tuyau de sortie) */
	close(STDOUT_FILENO) ;
	if ( dup2(tuyau_sortie[ECRITURE],STDOUT_FILENO) == -1 )
	  {
	    perror("[fils] Erreur dans le dup2 du tuyau_sortie ") ;
	  }
	close(tuyau_sortie[ECRITURE]) ;
	 DEB (fprintf(stderr, "[fils] Redirection la sortie standard (tuyau_sortie) \n" ) ) ;
      }
 
      /* Recouvrement par  */
      if ( execvp(arg[0], arg) == -1 )
	{
	  perror("[fils] Erreur dans le excvp de predico3 ") ;
	  exit(EXIT_FAILURE) ;
	}
      break ;

    
    default:  /* processus père */

     
       DEB (fprintf(stderr, "[Père] je suis le père \n" ) ) ;
      
      /* Si l'entrée standard du fils  est redirigée et le père écrit dedans */
      
      if (*f_entree != stdin) {
	/* Ouverture du tuyau d'entrée  en écriture */
	close( tuyau_entree[LECTURE] ) ;
	*f_entree = fdopen (tuyau_entree[ECRITURE], "w") ;
	if (  *f_entree == NULL){
	  perror("[Père] Erreur dans le fdopen de f_entree ") ; 
	  exit(EXIT_FAILURE) ;
	}
	DEB (fprintf(stderr, "[Père] Ouverture du tuyau d'entree  en écriture \n" ) ) ;
      }

      /* Si la sortie  standard du fils  est redirigée et le père lit dedans  */
      
      if (*f_sortie != stdout) {
	/* Ouverture du tuyau B en lecture */
	close( tuyau_sortie[ECRITURE] ) ;
	*f_sortie = fdopen (tuyau_sortie[LECTURE], "r") ;
	if ( fd_B != NULL )  *fd_B = tuyau_sortie[LECTURE] ;
	if (  *f_sortie == NULL){
	  perror("[Père] Erreur dans le fdopen de f_sortie  ") ; 
	  exit(EXIT_FAILURE) ;
	}
	DEB (fprintf(stderr, "[Père] Ouverture du tuyau sortie en ecriture \n" ) ) ;
      }

      
      
    } /* fin du fork*/ 
   DEB ( fprintf(stderr, "[Père] je renvoie le pid, c'est <%d> \n", (int) pid )  );
  return (pid) ;
}
      
