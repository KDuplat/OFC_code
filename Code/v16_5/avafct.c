#include"../../fct/fct.h" 
#include"./avafct.h" 
#include"./analysis.h"
#include"./heap.h"
#include"./corr.h"
#include"./noise.h"	
#include"./fractal.h"
#include"./prediction.h"
#include"./random.h"
#include"./patch.h"

/************** Tools / Auxiliary functions **************/
/* Calculate the coordinates from index */
void coorno(int *x1, int *x2, int n){
	*x1=n%Par.L1; 
	*x2=n/Par.L1; 
}

/* Box Muller method
Return a value from a gaussian distribution
a: avg value; d = standart deviation roughness*/
double gaussian(double a, double d){ 
	static double t = 0.0;
	double x,v1,v2,r;
	if (t == 0) {
		do {
			v1 = 2.0 * drand48() - 1.0;
			v2 = 2.0 * drand48() - 1.0;
			r = v1 * v1 + v2 * v2;
		} while (r>=1.0);
		r = sqrt((-2.0*log(r))/r);
		t = v2*r;
		return(a+v1*r*d);
	}
	else {
		x = t;
		t = 0.0;
		return(a+x*d);
	}
}

/* Create a logarithmic binning */
void log_bins(double min, double max, int nbins, double *bins) { 
	int i;
    double log_min = log10(min);
    double log_max = log10(max);
    double log_range = log_max - log_min;
    double bin_width = log_range / nbins;
    
	
    for (i = 0; i <= nbins; i++) {
        bins[i] = pow(10, log_min + i * bin_width);
    }
}

/* Create a linear binning */
void lin_bins(double min, double max, int nbins, double *bins){
	int i;
    double range = max - min;
    double bin_width = range / nbins;
    
    for (i = 0; i <= nbins; i++) {
        bins[i] = min + i * bin_width;
    }
}

/*Load the parameters*/
void parameters(int argco,  char* nameo){  
	int i,j,n, status; double *col[5]; char **listr; char str[_NL]; 
	
	if (argco != 2 ) {  fprintf(stderr,"The arguments are not correct \nExample : agg agg_par \n");  exit(1);  }
	/* input parameters */ 
	/* It reads all the lines from the parameter file */
	/* First the different numbers */
	for (i=1; i<5; i++){dcolread(&col[i],&n,nameo,i);}    /*it bugs if there is more than 4 columns*/
	i=-1; 
	i++;   	Par.L1        = (int)col[1][i];
	i++;   	Par.L2        = (int)col[1][i];                 
	i++;   	Par.nu        =      col[1][i];  
	i++;   	Par.zmmode    = (int)col[1][i];   
	i++;   	Par.zmode     =      col[1][i];
	i++;   	Par.zmpar     =      col[1][i];
	i++;   	Par.initmode  = (int)col[1][i]; 
	i++;   	Par.bcmode    = (int)col[1][i];   
	i++;   	Par.navmax    = (u_int64_t)col[1][i]; 
	i++;   	Par.switchPDF = (int)col[1][i]; Par.nbdata = (int)col[2][i];

	i++;   	Noise.noise_switch = (int)col[1][i];
	i++;    Noise.noise_mode = (int)col[1][i]; Noise.nb_noise = (int)col[2][i]; 
	        Noise.nloop = (int)col[3][i]; Noise.nbava = (int)col[4][i];
	i++; 	Fractal.sw = (int)col[1][i]; Fractal.sw_boxlist = (int)col[2][i];
	
	i++;   	Par.randseed  = (int)col[1][i];
	i++;   	for(j=1; j<5; j++){Snap.snapshot[j-1]=(u_int64_t)col[j][i];}
	i++;   		Par.timedelta = (u_int64_t)col[1][i];
	i++;   	Par.savedelta = (u_int64_t)col[1][i];
	i++;   	Par.shapetime = (int)col[1][i];
	i++;	Top.sw = (int)col[1][i]; Top.maxnb = (int)col[2][i];

	i++;	Prediction.sw = (int)col[1][i];
	i++; 	Prediction.mean_t = (int)col[1][i];
	i++;	Prediction.nb_snap = (int)col[1][i];
	i++;	Prediction.size_th[0] = (int)col[1][i]; Prediction.size_th[1] = (int)col[2][i];

	i++;	Ft.sw = (int)col[1][i];
	i++;   	Ft.itime = (int64_t)col[1][i]; Ft.dtime = (int64_t)col[2][i]; 
			Ft.nava = (int64_t)col[3][i]; Ft.davg = (int64_t)col[4][i];
	i++;   	Ft.nbl = (int)col[1][i]; Ft.nl = (int)col[2][i];
	i++;	Ft.endava = (int64_t)col[1][i];

	i++;   	Comp.sw = (int)col[1][i]; Comp.savetrans = (u_int64_t)col[3][i];
			Comp.savestat = (u_int64_t)col[4][i]; Comp.limtrans = (u_int64_t)col[2][i];
	i++;	Comp.switch_pos_histo = (int)col[1][i]; 

	i++;	Random.sw=(int)col[1][i]; Random.delta = col[2][i];
	i++;	Random.nbtry = (int)col[1][i];
	i++; 	Random.nb_ava = (int)col[1][i];
	i++;	Random.sw_histo = (int)col[1][i];

	i++; 	Patch.sw = (int)col[1][i];
	i++;	Patch.nbins_area = (int)col[1][i]; Patch.nbins_avasize = (int)col[2][i];
			Patch.delta = (double)col[3][i]; Patch.navsave = (u_int64_t)col[4][i];

	/* Than the strings */
	scolread(&listr,&n,nameo,2,"*"); i=-1;
	i++;  sprintf (Par.rac      ,"%s"  , listr[i]); 
	i++;  sprintf (Par.path     ,"%s"  , listr[i]); 

	/* Derived parameters */ 
	Par.N     = (int) floor(Par.L1*Par.L2);
	Par.redis = (1 - Par.nu)*0.25         ; 
	Comp.nbins = 1000;
	Comp.nbins_fractal = 100;
	Random.nbclass = 5;
	Random.nbins = 100;
	checkParameters();

	/* Create the file Para_used.txt */
	sprintf(Par.path_code, "%s",  "/home/kduplat/Documents/SimuQuakes/Prog/Code/v16_5");
	sprintf(Par.name,"%s", nameo); sprintf(Par.pathrac,"%s%s"   , Par.path, Par.rac);
	sprintf(str,"cp -f %s %s.par \n", Par.name, Par.pathrac); status=system(str);   
	
	if (status==0) printf("Parameters loaded correctly \n");
	if (status==1) printf("parameters loaded incorrecty \n");
	
} 

