#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./analysis.h"
#include"./random.h"

void onetopple_rd(int no){ /* Topple sites */
	
	int n,x1,x2,x1o,x2o; long double zinc; sitest *sit, *sit2;  

	sit=&(Random.Site[no]);   

	/* Energy Redistribution */
	zinc = Par.redis*(sit->z + Random.zF - sit->zF);

	/* Reset of the toppling site */
	sit2=&(Random.Site2[no]); 
	sit2->z   = 0          ; sit2->zF  = Random.zF        ; 
	sit2->zm  = newzm(no) ; sit2->top ++          ;

	/* Get the coordinates of the toppling site */
	coorno(&x1o, &x2o, no);

	if(sit2->mod == 0){	/* Insert the toppling site in the list of modified sites */
		Random.modifsys[Random.nbmod] = no;
		Random.nbmod++;
		sit2->mod = 1;
	}

	switch (Par.bcmode){ 
		/* Make the redistribution for each neighbors */
		case 0: /* All open boundaries */
			x2=x2o; x1=x1o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc; } 
					x1=x1o+1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc; }  
			x1=x1o; x2=x2o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc; }  
					x2=x2o+1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc; }
			break;
		case 1: /* Open on x1 and periodic on x2 */
			x2=x2o; x1=x1o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc;}
					x1=x1o+1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc;}   
			x1=x1o; x2=x2o-1;   if ( x2 == -1 ){ x2 = Par.L2 - 1; }; n=No(x1,x2); Random.Site2[n].z += zinc;  
					x2=x2o+1;   if ( x2 == Par.L2 ){ x2 = 0; }; n=No(x1,x2); Random.Site2[n].z += zinc;
			break;
		case 2: /* Periodic on x2, open on top, reflective on the bottom*/
			x2=x2o; x1=x1o-1;   if ( In(x1,x2) ){ n=No(x1,x2); Random.Site2[n].z += zinc;}
					x1=x1o+1;   if ( x1 == Par.L1  ){ x1 = x1o;} n=No(x1,x2); Random.Site2[n].z += zinc;
			x1=x1o; x2=x2o-1;   if ( x2 == -1 ){ x2 = Par.L2 - 1; }; n=No(x1,x2); Random.Site2[n].z += zinc;
					x2=x2o+1;   if ( x2 == Par.L2 ){ x2 = 0; }; n=No(x1,x2); Random.Site2[n].z += zinc;
			break;
		default: printf("[bcmode]: mode=%d not defined\n", Par.bcmode); exit(1); 
	}
}

inline int sweepbox_rd(int nin, int *x1p, int *x1n, int *x2p, int *x2n){ /*Propagate the avalanche*/
	int n,ntopc, x1,x2;  sitest *sit;  
	int x1p_temp = *x1p, x1n_temp = *x1n, x2p_temp = *x2p, x2n_temp = *x2n;
	int x2_s; 
	
	boxdefine(&Random.Box,nin,*x1p, *x1n, *x2p, *x2n);  /* Define the box limits */
	ntopc=0;  

	Bbox_rd( Pcond(x2, &x2_s); n=No(x1, x2_s); Random.Site[n] = Random.Site2[n]; ); /* Update the parallel system */

	Bbox_rd( /* Loop over the sites in the box to see if one of them topple*/
		Pcond(x2, &x2_s);
		n=No(x1, x2_s); sit=&(Random.Site[n]);  

		if(sit->z + Random.zF - sit->zF  >=  sit->zm){
			if (sit->top == 0){Random.S++;}
			onetopple_rd(n); ntopc++; 
			if(x1 == Random.Box.x1i && *x1n == x1n_temp){(*x1n)++;} /* Box limits are increased only one time per sweepbox */
			if(x1 == Random.Box.x1a && *x1p == x1p_temp){(*x1p)++;}
			if(x2 == Random.Box.x2i && *x2n == x2n_temp){(*x2n)++;}
			if(x2 == Random.Box.x2a && *x2p == x2p_temp){(*x2p)++;}
		}; 

	);
	

	return(ntopc);
}

