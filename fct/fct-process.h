

int pidcommand(int *pido,char *stro);
void syspid(char *syso, int pido);


void envoyer_entier(FILE *f , int n) ;      
int recevoir_entier( FILE *f) ; 
pid_t lancerprocessus(char *arg[10], FILE **f_entree, FILE **f_sortie, int* fd_B ) ; 

/* void associer_fonction_signal( int i , struct sigaction *act,void (*traitesignal)(void) ) ; */