/* Memory allocation */
void memoryallocation(void){ 
	int x1;
	/* WARNING! : calloc set the values to 0 but malloc doesn't */
	Site  = (sitest*) calloc(Par.N   ,sizeof(sitest));	/* Array of the system */
	Site2  = (sitest*) calloc(Par.N   ,sizeof(sitest));	/* Array for parallel update */
	Heap = (heapst*) calloc(Par.N, sizeof(heapst));	/* Array that contain the heap structure */
	Modifsys = (int*) calloc(Par.N, sizeof(int)); /* Array to update the heap */
	if (Comp.sw == 1){	/* Array for different histogram for compact_data() */
		Comp.histos = (u_int64_t*) calloc(Par.N + 1, sizeof(u_int64_t));
		Comp.histot = (u_int64_t*) calloc(Par.N * 1000 , sizeof(u_int64_t)); /* 1000 is just to have an array big enough */
		Comp.histotp = (u_int64_t*) calloc(Par.N * 1000, sizeof(u_int64_t));
		Comp.histoit = (u_int64_t*) calloc(Comp.nbins, sizeof(u_int64_t));
		Comp.histoit2 = (u_int64_t*) calloc(Comp.nbins, sizeof(u_int64_t));
		Comp.bins = (double*) calloc(Comp.nbins + 1, sizeof(double));

		Comp.histo_fractal = (u_int64_t*) calloc(Comp.nbins_fractal, sizeof(u_int64_t));
		Comp.bins_fractal = (double*) calloc(Comp.nbins_fractal + 1, sizeof(double));

		if(Comp.switch_pos_histo){
			printf("Test3\n");
			Comp.pos_histo = (u_int64_t**) calloc(Par.L1, sizeof(u_int64_t*));
			for(x1 = 0; x1 < Par.L1; x1++){
				Comp.pos_histo[x1] = (u_int64_t*) calloc(Par.N + 1, sizeof(u_int64_t));
			}
			Comp.cm_histo = (u_int64_t**) calloc(Par.L1, sizeof(u_int64_t*));
			for(x1 = 0; x1 < Par.L1; x1++){
				Comp.cm_histo[x1] = (u_int64_t*) calloc(Par.N + 1, sizeof(u_int64_t));
			}
		}
	}

	if(Top.sw){Top.prop = (int*) calloc(Par.N * 1000, sizeof(int));}

	if (Noise.noise_switch && Noise.nloop != 0){
		Noise.dev = (double*) calloc(Par.N, sizeof(double));
		Noise.init_Site = (sitest*) calloc(Par.N   ,sizeof(sitest));
		Noise.init_Heap = (heapst*) calloc(Par.N, sizeof(heapst));
	}
 
	if (Prediction.sw){
		Prediction.Site_s1  = (sitest*) calloc(Par.N   ,sizeof(sitest));	/* Array snapshot ava size 1*/
		Prediction.Site_s2 = (sitest*) calloc(Par.N   ,sizeof(sitest));	/* Array snapshot medium ava*/
	}

	if (Prediction.sw || Random.sw) {
    	Prediction.Site_tempo  = (sitest*) calloc(Par.N   ,sizeof(sitest));
	}

	if (Random.sw_histo){
		Random.bins = (double*) calloc(Random.nbins + 1, sizeof(double));
	}

	if (Random.sw == 2){
		Random.id_ava = (int*) calloc(Par.shapetime, sizeof(int)); /* Array for random id toopling*/
		Random.dz_class = (double*) calloc(Random.nbclass, sizeof(double));
		Random.count_class = (int*) calloc(Random.nbclass, sizeof(int));
		Random.class_file = (FILE**) calloc(Random.nbclass, sizeof(FILE*));
	}

	if ((Prediction.sw == 2) || Random.sw){
		Random.Site  = (sitest*) calloc(Par.N   ,sizeof(sitest));	/* Array of the system */
		Random.Site2  = (sitest*) calloc(Par.N   ,sizeof(sitest));	/* Array for parallel update */
		Random.modifsys = (int*) calloc(Par.N, sizeof(int)); /* Array to update the heap */
	}

	if (Patch.sw){
		Patch.id = (int*) calloc(Par.N, sizeof(int));
		Patch.histos = (u_int64_t**) calloc (Patch.nbins_area, sizeof(u_int64_t*));
		for (x1 = 0 ; x1<Patch.nbins_area; x1++){
			Patch.histos[x1] = (u_int64_t*) calloc(Patch.nbins_avasize, sizeof(u_int64_t));

		Patch.bins_area = (double*) calloc(Patch.nbins_area + 1, sizeof(double));
		Patch.bins_avasize= (double*) calloc(Patch.nbins_avasize + 1, sizeof(double));
		Patch.sizes = NULL;
		}
	}
}