void oneavalanche_rd(int nin){ /*Main function*/

	int ntopc, tava = 0;
	int x1p = 1, x1n = 1, x2p = 1, x2n = 1; /* Limits of the box */
	int n, x1, x2, x2_s;
	int x1_cm, x2_cm;

	Random.nbmod = 0;

	Random.Site[nin].z = 1-(Random.zF - Random.Site[nin].zF);
	onetopple_rd(nin); /* Topple the initial site */
	Random.S = 1;   
	
	do{ ntopc=sweepbox_rd(nin,&x1p, &x1n, &x2p, &x2n); tava += 1; }while(ntopc>0); /* Propagate the avalanche */
	periodic_cm_rd(&x1_cm,&x2_cm);
	Random.n_cm = No(x1_cm, x2_cm);
	
	/* Avalanche number of topplings and heap reorganisation */
	Random.ttop = 0; Bbox_rd(Pcond(x2, &x2_s); n=No(x1,x2_s); if(Random.Site2[n].top >0){Random.ttop += Random.Site2[n].top;}
						Random.Site2[n].top=0;);
} 


void periodic_cm_rd(int *x1_cm,int *x2_cm){
    double theta_x;
    int x1, x2;
    int i;
    double C2 = 0.0, S2 = 0.0;
    double C1 = 0.0;
    double x2_temp, x1_temp;

    const double two_pi = 2.0 * M_PI;

    for (i = 0; i < Random.nbmod; i++) {
        coorno(&x1, &x2, Random.modifsys[i]);
        theta_x = two_pi * x2 / (double)Par.L2;
        C2 += cos(theta_x);
        S2 += sin(theta_x);

        C1 += x1;
		Random.modifsys[i] = 0;
    }

	Random.nbmod = 0;
    x1_temp = C1/Nbmod;
    C2 /= Nbmod;
    S2 /= Nbmod;

    x2_temp = (Par.L2 / two_pi) * atan2(S2, C2);

    /* ensure result in [0, L) */
    if (x2_temp < 0){x2_temp += Par.L2;}

    *x2_cm = ((int) lround(x2_temp)) % Par.L2;
    *x1_cm = ((int) lround(x1_temp)) % Par.L1;
}



void random_toppling(int nin, long double fct_zF, u_int64_t fct_Nav, sitest  *Sys, int s){  /*Stop when a large avalanche is found*/
    int *indices = (int*) calloc(Par.N, sizeof(int));
    int n, temp, i, sw_break, index;

    /* Built an array with all the indices*/
	Bn(indices[n] = n;);
    indices[Par.N - 1] = nin;
    indices[nin] = Par.N - 1;
	/* Randomize the indices */
    /* This index array avoid to pick the same site twice or the most unstable site*/
	for(n=0;n<(Par.N-1);n++){
        do{
            i = randim(Par.N);
        }while (i == (Par.N - 1));
        
        temp = indices[n];
        indices[n] = indices[i];
        indices[i] = temp;
	};
	Random.zF = fct_zF;

	sw_break = 0;
	for(n=0;n<Random.nbtry;n++){
		memcpy(Random.Site, Sys, Par.N*sizeof(sitest));
		memcpy(Random.Site2, Sys, Par.N*sizeof(sitest));

		oneavalanche_rd(indices[n]);

		if (Random.S >= (int)(Par.N/4)){break; sw_break = 1;}
	
	}

	index = sw_break ? indices[n] : indices[n-1]; /* If it does not break n is incremented one last time to check the for loop so n-1 is necessary */

	/* if (s >= (int)(Par.N/4)){ #TO FIX
		fprintf(Random.file2, "%ld  %d  %d  %d  %d  %d\n", fct_Nav, nin, s, index, Random.S, n);
	}else if (s == 1){
		fprintf(Random.file1, "%ld  %d  %d  %d  %d  %d\n", fct_Nav, nin, s, index, Random.S, n);
	} */
	free(indices);
}

