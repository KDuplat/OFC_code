

inline void scolalloc(char ***ro, int nlo, int nso);
inline void scolfree(char** so,int no);


inline void strmncat(char *reso, int maxo, char* srco);
inline void dirfil(char *diro, char *filo, char *srco);


void ssplit(char *srco, char *separo, char ***reso, int *no);
void ssplitfree(char **reso, int no);


int sreplace(char* srco, char* ro, char* no, char *reso);

       int  sgetbetween(char* nom, char* s1, char* s2, char* s0, char   *reso);
inline int  igetbetween(char* nom, char* s1, char* s2, char* s0, int    *reso); 
inline int  dgetbetween(char* nom, char* s1, char* s2, char* s0, double *reso);

void sreplacefile(int no, char **outo, char **ino, char *scro, char *reso);
void sreplacefileone(char *outo, char *ino, char *scro, char *reso);


/**/
int s_in(char* sa, char* sb);
int s_replace(char* sa, char* sb, char* sc );


void removestarcolword(char **colw) ;
int Nfromword(char *word) ;
void writestarword_m(FILE *f, double a[3][3]) ;
char *zstrncat( char *dst, const char *src, size_t len );