/* Initialise the system*/
void initialize(void){ 
	int n,seed; sitest *sit; 
	int tabseed[21] = {0, 20418437,94704581,45898144,49094059,77416885,91322469,40218964,93641667,29822916,625909,
											16336774,62881795,27998504,9612956,25592297,77114095,23554325,55380051,47971513,41100793};
	FILE *ifile, *initfile; long double col1, col2, col3; char savepath[200], initpath[200];
	char str[_NL];

	printf("AvalancheB begin\n");
  	initialize_files();
	sprintf(initpath, "%s/init_outputB.txt", Par.path);
	initfile = fopen(initpath, "w");

	sprintf(savepath, "%s/save_B.txt", Par.path);
	fprintf(initfile, "#z  zm  Site.zF\n");
	srand(time(NULL));
	Time.start = clock();
	StackzF = 0; /* Stack of the zF to avoid decrease of precision with time*/
	zF = 0;
	Nav=0;
	Snap.nsnap = 1; Snap.p = 0; /* nsnap: index of ava to take a snapshot */
	Time.avgt = 0.0; Time.maxt = 0.0;
	Ft.m = 0; /* Correlation counter */
	Comp.vmin = 1e-12; /* Min for log bins */
	Comp.vmax = 2;	/* Max for log bins */
	Comp.dz_2 = 0;

	Shape.sw = (Par.shapetime != 0);	/* Switch */
	/* Ulim = Comp.limtrans * 10; */ /* Transiant time upper limit to take data */
	/*Llim = Comp.limtrans * 2;  Transiant time lower limit to take data */
	Noise.cloop = 0;

	Prediction.Nava_last_large = 0;
	Prediction.N_ava_s1 = 0;
	Prediction.sw_s1 = 1;
	Prediction.Nin_s1 = -1;
	Prediction.Nav_s1 = 0;
	Prediction.N_ava_s2 = 0;
	Prediction.sw_s2 = 1;
	Prediction.Nin_s2 = -1;
	Prediction.Nav_s2 = 0;
	Prediction.s1 = 0;
	Prediction.s2 = 0;
	Prediction.c1 = 0;
	Prediction.c2 = 0;
	Prediction.c3 = 0;

	Random.count = 0;
	Random.sum_dz = 0;
	Random.dz_delta = 0;

	Top.nb_ava = 0;
	
	if (Comp.sw == 1){log_bins(Comp.vmin, Comp.vmax, Comp.nbins, Comp.bins); lin_bins(0, 2, Comp.nbins_fractal, Comp.bins_fractal);} 

	
	if (Patch.sw){
		Patch.sum_dz = 0;
		Patch.c_snap = 0;
		log_bins(1, Par.N, Patch.nbins_area, Patch.bins_area);
		log_bins(1, Par.N, Patch.nbins_avasize, Patch.bins_avasize);
	}

	switch(Par.initmode){
		case 0:	/* Start a new simulation from scratch */
			n=Par.randseed; if(n>0 && n<=20){ seed=tabseed[n]; }else{seed=abs(n);} ; init_genrand(seed); /* Create the seed */
			srand48(n);
			/* Initialisation of the system the system */
			Bn( sit=&(Site[n]);   sit->zm  = newzm(n);  
				sit->z   = newz(sit->zm)       ;  
				sit->zF  = 0       ;  sit->top = 0       ;
				sit->mod = 0;
				insert(sit->zm - (sit->z - sit->zF), n); /* Create the heap structure */
				Modifsys[n] = -1; 

				fprintf(initfile, "%Lf  %Lf  %Lf\n", Site[n].z, Site[n].zm, Site[n].zF);
			);

			memcpy(Site2, Site, Par.N*sizeof(sitest)); /* Copy Site into Site2 */


			Ft.ndata = 0;
			break;

		case 1: /* Start a new simulation from a previous configuration */
			ifile = fopen(savepath, "r");
			if(ifile == NULL){
				perror("Error while opening the file for the initialization\n");
				exit(0);
			}
			n=0;
			/* Compare the parameters with the save file parameters */
			checkCondition(fscanf(ifile, "%Lf  #L1", &col1) == 1, "Error in L1 initialisation");
			checkCondition( Par.L1 == (int)col1, "L1 dimention is not correct");

			checkCondition(fscanf(ifile, "%Lf  #L2", &col1) == 1, "Error in L2 initialisation");
			checkCondition( Par.L2 == (int)col1, "L2 dimension is not correct");
			checkCondition(fscanf(ifile, "%Lf  #Nav", &col1) == 1, "Error in Nav initialisation");
			Nav = (u_int64_t)col1;

			checkCondition(fscanf(ifile, "%Lg  #zF", &col1) == 1, "Error in zF initialisation");
			zF = col1;
			checkCondition(fscanf(ifile, "%Lg  #StackzF", &col1) == 1, "Error in StackzF initialisation");
			StackzF = col1;

			checkCondition(fscanf(ifile, "%Lf  #Seed", &col1) == 1, "Error in Seed initialisation");
			seed = (int)col1; 
			/* Load the pervious configuration */
			while(fscanf(ifile, "%Lf  %Lf  %Lf", &col1, &col2, &col3) == 3 ){
				sit=&(Site[n]); 
				sit->z   = col1;  sit->zF  = col3;
				sit->zm  = col2;  sit->top = 0;
				sit->mod = 0;


				insert(sit->zm - (sit->z - sit->zF), n); /* Create the heap structure */
				/* Site2[n] = Site[n]; */
				Modifsys[n] = -1; 
				n++;

				fprintf(initfile, "%Lf  %Lf  %Lf\n", sit->z, sit->zm, sit->zF);
			}
			memcpy(Site2, Site, Par.N*sizeof(sitest));/* Copy Site into Site2 */

			Ft.ndata = Nav/Ft.itime; /* Update the correlation counter */
			printf("System correctly initialized\n");
			fclose(ifile); ifile = NULL;
			break;
		default: printf("[Initialization]: mode=%d not defined\n", Par.initmode); exit(1);
	}
	
	/* Files initialisations */
	fprintf(Time.timefile, "#Nava  zF  time(s)  avgt(since the last time)   maxt(since the last time)\n");
	fprintf(Shape.Shapefile10, "#Shape of avalanche smaller than N/10 and larger than N/100\n#Nava  zF  StackzF  s  nin\n");
	fprintf(Shape.Shapefile100, "#Shape of avalanche smaller than N/100 and larger than N/1000\n#Nava  zF  StackzF  s  nin\n");
	fprintf(Shape.Shapefile1000, "#Shape of avalanche smaller than N/1000\n#Nava  zF  StackzF  s  nin\n");
	fflush(Shape.Shapefile10); fflush(Shape.Shapefile100); fflush(Shape.Shapefile1000);

	if(Par.switchPDF){
		if (Fractal.sw){sprintf(str, "zF + StackzF, S, nin, dz, ttop, tava, fractal_slope, fractal_chi2 \n");
		}else{sprintf(str, "zF + StackzF, S, nin, dz, ttop, tava, nin_cm\n");}
		
		gzwrite_str(F2, str);
	}

	if(Prediction.sw){
		fprintf(Prediction.files1, "#Nava  s  pos\n");
		fprintf(Prediction.files2, "#Nava  s  pos\n");
		fprintf(Prediction.files3, "#Nava  s  pos\n");
		fflush(Prediction.files1); fflush(Prediction.files2); fflush(Prediction.files3);
	}

	if (Prediction.sw == 2){
		gzwrite_str(Random.file1, "#Nav  zF  Stack_zF  nin  s  Newnin  NewS  nbtry  s_max_1  ntop_max1  nin_max1  cm_max_1  s_mean\n");
		gzwrite_str(Random.file2, "#Nav  zF  Stack_zF  nin  s  Newnin  NewS  nbtry  s_max_1  ntop_max1  nin_max1  cm_max_1  s_mean\n");
		gzwrite_str(Random.file3, "#Nav  zF  Stack_zF  nin  s  Newnin  NewS  nbtry  s_max_1  ntop_max1  nin_max1  cm_max_1  s_mean\n");
	}

	if(Random.sw){
		gzwrite_str(Random.file4, "#Nav  zF  Stack_zF  nin  s  Newnin  NewS  nbtry  s_max_1  ntop_max1  nin_max1  cm_max_1  s_mean\n");
		if (Random.sw_histo){log_bins(1, Par.N, Random.nbins, Random.bins);}
	}

	if (Ft.sw == 1){
		fprintf(Ft.ftfile, "#");
		fprintf(Ft.ftlenfile, "# Correlation lenght evolution averaged over %d lines\n", Ft.nl);
		fprintf(Ft.ftlenfile, "#");

		switch(Par.bcmode){
			case 0:
			case 1:

				if (Ft.nbl == 1){
					fprintf(Ft.ftfile,"%d  ", (int)(Par.L1 / 2) - 1);
					fprintf(Ft.ftlenfile,"%d  ", (int)(Par.L1 / 2) - 1);
				}else{
					for (n = 0; n < Ft.nbl; n++){
						fprintf(Ft.ftfile,"%d  ", (int)((Par.L1) * (double)(n + 1) / Ft.nbl) - 1);
						fprintf(Ft.ftlenfile,"%d  ", (int)((Par.L1) * (double)(n + 1) / Ft.nbl) - 1);
					}
				}
				fprintf(Ft.ftlenfile, "Nava  StackzF  zF"); 
				break;

			case 2:
				for (n = 0; n < Ft.nbl; n++){
					fprintf(Ft.ftfile,"#%d ", ((Par.L1 - 1)/(Ft.nbl)) * n);
					fprintf(Ft.ftlenfile,"#%d ", (int)((Par.L1 - 1)/(Ft.nbl)) * n);
				}
				fprintf(Ft.ftlenfile, "Nava  StackzF  zF");
				break;
			default: printf("[bcmode]: mode=%d not defined Ft.corrfile init\n", Par.bcmode); exit(1);


		}
		fprintf(Ft.ftlenfile, "\n");
		fprintf(Ft.ftfile, "\n"); 
	}
	
	fclose(initfile); initfile = NULL;
	/* for (n = 0; n < Par.N; n++){
		sit=&(Site[n]);
		if (sit->zm == 0){printf("ERROR: n=%d, zm=%f\n", n, sit->zm); exit(1);}
		printf("%f\n", sit->zm);
	} */

	if (Noise.noise_switch){add_noise();}
	/* printf("No error\n");
	exit(0); */
}