void random_toppling_nostop(int nin, long double fct_zF, long double fct_stack_zF, u_int64_t fct_Nav, sitest *Sys_fct, int s, gzFile file, int *sc_out, double *smean_out){  /*Stop when all site were tried*/
    
    /* int n, temp, i, sw_break, s_min, s_max, new_s, nbtry, new_id; */
	int n, i, sw_break, new_s, nbtry, new_id, temp;

	int *indices = (int*) calloc(Par.N, sizeof(int)); /* Random sites DON'T FORGET TO UNCOMMENT THE FREE FCT*/

	int Ts_max[5] = {0};
	int Tnin_max[5] = {0};
	int Tcm_max[5] = {0};
	int Tid_max[5] = {0};
	int Tntop_max[5] = {0};
	int j;
	double s_mean;

	int *histo_s_max = (int*) calloc(Random.nbins, sizeof(int));
	/* int s_max = 0; */

	/* double dev;
    FILE* file;
    char path[300]; */
	char str[_NL];
	
	int offset = 0;

	nbtry = 0;
	new_s = -1;
	new_id = -1;
	s_mean = 0;
	 

	/* Save the system configuration */
	/* sprintf(path, "%s/Snapshots/nav%ld.txt", Par.path, Nav + 1);
	file = fopen(path, "w");
	fprintf(file, "#dev\n");
	Bn(dev = Prediction.Site_tempo[n].zm - (Prediction.Site_tempo[n].z + zF - Prediction.Site_tempo[n].zF ); fprintf(file, "%f\n", dev););
	fclose(file); */


	/* Random sites */
    /* Built an array with all the indices
	Randomize the indices
    This index array avoid to pick the same site twice or the most unstable site*/
	Bn(indices[n] = n;);
    indices[Par.N - 1] = nin;
    indices[nin] = Par.N - 1;
	
	for(n=0;n<(Par.N-1);n++){
        do{
            i = rand() % Par.N;
        }while (i == (Par.N - 1));
        
        temp = indices[n];
        indices[n] = indices[i];
        indices[i] = temp;
	};


	Random.zF = fct_zF; /* To propagate avalanche correctly */
	sw_break = 0;
	for(n=0;n<Random.nbtry;n++){
		/* Reset the configuration */
		memcpy(Random.Site, Sys_fct, Par.N*sizeof(sitest));
		memcpy(Random.Site2, Sys_fct, Par.N*sizeof(sitest));
		/* oneavalanche_rd(Heap[1].iSite); */ /* Unstable sites */

		oneavalanche_rd(indices[n]);	/* Random sites */

		/* if (Random.S>s_max){s_max = Random.S;} */
		/* Keep the five largest avalanches */
		if(Random.S > Ts_max[0]){
			for(i = 4; i >= 0; i--){
				if (Random.S >Ts_max[i]){
					for(j = 0; j < i; j++){
						Ts_max[j] = Ts_max[j+1];
						Tnin_max[j] = Tnin_max[j+1];
						Tid_max[j] = Tid_max[j+1];
						Tntop_max[j] = Tntop_max[j+1];
						Tcm_max[j] = Tcm_max[j+1];
					}

					Ts_max[i] = Random.S;
					Tnin_max[i] = indices[n];
					Tid_max[i] = n+1;
					Tntop_max[i] = Random.ttop;
					Tcm_max[i] = Random.n_cm;
					break;
				}
			}
		}
		s_mean += Random.S;
		/* Make an histogram per configuration */
		if(Random.sw_histo){
			for( j = 0; j <= Random.nbins; j++){
				if ((Random.bins[j] <= Random.S ) && (Random.S < Random.bins[j+1])){
					histo_s_max[j]++;
					break;
				}
			}
		}

		/* Enregistre la première grande ava trouvé */
		if (Random.S >= (int)(Par.N/4) && !sw_break){	
			new_s = Random.S; 
			/* new_id = Heap[1].iSite; */ /* Unstable sites */
			new_id = indices[n];	/* Random sites */
			nbtry = n;
			sw_break = 1;
		}
	}
	s_mean /= Random.nbtry;

	/* Return the largest value */
	*sc_out = Ts_max[4];
	*smean_out = s_mean;

	/* Write the histogram */
	if(Random.sw_histo){
		for( n = 0; n < Random.nbins; n++){
			fprintf(Random.file_histo, "%d  ", histo_s_max[n]);
		}
		fprintf(Random.file_histo, "\n");
		fflush(Random.file_histo);
	}

	if (new_s == -1){nbtry = n-1;} /* If not large avalanche found, set to a default value */

	if (Patch.sw){	/* Random toppling in a single patch */
		/* Offset enregistre la position de l'écriture dans str */
		offset += sprintf(str+ offset, "%ld  %Lg  %Lg  %d  %d  %d  %d  %d  %g  ",
								fct_Nav, fct_zF, fct_stack_zF, nin, s, Tid_max[4], Tnin_max[4], Ts_max[4], s_mean);
		
		
		for (n = 0; n<Patch.len_arr; n++){
			offset += sprintf(str + offset, "%d ", Patch.sizes[n]);
		}
		offset += sprintf(str + offset, "\n");
	}else{
		
		sprintf(str, "%ld  %Lg  %Lg  %d  %d  %d  %d  %d  %d  %d  %d  %d  %g\n",
	 						fct_Nav, fct_zF, fct_stack_zF, nin, s, new_id, new_s, nbtry, Ts_max[4], Tntop_max[4], Tnin_max[4], Tcm_max[4], s_mean);
		
	}
	gzwrite_str(file, str);

	if(Random.sw){
		Random.count++;
		if (Random.count == Random.nb_ava){Random.sw = 0; Nav = Par.navmax - 1;} /* ENABLE THIS WHEN NOT DONE IN patch_analysis() */
	}
	/* if (Random.count % 10 == 0){fflush(Random.file3);}  */

	free(indices);	/* Random sites */
	free(histo_s_max);
}

