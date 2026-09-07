inline void initialize_files(void);
inline void gzwrite_str(gzFile file, const char *str);

inline void snapsave(void);
inline void snapshot();
inline void checkCondition(int condition, const char *errorMessage);
inline void checkParameters(void);
inline void savesys(void);
inline void savetime(void);
void saveShapes(void);
void saveShapes2(int smin, int smax);
void save_propagation(void);

void verifSys(void);
void compact_data(double dz, int ttop, int tava, double fractal_slope, int nin);
void periodic_cm(int *x1_cm,int *x2_cm);

void file_closed(void);
void free_fct(void);
void cleanup_and_exit(int signum);