/* Different methodes to fix each site threshold */
double newzm(int n){ 
	double u,res ;  res=-1;
	switch (Par.zmmode) {
		case  0: res = rand01()                           ; break;
		/* case  1: res = 1. + Par.zmpar*rand11i()           ; break; */
		case  1: res = 1. + sqrt(Par.zmpar*Par.zmpar*3)*rand11i(); break; 
		case  2: u = n*1./Par.N; res = 1. + Par.zmpar*u   ; break;
		case  3: res = gaussian(1.0,Par.zmpar)            ; break; 
		case  4: res = 1                                  ; break;
		default: printf("[newzm]: mode=%d not defined\n", Par.zmmode); exit(1); }
	return(res); 
}

/* Different methodes to initialise each site energy */
long double newz(double zm){ 
	long double res ;  res=-1;
	switch (Par.zmode) { 
		case  0: res = 0                           ; break;
		case  1: res = drand48()                   ; break;
		case  2: res = rand01() * 0.9              ; break;
		case  3: res = rand01() * zm               ; break;
		default: printf("[newz]: mode=%d not defined\n", Par.zmode); exit(1); }
	return(res); 
}

/* Definition of the box to sweep during avalanche propagation */
int bcx1(int x){ int u; u=x;  if(x<0){u=0;} else{ if(x>=Par.L1){ u=Par.L1-1;}}; return(u);}