/* 
Do random toppling in a single patch  
Can generate indexes using Randomfct_indec_geneator.py
Need to start from a configuration with the patch 
*/
void random_toppling_in_patch(long double fct_zF, sitest *Sys){ 
    
	int n,j;
	int *indices = (int*) calloc(Par.N, sizeof(int)); /* Random sites DON'T FORGET TO UNCOMMENT THE FREE FCT*/
	double s_mean;
	int *histo_s_max = (int*) calloc(Random.nbins, sizeof(int));

	FILE *id_file;
	char id_path[_NL];
	int col1;
	int temp;
	s_mean = 0;
	 

	/* Save the system configuration */
	/* sprintf(path, "%s/Snapshots/nav%ld.txt", Par.path, Nav + 1);
	file = fopen(path, "w");
	fprintf(file, "#dev\n");
	Bn(dev = Prediction.Site_tempo[n].zm - (Prediction.Site_tempo[n].z + zF - Prediction.Site_tempo[n].zF ); fprintf(file, "%f\n", dev););
	fclose(file); */

	/* Load the index of the sites in the patch */
	sprintf(id_path, "%s/Random_index.txt", Par.path);
	id_file = fopen(id_path, "r");
	if(id_file == NULL){
		perror("Error while opening the file to load index\n");
		exit(0);
	}

	n = 0;
	/* Indices are drwn from in the patch */
	temp = fscanf(id_file, "%d", &col1);
	(void) temp;
	Random.nbtry = col1;
	printf("%d\n",Random.nbtry);
	while(fscanf(id_file, "%d", &col1) == 1 ){
		indices[n] = col1;
		n++;
	}
	


	Random.zF = fct_zF; /* To propagate avalanche correctly */
	for(n=0;n<Random.nbtry;n++){
		memcpy(Random.Site, Sys, Par.N*sizeof(sitest));
		memcpy(Random.Site2, Sys, Par.N*sizeof(sitest));
		/* oneavalanche_rd(Heap[1].iSite); */ /* Unstable sites */

		oneavalanche_rd(indices[n]);	/* Random sites */

		s_mean += Random.S;
		for( j = 0; j <= Random.nbins; j++){
			if ((Random.bins[j] <= Random.S ) && (Random.S < Random.bins[j+1])){
				histo_s_max[j]++;
				break;
			}
		}

	}
	
	for( n = 0; n < Random.nbins; n++){
		fprintf(Random.file_histo, "%d  ", histo_s_max[n]);
	}
	fprintf(Random.file_histo, "\n");
	fflush(Random.file_histo);

	free(indices);	/* Random sites */
	free(histo_s_max);
	
}



