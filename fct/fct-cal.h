
inline int    imax(int a, int b);
inline int    imin(int a, int b);
inline double dmax(double a, double b);
inline double dmin(double a, double b);

inline void dcolminmax(double* colo, int no, double *mino, double *maxo);
inline void icolminmax(int* colo, int no, int *mino, int *maxo);
inline void dcolmoy   (double* colo, int no, double *moyo);
inline void dcolmoyvar(double* colo, int no, double *moyo, double *varo);

/* derived */
inline int    icolmin(int*    colo, int no);
inline int    icolmax(int*    colo, int no);
inline double dcolmin(double* colo, int no);
inline double dcolmax(double* colo, int no);


inline void dcolzero(double *ao, double nao);

inline void icolcopy(int    *ao, int    nao, int    *bo);
inline void dcolcopy(double *ao, double nao, double *bo);

inline void icoldupl(int *ao, int nao, int **bo, int *nbo);
inline void icoljoint(int *ao, int nao, int *bo, int nbo, int **reso, int *nreso);

inline void icoldiscard(int disco, int **ao, int *nao);


void icoluniq(int **reso, int *nreso);

int    intcompare(const void *ao, const void *bo);
int doublecompare(const void *ao, const void *bo);

void tqsort(char co, void *ao, int no);

int ibsearch (char co, const void *keyo, const void *baso, size_t no);
  


int compare_getbetween(const void *coa,const void *cob);
void qsort_getbetween(char **listo, int no, char *s1o, char *s2o, char *s0o);