/* To take into account boundary condition along one axis */
int bcx2(int x){ 
	int u;
	switch (Par.bcmode){
	case 0: /* Open bc */
		u=x;  if(x<0){u=0;} else{ if(x>=Par.L2){ u=Par.L2-1;}}; return(u);
		break;

	case 1: /* Per bc */
	case 2:
		return x;
		break;

	default:
		printf("[bcmode]: mode=%d not defined in the box definition\n", Par.bcmode);
		exit(1);
		break;
	}
} 

/* Periodic condition for the box */
inline void Pcond(int x2, int *x2_s){ 
    *x2_s = x2 % Par.L2;
    if (*x2_s < 0) {
        *x2_s += Par.L2;
    }
}

void boxdefine(boxst *box, int nin, int x1p, int x1n, int x2p, int x2n){ /* Define the box limits */
	int x1,x2;   coorno(&x1,&x2,nin); 
	box->x1i=bcx1(x1-x1n); box->x2i=bcx2(x2-x2n); 
	box->x1a=bcx1(x1+x1p); box->x2a=bcx2(x2+x2p);   

	if(((box->x2i + Par.L2 ) <= box->x2a) || ((box->x2a - Par.L2 ) >= box->x2i)){box->x2i = 0; box->x2a = Par.L2-1;}/*Avoid the box to overlap itself*/
}

void boxprint(void){   fprintf(stderr,"boxprint: %d %d %d %d \n", Box.x1i, Box.x1a, Box.x2i, Box.x2a) ; }



/*********** Core functions **********/

