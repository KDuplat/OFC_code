

inline int fopener(FILE **fo, char* namo , char* modo);

inline int elread(void* ro, char to, char *namo, char *ero);


inline void discsys(char *ro, char *baso);
inline void rmsys (char *srco);

inline void mrsys(char *scro, char *labelo, char *syso);


inline int  ressys(void* ro, char to, char *syso);

inline int scolressys(char ***rso, int *nlo, char *syso);
inline int listfile(char ***rso, int *nlo, char *srco);

inline int  elreadposfull(void* ro, char to, char *namo, char *disco, char *separo, int ncolo);

inline int ireadpos(int   * ro, char *namo, char *disco, int ncolo);
inline int dreadpos(double* ro, char *namo, char *disco, int ncolo);
inline int sreadpos(char  * ro, char *namo, char *disco, int ncolo);

inline int ireadpossys(int   * ro, char *syso, int ncolo);
inline int dreadpossys(double* ro, char *syso, int ncolo);
inline int sreadpossys(char  * ro, char *syso, int ncolo);



inline int colreadfull_vl(char co, char modo, char *caro, int lo);

int  colreadfull(int **rio, double **rdo, char ***rso, char **rco,  char to, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo);

inline int icolreadfull(   int **rio, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo); 
inline int dcolreadfull(double **rdo, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo); 
inline int scolreadfull(char  ***rso, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo); 
inline int ccolreadfull( char  **rco, int *nlo, char *namo, int ncolo, char *separo, char modo, char *caro, int linemaxo) ;

inline int icolread(int    **rio, int *nlo, char *namo, int ncolo            );
inline int dcolread(double **rdo, int *nlo, char *namo, int ncolo            );
inline int scolread(char  ***rso, int *nlo, char *namo, int ncolo, char *caro);
inline int ccolread(char   **rco, int *nlo, char *namo, int ncolo            );

inline void icolprfull(int    *rio, int nlo, FILE *fo, char *pro);
inline void dcolprfull(double *rdo, int nlo, FILE *fo, char *pro);
inline void scolprfull(char  **rso, int nlo, FILE *fo, char *pro);
inline void ccolprfull(char   *rco, int nlo, FILE *fo, char *pro);

inline void icolpr(int    *rio, int nlo);
inline void dcolpr(double *rdo, int nlo);
inline void scolpr(char  **rso, int nlo);
inline void ccolpr(char   *rco, int nlo);


int dcolsavemfull(int ntabo, double **tabo, int no, char *namo, char *como, int indpreco);
int dcolsavem(int ntabo, double **tabo, int no, char *namo, char *como);


/*  */
void copysmallfile(char *filea, char *fileb);
void writebuffer(char *buf, int *lcur, int lmax, FILE* f, char *string) ;