int save_info(int nin, int class_id, FILE* file){
    int sc, sum, i, n;
    double smean;

    double dev;
    FILE* snap_file;
    char path[300];
	/* Save the system state before */
    sprintf(path, "%s/Snapshots/nav%ld.txt", Par.path, Nav + 1);
	snap_file = fopen(path, "w");
	fprintf(snap_file, "#dev\n");
	Bn(dev = Prediction.Site_tempo[n].zm - (Prediction.Site_tempo[n].z + zF - Prediction.Site_tempo[n].zF ); fprintf(snap_file, "%f\n", dev););
	fclose(snap_file);
	/* Find the largest avalanche in the configuration and save the informations*/
    random_toppling_nostop(nin, zF, StackzF, (Nav + 1), Prediction.Site_tempo, S, Random.file4, &sc, &smean);
    fprintf(file, "%ld  %d  %d  %d  %g\n", Nav+1, S, nin, sc, smean);
    Random.count_class[class_id]++;

	/* Close files if enough avalanche taken */
    if (Random.count_class[class_id] == Random.nb_ava){
        fclose(file); file = NULL;
        sum = 0;
        for(i = 0; i<Random.nbclass; i++){sum += Random.count_class[i];}
        printf("%d\n", sum);
        if(sum == (Random.nb_ava*Random.nbclass)){
            Nav = Par.navmax-1;
            Shape.sw = 0;
        }
        
    }
    return 1;
}

int update_class(int smin, int smax, int class_id, int nin, FILE* file){
    int stop = 0;
	/* If there is enough time since the last avalanche taken  in the class*/
    if (Random.dz_class[class_id] >= Random.delta && smin<=S && S<smax && Random.count_class[class_id] < Random.nb_ava){ /* On a aussi un temps minimal pour chaque classe */
        Random.dz_class[class_id] = 0;
        Random.dz_delta = 0;   

        stop = save_info(nin, class_id, file);
    }
    return stop;
}

/* Récupère le snapshot, et la taille d'avalanche avec la suceptibilité*/
void patch_analysis(double dz, int nin){
    int i;
    int smin[] = {1, 10, 100, 1000, 10000};	/* 5 different classes */
    int smax[] = {10, 100, 1000, 10000, 100000};

    for(i = 0; i< Random.nbclass-1; i++){	/* Independant time between classes */
        Random.dz_class[i]+= dz;
    }
    Random.dz_delta += dz;	/* Global time */
    if(S>=smin[Random.nbclass-1]){ /* Si c'est une grande  on enregistre direct*/
        if (Random.count_class[Random.nbclass-1] < Random.nb_ava){
            save_info(nin, Random.nbclass-1, Random.class_file[Random.nbclass-1]);
        }
    }
    else if (Random.dz_delta >= Random.delta/Random.nbclass){ /* On fixe untemps minimal entre les classes */
        for(i = 0; i< Random.nbclass-1; i++){
            if(update_class(smin[i], smax[i] ,i ,nin ,Random.class_file[i])){
                break;
            }
        }
    }
}




int intComparator ( const void * first, const void * second ) {
    int firstInt = * (const int *) first;
    int secondInt = * (const int *) second;
    return firstInt - secondInt;
}