/* Topple sites */
void onetopple(int no, int mode){ 
	
	int n,x1,x2,x1o,x2o; long double zinc; sitest *sit, *sit2;  

	sit=&(Site[no]);   

	/* if ((sit->z + zF - sit->zF) > Energy.max_site){Energy.max_site = (sit->z + zF - sit->zF);} */ /* Update the maximum toppling energy */
	/* Energy.sweep += (sit->z + zF - sit->zF); */ /* Update the total toppling energy */

	/* Energy Redistribution */
	if(mode>0){ zinc = Par.redis*(sit->z + zF - sit->zF); } 
	else      { zinc = Par.redis*(sit->z               ); }

	/* Reset of the toppling site */
	sit2=&(Site2[no]); 
	sit2->z   = 0          ; sit2->zF  = zF        ; 
	sit2->zm  = newzm(no) ; sit2->top ++          ;

	/* Get the coordinates of the toppling site */
	coorno(&x1o, &x2o, no);

	if(sit2->mod == 0){	/* Insert the toppling site in the list of modified sites */
		Modifsys[Nbmod] = no;
		Nbmod++;
		sit2->mod = 1;
		
	}
	if (Top.sw){Top.prop[Top.id] = no; Top.id++;} /* Save the propagation list of sites */

	/* Make the redistribution for each neighbors */
	switch (Par.bcmode){ 
		
		case 0: /* All open boundaries */
			x2=x2o; x1=x1o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}} 
								else{Energy.border += zinc; Energy.cborder += 1;}
					x1=x1o+1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}}  
								else{Energy.border += zinc; Energy.cborder += 1;}
			x1=x1o; x2=x2o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}}  
								else{Energy.border += zinc; Energy.cborder += 1;}
					x2=x2o+1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}}
								else{Energy.border += zinc; Energy.cborder += 1;}
			break;
		case 1: /* Open on x1 and periodic on x2 */
			x2=x2o; x1=x1o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc;if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}}
								else{Energy.border += zinc; Energy.cborder += 1;}
					x1=x1o+1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc;if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}}   
								else{Energy.border += zinc; Energy.cborder += 1;}
			x1=x1o; x2=x2o-1;   if ( x2 == -1 ){ x2 = Par.L2 - 1; }; n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;} 
					x2=x2o+1;   if ( x2 == Par.L2 ){ x2 = 0; }; n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}
			break;
		case 2: /* Periodic on x2, open on top, reflective on the bottom*/
			x2=x2o; x1=x1o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}} 
								else{Energy.border += zinc; Energy.cborder += 1;}
					x1=x1o+1;   if ( x1 == Par.L1  ){ x1 = x1o;} n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;} 
			x1=x1o; x2=x2o-1;   if ( x2 == -1 ){ x2 = Par.L2 - 1; }; n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}
					x2=x2o+1;   if ( x2 == Par.L2 ){ x2 = 0; }; n=No(x1,x2); Site2[n].z += zinc; if(Site2[n].mod == 0){Modifsys[Nbmod] = n; Nbmod++; Site2[n].mod = 1;}
			break;
		default: printf("[bcmode]: mode=%d not defined\n", Par.bcmode); exit(1); 
	}
}

/*Propagate the avalanche*/
inline int sweepbox(int nin, int *x1p, int *x1n, int *x2p, int *x2n){ 
	int n,ntopc, x1,x2;  sitest *sit;  
	int x1p_temp = *x1p, x1n_temp = *x1n, x2p_temp = *x2p, x2n_temp = *x2n;
	int x2_s; 

	/* Energy.sweep = 0; */
	
	boxdefine(&Box,nin,*x1p, *x1n, *x2p, *x2n);  /* Define the box limits */
	ntopc=0;  

	Bbox( Pcond(x2, &x2_s); n=No(x1, x2_s); Site[n]=Site2[n]; ); /* Update the parallel system */

	
	Bbox( /* Loop over the sites in the box to see if one of them topple*/
		Pcond(x2, &x2_s);
		n=No(x1, x2_s); sit=&(Site[n]);  

		
		if(sit->z + zF - sit->zF  >=  sit->zm){
			if (sit->top == 0){S++;}
			onetopple(n,1); ntopc++; 
			if(x1 == Box.x1i && *x1n == x1n_temp){(*x1n)++;} /* Box limits are increased only one time per sweepbox */
			if(x1 == Box.x1a && *x1p == x1p_temp){(*x1p)++;}
			if(x2 == Box.x2i && *x2n == x2n_temp){(*x2n)++;}
			if(x2 == Box.x2a && *x2p == x2p_temp){(*x2p)++;}
		}; 
	);
	if (Top.sw){Top.prop[Top.id] = -1; Top.id++;} /* Set the a marker for the end of the sweep  */
	

	/* if (Energy.sweep > Energy.max_sweep){Energy.max_sweep = Energy.sweep;}
	Energy.tot +=  Energy.sweep; */

	return(ntopc);
}

void oneavalancheEn(void){ /* One avalanche in the case where the energy is studied*/ /* NEED TO BE UPDATED */
	int n,nin,ntopc, x1,x2, x2_s, ttop, tava = 0;
	long double dz;
	int x1p = 1, x1n = 1, x2p = 1, x2n = 1; /* Limits of the box */
	
	/* double* ediff = (double*) calloc(Par.L1*Par.L2, sizeof(double));
	double sumediff = 0; */

	char str[_NL];
	double fractal_slope = 0, fractal_chi2 = 0;
	double sum_std = 0.0, sum_squares_std = 0.0, devth_std = 0.0, mean_std = 0.0;

	
	Energy.ei = 0, Energy.ef = 0, Energy.max_sweep = 0, Energy.max_site = 0, Energy.tot = 0, Energy.cborder = 0;
	Energy.border = 0, Energy.std = 0;

	/* find weakest site, topple it */
	Nbmod = 0;
	
	dz = Heap[0].value - zF; nin = Heap[0].iSite;
	zF += dz; 

	if ( (Nav + 1) >= Llim && (Nav + 1) < (Llim + Par.nbdata) ){

		Bn(
			/* ediff[n] = Site[n].z + zF - Site[n].zF; */			/*To calculate the energy in the entire system  by the sum of the difference (faster)*/
			
			Energy.ei += Site[n].z + zF - Site[n].zF; /* To calculate the energy in the entire system  by the difference of the sum */
			
			devth_std = Heap[n].value - zF;
			sum_std += devth_std;
			sum_squares_std += devth_std * devth_std;
		);
		mean_std = sum_std / n;
		Energy.std= (sum_squares_std / n) - (mean_std * mean_std);
	}

	/* Find the minimum of the heap */
	/* minheap(); */

	
	onetopple(nin,1);
	S = 1;   
	
	do{ ntopc=sweepbox(nin,&x1p, &x1n, &x2p, &x2n); tava += 1; Energy.sweep = 0;}while(ntopc>0);
	if( (Nav + 1) >= Comp.limtrans){
		if(Shape.sw){saveShapes();}	/* Save the shape of the avalanche */
		if (Fractal.sw && (Comp.sw || Par.switchPDF)){fractalboxes(&fractal_slope, &fractal_chi2);}
	}
	/* Avalanche number of topplings and heap reorganisation */
	ttop = 0; Bbox(Pcond(x2, &x2_s); n=No(x1,x2_s); if(Site[n].top >0){ttop += Site[n].top;}
						Site2[n].top=0; if(Nbmod != 0){reorganize();} );
	

	
	/* Compact data */
	if (Comp.sw){
		/* Comp.histos[S] += 1; 
		if (tava < (Par.N * 1000)){Comp.histotp[tava] +=1;}

		dz_2 += dz;
		for( n = 0; n < Comp.nbins - 1; n++){
			
			if (S > 1 && dz_2 > Comp.bins[n] && dz_2 <= Comp.bins[n+1]){ 
				Comp.histoit2[n] += 1;
				dz_2 = 0;
			}

			if (dz > Comp.bins[n] && dz <= Comp.bins[n+1]){
				Comp.histoit[n] += 1;
			}
		}

		if (Fractal.sw && (Nav + 1) > Comp.limtrans){
			for( n = 0; n < Comp.nbins_fractal-1; n++){
				if ( -fractal_slope >= Comp.bins_fractal[n] && -fractal_slope < Comp.bins_fractal[n+1]){
					Comp.histo_fractal[n] += 1;
					break;
				}
			}
		}
		
		compact_data(); */
		compact_data(dz, ttop, tava, fractal_slope, nin);
	}

	
	if(Par.switchPDF){
		
		if ( (Nav + 1) >= Comp.limtrans && (Nav + 1) < (Comp.limtrans + Par.nbdata)){	/* Save a catalogue of avalanches */
			if (Fractal.sw){
				sprintf(str, "%Lg  %.5Le  %d  %d  %d  %d  %.2f  %.3e  %.3e  %.3e  %g  %g\n", zF + StackzF, dz, nin, S, ttop, tava,
																			Energy.ei, Energy.min, Energy.std, Energy.tot, fractal_slope, fractal_chi2);
			}else{
				sprintf(str, "%Lg  %.5Le  %d  %d  %d  %d  %.2f  %.3e  %.3e  %.3e\n", zF + StackzF, dz, nin, S, ttop, tava,
																			Energy.ei, Energy.min, Energy.std, Energy.tot);
			}
			gzwrite_str(F2, str);
		}
		else if ((Nav + 1) == (Comp.limtrans + Par.nbdata)){
			gzclose(F2); F2 = NULL; 
			gzclose(Fractal.boxfile);Fractal.boxfile = NULL;
			Par.switchPDF = 0;
			if (!Comp.sw && Fractal.sw){Fractal.sw = 0;}
		}
	}

	rescale();	/* Rescale and reorganise the heap */
	/* verifHeap();
	verifSys(); */
	snapshot(); /* Take snapshots of the system */
	if (Nav + 1 < Ft.endava){Fftw();}	/* Calculate the correlation in the system */
	else if(Nav + 1 == Ft.endava){
		fclose(Ft.ftfile); Ft.ftfile = NULL;  fclose(Ft.ftlenfile); Ft.ftlenfile  = NULL;
	}

	/* free(ediff); */

}

/*Main function: Do an avalanche*/
void oneavalancheB(void){ 
	
	int n,nin,ntopc, x1,x2, x2_s, ttop, tava = 0;
	long double dz;
	int x1p = 1, x1n = 1, x2p = 1, x2n = 1; /* Limits of the box */

	char str[_NL];

	double fractal_slope = 0, fractal_chi2 = 0;

	int trash1; double trash2;
	int x1_cm, x2_cm,nin_cm;

	/* find weakest site, topple it */
	Nbmod = 0;
	Top.id = 0;

	nin = Heap[0].iSite; /* Calculate the incrementation in the system and find the epicenter*/
	dz = Heap[0].value - zF; /* Calculate the smallest energy to increment the system */
	zF += dz; 

	if (Random.sw || Prediction.sw ==2){Random.sum_dz += dz;}/* Time since last random avalanche done */
	if (Patch.sw){Patch.sum_dz += dz;}/* Time since last random patch analysis done */
	if (Prediction.sw || Random.sw ==2 ){memcpy(Prediction.Site_tempo, Site2, Par.N*sizeof(sitest));} /* Save Site2 state before the avalanche */
	
	if (Random.sw == 1 && ((Random.sum_dz - Random.delta) >= 0)){	/* Save the system initial state, random toppling_nostop*/
		memcpy(Prediction.Site_tempo, Site2, Par.N*sizeof(sitest));
		if (Patch.sw){load_patch();}
	}	
	
	onetopple(nin,1); /* Topple the initial site */
	S = 1;   
	
	do{ ntopc=sweepbox(nin,&x1p, &x1n, &x2p, &x2n); tava += 1; }while(ntopc>0); /* Propagate the avalanche */

	periodic_cm(&x1_cm,&x2_cm); /* Calculate the center of mass of the avalanche */
	if ((x1_cm < 0) || (x1_cm >= Par.L1) || (x2_cm < 0) || (x2_cm >= Par.L2)){
		printf("%ld\n", Nav);
		printf("ERROR IN CM, x1_cm = %d, x2_cm = %d\n", x1_cm, x2_cm);
	}
	nin_cm = No(x1_cm, x2_cm);
	
	/* If the transiant state is completed we start to take statistics */
	if( (Nav + 1) >= Comp.limtrans){
		if(Random.sw == 1 && ((Random.sum_dz - Random.delta) >= 0)){ 
				random_toppling_nostop(nin, zF, StackzF, (Nav + 1), Prediction.Site_tempo, S, Random.file4, &trash1, &trash2);
				Random.sum_dz = 0;
		}
		
		if (Random.sw == 2){
			patch_analysis(dz, nin); /* For Machine learning */	
		}
		
		if (Prediction.sw){
			prediction(nin); 
			/* prediction2(nin); */
		}
		
		if (Fractal.sw && (Comp.sw || Par.switchPDF)){fractalboxes(&fractal_slope, &fractal_chi2);}
		
		if(Shape.sw){
			saveShapes();
			/* saveShapes2(11000, 12000); */
		}

		if(Top.sw){save_propagation();}

		if(Comp.sw && Comp.switch_pos_histo){
			coorno(&x1, &x2, nin);
			Comp.pos_histo[x1][S] +=1; 
			Comp.cm_histo[x1_cm][S] +=1 ;
		}
		
		if(Patch.sw){patch_histo(nin);}
		
	}
	
	/* Avalanche number of topplings and heap reorganisation */
	ttop = 0; Bbox(Pcond(x2, &x2_s); n=No(x1,x2_s); if(Site[n].top >0){ttop += Site[n].top;}
						Site2[n].top=0; if(Nbmod != 0){reorganize();} );
	
	/* Compact data */ 
	if (Comp.sw){compact_data(dz, ttop, tava, fractal_slope, nin);}

	/* Save a list of avalanches */
	if(Par.switchPDF){
		
		if ( (Nav + 1) >= Comp.limtrans && (Nav + 1) < (Comp.limtrans + Par.nbdata)){	/* Save a catalogue of avalanches */
			if (Fractal.sw){
				sprintf(str, "%Lg  %d  %d  %.5Le  %d  %d  %g  %g\n", zF + StackzF, S, nin, dz, ttop, tava, fractal_slope, fractal_chi2);
			}else{
				sprintf(str, "%Lg  %d  %d  %.5Le  %d  %d  %d\n", zF + StackzF, S, nin, dz, ttop, tava, nin_cm);
			}
			gzwrite_str(F2, str);
		}
		else if ((Nav + 1) == (Comp.limtrans + Par.nbdata)){
			gzclose(F2); F2 = NULL; 
			gzclose(Fractal.boxfile);Fractal.boxfile = NULL;
			Par.switchPDF = 0;
			if (!Comp.sw && Fractal.sw){Fractal.sw = 0;}
		}
	}
	
	rescale();	/* Rescale the heap */
	/* verifHeap();
	verifSys(); */
	snapshot(); /* Take snapshots of the system */
	if (Ft.sw){Fftw();}	/* Calculate the correlation in the system */
	
} 



/* printing */
void printsitemode( int mode){
	int x1,x2; double u;  sitest *sit; 
	for(x2=0; x2<Par.L2; x2++){ 
	for(x1=0; x1<Par.L1; x1++){   sit=&(Site[No(x1,x2)]);   
		switch (mode) {
			case  0: u = sit->z    ; break;
			case  1: u = sit->zm   ; break;
			case  2: u = sit->zF   ; break;
			default: printf("[printsidemode]: mode=%d not defined\n", mode); exit(1); 
		}
	printf("%.2f ", u); }; 
	printf("\n");}
	printf("\n");
}

void printsite(void){ 
	printf("zF=%Lg \n\n", zF + StackzF); 
	printsitemode(0);
	printsitemode(1);
	printsitemode(2);
} 

