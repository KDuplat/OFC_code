#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./analysis.h"

/* Create and open an output file. */
inline void initfile(char* fname, FILE **file){
    char path[200];
    sprintf(path, "%s/%s", Par.path,fname);
    *file = fopen(path, "w");
    if(file == NULL){
        perror("Error while opening the file in initfile\n");
        exit(0);
    }
}

/* Create and open a compressed output file. */
inline void initfilegz(char* fname, gzFile *file){
    char path[200];
    sprintf(path, "%s/%s", Par.path,fname);
    *file = gzopen(path, "wb");

    if (*file == NULL){
        perror("Error while opening compressed file");
        exit(EXIT_FAILURE);
    }
}

/* Write a string to a compressed file. */
inline void gzwrite_str(gzFile file, const char *str) {
    int len = strlen(str);
    if (gzwrite(file, str, len) != len) {
        fprintf(stderr, "Erreur lors de l'écriture des données compressées.\n");
        gzclose(file);
        exit(EXIT_FAILURE);
    }
}

/* Create an output directory if it does not exist. 
stat() gives info on folder state.
0777 donnes les permissions aux nouveaux dossier
*/
inline void initfolder(char* fname){
  char path[200];
  struct stat st = {0};
  sprintf(path, "%s/%s", Par.path,fname);

    /* Vérifier si le dossier existe. */
    if (stat(path, &st) == -1) {
        /* Si le dossier n'existe pas, le créer */
        if (mkdir(path, 0777) == 0) {
            printf("Dossier créé avec succès\n");
        } else {
            perror("Erreur lors de la création du dossier");
            exit(1);
        }
    }
}
/* Initialize all the output files */
inline void initialize_files(void){ 
    int i;
    char path[200];

    /* initfile("init_outputB.txt", &InitfileB); */
    initfile("time_outputB.txt",&Time.timefile);
    if(Ft.sw == 1){
        initfile("tf_outputB.txt", &Ft.ftfile);
        initfile("tflen_outputB.txt", &Ft.ftlenfile);
    }
    /* initfile("shape_outputB.txt",&Shapefile); */
    
    initfile("shape_outputB10.txt",&Shape.Shapefile10);
    initfile("shape_outputB100.txt",&Shape.Shapefile100);
    initfile("shape_outputB1000.txt",&Shape.Shapefile1000);

    initfilegz("Avalanche_outputB.gz", &F2);
    initfilegz("fractalbox.gz", &Fractal.boxfile);

    if (Comp.sw == 1){
        initfilegz("trans_histosize.gz", &Comp.transfile);
        initfilegz("trans_histotop.gz", &Comp.transfiletop);
        initfilegz("trans_histotprop.gz", &Comp.transfiletprop);
        initfilegz("trans_histoitime.gz", &Comp.transfileitime);
    }
    if(Top.sw){initfile("propagation.txt", & Top.file);}

    initfolder("Snapshots");

    if (Prediction.sw){
        initfile("prediction_s1.txt", &Prediction.files1);
        initfile("prediction_s2.txt", &Prediction.files2);
        initfile("prediction_s3.txt", &Prediction.files3);

        if(Prediction.sw == 2){
            initfilegz("random_s1.gz", &Random.file1);
            initfilegz("random_s2.gz", &Random.file2);
            initfilegz("random_s3.gz", &Random.file3);
        }
    }

    if(Random.sw){
        initfilegz("random_random.gz", &Random.file4);

        if (Random.sw == 2){
            for(i = 0; i<Random.nbclass; i++){

                sprintf(path,  "class%d.txt", i);
                initfile(path, &Random.class_file[i]);
  
            }
        }
        if (Random.sw_histo) initfile("random_histo_si.txt", &Random.file_histo);
    }

}

/* Save the state of the system */
inline void snapsave(void){
    int n; 
    double dev;
    FILE* file;
    char path[300];

    sprintf(path, "%s/Snapshots/nav%ld_outputB.txt", Par.path, Nav + 1);
    file = fopen(path, "w");
    fprintf(file, "#dev\n");
    Bn(dev = Site[n].zm - (Site[n].z + zF - Site[n].zF ); fprintf(file, "%f\n", dev););
    fclose(file);
}

/* Take snapshot of the system
0: Take snapshot at log times
1: Take snapshot at defined times with Snap.snapshots[1] and Snap.snapshots[3]
*/    
inline void snapshot(void){ 
    switch(Snap.snapshot[0]){
        case 0: 
            if((Nav + 1) == Snap.nsnap){
                snapsave();
                
                Snap.p++;
                Snap.nsnap = Snap.nsnap * 10;
            }
            break;
        case 1: 
            if((Nav + 1) >= Snap.snapshot[1] && (Nav + 1) <= Snap.snapshot[3]){
                if ((Nav + 1) % Snap.snapshot[2] == 0){
                    snapsave();
                }
            }
            break;
    }

    if ((Nav + 1)  == Par.navmax){
        snapsave();
    }
}

/* Test if a condition is fullfield */
inline void checkCondition(int condition, const char *errorMessage) { /* Check condition */
    if (!condition) {
        printf("%s\n", errorMessage);
        exit(1);
    }
}

/* Check all the parameters */
inline void checkParameters(void){
    checkCondition(Par.L1 > 0, "Error L1");
    checkCondition(Par.L2 > 0, "Error L2");
    checkCondition((Par.nu >= 0) && (Par.nu <=100), "Error nu");
    checkCondition((Par.zmmode >= 0) && (Par.zmmode <= 5), "Error zmmode");
    checkCondition((Par.zmode >= 0) && (Par.zmode <= 3), "Error zmode");
    checkCondition((Par.zmpar >= 0), "Error zmpar");
    checkCondition((Par.initmode == 0) || (Par.initmode == 1), "Error initmode");
    checkCondition((Par.bcmode >= 0) || (Par.bcmode <= 2), "Error bcmode");
    checkCondition((Par.navmax >= 0) , "Error navmax");
    checkCondition((Par.nbdata >= 0) , "Error nbdata");
    checkCondition((Par.savedelta >= 0), "Error savedelta");
    checkCondition((Par.shapetime >= 0), "Error shapetime");

    checkCondition((Noise.noise_mode == 0) || (Noise.noise_mode == 1), "Error Noise.noise_mode");
    checkCondition((Noise.nb_noise >= 0), "Error Noise.nb_noise ");
    checkCondition((Noise.nloop >= 0), "Error Noise.nloop");
    checkCondition((Noise.nbava >= 0), "Error Noise.nbava");

    checkCondition((Ft.itime  >= 0), "Error Ft.itime");
    checkCondition((Ft.dtime >= 0), "Error Ft.dtime ");
    checkCondition((Ft.nava >= 0), "Error Ft.nava");
    checkCondition((Ft.davg >= 0), "Error Ft.davg ");
    checkCondition((Ft.nbl >= 1), "Error Ft.nbl");
    checkCondition((Ft.nl >= 1), "Error Ft.nl ");
    /* checkCondition((Ft.endava >= 0), "Error Ft.endava");
    checkCondition((Ft.temp_dt_save >= 0), "Error Ft.temp_dt_save"); */

    checkCondition((Comp.savetrans >= 0), "Error Comp.savetrans");
    checkCondition((Comp.savestat >= 0), "Error Comp.savestat");
    checkCondition((Comp.limtrans >= 0), "Error Comp.limtrans");

    checkCondition((Prediction.sw >= 0) && (Prediction.sw <= 2), "Error Prediction.sw");
    checkCondition((Random.sw >= 0) && (Random.sw <= 2), "Error Random.sw");
    checkCondition(!(Prediction.sw && Random.sw), "Error Cannot have Prediction.sw and Random.sw at the same time" );
}


/* Save the full configuration of a system */
inline void savesys(void){ /* Save the system */
    int n;
    FILE *savefile;
    char savepath[200];
    
    if ((Nav % Par.savedelta == 0) || (Nav == Par.navmax)){
        sprintf(savepath, "%s/save_B.txt", Par.path);

        savefile = fopen(savepath, "w");
        fprintf(savefile,  "%d  #L1\n", Par.L1);
        fprintf(savefile,  "%d  #L2\n", Par.L2);
        fprintf(savefile, "%ld  #Nav\n", Nav);
        fprintf(savefile, "%.20Lf  #zF\n", zF);
        fprintf(savefile, "%.10Lf  #StackzF\n", StackzF);
        fprintf(savefile, "%d  #Seed\n", Par.randseed);
        /* Bn(fprintf(savefile, "%f  %f  %f\n", Site[n].z, Site[n].zm, Site[n].zF + StackzF)) */;
        Bn(fprintf(savefile, "%.20Lf  %.20Lf  %.20Lf\n", Site[n].z, Site[n].zm, Site[n].zF))
        fclose(savefile);
    }

}

/* Save the simulation time */
inline void savetime(void){ /*Save the time to calculate 1 avalanches averaged over Par.timedelta avalanches and the total simulation time*/
    Time.avgt += ((double) (Time.end1 - Time.start1)) / CLOCKS_PER_SEC;
    if((double)Time.end1 - Time.start1 > Time.maxt){Time.maxt =  ((double) (Time.end1 - Time.start1)) / CLOCKS_PER_SEC;} /* Max time to make an avalanche */   
    if (Nav % Par.timedelta == 0){
        Time.end = clock();
        Time.avgt = Time.avgt/Par.timedelta;
        fprintf(Time.timefile, "%ld  %Lf  %f  %f  %f\n", Nav, zF + StackzF, ((double) (Time.end - Time.start)) / CLOCKS_PER_SEC, Time.avgt, Time.maxt);
        Time.avgt = 0;
        Time.maxt = 0;
        fflush(Time.timefile);
    }
}

/* Save shapes of avalanches for three class of avalanche, small, medium and large*/
void saveShapes(void){ /*Save the shape of the avalanches*/
    int n;

    char buffer[_NL]; 
    int offset = 0;

    if (S <= ceil(Par.N / 1000) && Shape.c1000 < Par.shapetime) { /* Different files for different min size of the avalanche*/
    
        
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%ld  %.10Lf  %.5Lf  ", Nav + 1, zF,  StackzF);/* Write in buffer at position offset */

        for (n = Nbmod - 1; n >= 0; n--) {
            if (Site[Modifsys[n]].top > 0) {
                if (offset >= sizeof(buffer) - 200) { /* Verification of the buffer size, if full: write it*/
                    buffer[offset+1] = '\0'; /* End properly the string */
                    fprintf(Shape.Shapefile1000, "%s", buffer);
                    offset = 0;
                }
                offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d  ", Modifsys[n]);
            }
        }
        snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
        fprintf(Shape.Shapefile1000, "%s", buffer);

        Shape.c1000++;
    }
    else if (S > ceil(Par.N / 1000) && S <= ceil(Par.N / 100) && Shape.c100 < Par.shapetime) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%ld  %.10Lf  %.5Lf ", Nav + 1, zF,  StackzF);

        for (n = Nbmod - 1; n >= 0; n--) {
            if (Site[Modifsys[n]].top > 0) {
                if (offset >= sizeof(buffer) - 200) { /* Verification of the buffer size, if full: write it*/
                    buffer[offset+1] = '\0'; /* End properly the string */
                    fprintf(Shape.Shapefile100, "%s", buffer);
                    offset = 0;
                }
                offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", Modifsys[n]);
            }
        }
        snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
        fprintf(Shape.Shapefile100, "%s", buffer);

        Shape.c100++;
    }
    else if (S > ceil(Par.N / 100) && Shape.c10 < Par.shapetime) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%ld  %.10Lf  %.5Lf ", Nav + 1, zF,  StackzF);

        for (n = Nbmod - 1; n >= 0; n--) {
            if (Site[Modifsys[n]].top > 0) {
                if (offset >= sizeof(buffer) - 200) { /* Verification of the buffer size, if full: write it*/
                    buffer[offset+1] = '\0'; /* End properly the string */
                    fprintf(Shape.Shapefile10, "%s", buffer);
                    offset = 0;
                }
                offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", Modifsys[n]);
            }
        }
        snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
        fprintf(Shape.Shapefile10, "%s", buffer);

        Shape.c10++;
    }

    
    if (Shape.c1000 == Par.shapetime){fclose(Shape.Shapefile1000); Shape.Shapefile1000 = NULL; Shape.c1000++;} 
    if (Shape.c100 == Par.shapetime){fclose(Shape.Shapefile100); Shape.Shapefile100 = NULL; Shape.c100++;}
    if (Shape.c10 == Par.shapetime){fclose(Shape.Shapefile10); Shape.Shapefile10 = NULL; Shape.c10++;}
    if (Shape.c10 == (Par.shapetime + 1) && Shape.c100 == (Par.shapetime + 1) && Shape.c1000 == (Par.shapetime + 1)){Shape.sw = 0;}

}

/* Save shapes of avalanches for size between two threshold */
void saveShapes2(int smin, int smax){
    int n;

    char buffer[_NL]; 
    int offset = 0;

    double dev;
    FILE* file;
    char path[_NL];

    

    if (S >= smin && S <= smax){

        sprintf(path, "%s/Snapshots/nav%ld_before.txt", Par.path, Nav + 1);
        file = fopen(path, "w");
        fprintf(file, "#dev\n");
        Bn(dev = Prediction.Site_tempo[n].zm - (Prediction.Site_tempo[n].z + zF - Prediction.Site_tempo[n].zF ); fprintf(file, "%f\n", dev););
        fclose(file);

        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%ld  %.10Lf  %.5Lf  %d  ", Nav + 1, zF,  StackzF, S);

        for (n = Nbmod - 1; n >= 0; n--) {
            if (Site[Modifsys[n]].top > 0) {
                if (offset >= sizeof(buffer) - 200) { /* Verification of the buffer size, if full: write it*/
                    buffer[offset+1] = '\0'; /* End properly the string */
                    fprintf(Shape.Shapefile10, "%s", buffer);
                    offset = 0;
                }
                offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", Modifsys[n]);
            }
        }
        snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
        fprintf(Shape.Shapefile10, "%s", buffer);

        Shape.c10++;
    }
    if (Shape.c10 == Par.shapetime){fclose(Shape.Shapefile10); Shape.Shapefile10 = NULL; Shape.c10++;}
    if (Shape.c10 == (Par.shapetime + 1) ){Shape.sw = 0; Nav = Par.navmax;}

}

/* Save the complete propagation of avalanches for s > N/50*/
void save_propagation(void){ /*Save the propagation of the avalanches*/
    int n;

    if (S > ceil(Par.N / 50) && Top.nb_ava < Top.maxnb) {
        char buffer[_NL]; 
        int offset = 0;
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%ld  %.10Lf  %.5Lf ", Nav + 1, zF,  StackzF);

        for (n = 0; n < Top.id; n++) {
            if (offset >= sizeof(buffer) - 200) { /* Verification of the buffer size, if full: write it*/
                buffer[offset+1] = '\0'; /* End properly the string */
                fprintf(Top.file, "%s", buffer);
                offset = 0;
            }
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", Top.prop[n]);
        }
        snprintf(buffer + offset, sizeof(buffer) - offset, "\n");
        fprintf(Top.file, "%s", buffer);
        snapsave();
        Top.nb_ava++;
        
    }

    if (Top.nb_ava == Top.maxnb){fclose(Top.file); Top.file = NULL; Top.nb_ava++;Top.sw = 0;}

}

/*Verify if the two system array are still identical at the end of an avalanches*/
void verifSys(void){ 
    int n; sitest *sit, *sit2;
    for( n = 0;  n < Par.N; n++){
        sit = &(Site[n]);
        sit2 = &(Site2[n]);

        if (sit->z != sit2->z){printf("Diff z :%ld\nSit->z = %Lf, Sit2->z =%Lf\n", Nav, sit->z, sit2->z);exit(0);}
        if (sit->zm != sit2->zm){printf("Diff zm :%ld\nSit->zm = %Lf, Sit2->zm =%Lf\n", Nav, sit->zm, sit2->zm); exit(0);}
        if (sit->zF != sit2->zF){printf("Diff zF :%ld\nSit->zF = %Lf, Sit2->zF =%Lf\n", Nav, sit->zF, sit2->zF); exit(0);}
        if (sit->iHeap != sit2->iHeap){printf("Diff iHeap :%ld\nSit->iHeap = %d, Sit2->iHeap =%d\n", Nav, sit->iHeap, sit->iHeap);exit(0);}
    }
}

/*Make histograms for the avalanche size, topplings, intertime, propagation time*/
void compact_data(double dz, int ttop, int tava, double fractal_slope, int nin){ 
	int n = 0;
	FILE *statfile, *statfile2;
	char statpath[_NL], statpath2[_NL];
	char str[_NL];

    int x1;

    Comp.histos[S] += 1; 
    if (ttop < (Par.N * 1000)){Comp.histot[ttop] +=1;} /* Trouver une meilleur façon de faire */
    if (tava < (Par.N * 1000)){Comp.histotp[tava] +=1;}

    /* For the case s >= 2 */
    Comp.dz_2 += dz;
    for( n = 0; n < Comp.nbins - 1; n++){
        
        if (S > 1 && Comp.dz_2 > Comp.bins[n] && Comp.dz_2 <= Comp.bins[n+1]){ 
            Comp.histoit2[n] += 1;
            Comp.dz_2 = 0;
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

    /* In the transiant state, we save the data every Comp.savetrans steps*/
	if ( ((Nav + 1) <= Comp.limtrans) && ((Nav + 1) % Comp.savetrans) == 0){ 

		sprintf(str, "#%ld Nav\n", Nav + 1);
		gzwrite_str(Comp.transfile, str);
		gzwrite_str(Comp.transfiletop, str);
		gzwrite_str(Comp.transfiletprop, str);
		gzwrite_str(Comp.transfileitime, str);
		for(n = 0; n < (Par.N + 1); n++){
			sprintf(str, "%ld\n", Comp.histos[n]);
			gzwrite_str(Comp.transfile, str);
			Comp.histos[n] = 0;
        };
		for(n = 0; n < (Par.N * 1000); n++){
			sprintf(str, "%ld\n", Comp.histot[n]);
			gzwrite_str(Comp.transfiletop, str);
			Comp.histot[n] = 0;

			sprintf(str, "%ld\n", Comp.histotp[n]);
			gzwrite_str(Comp.transfiletprop, str);
			Comp.histotp[n] = 0;
		}

		for(n = 0; n < Comp.nbins; n++){
			sprintf(str, "%ld\n", Comp.histoit[n]);
			gzwrite_str(Comp.transfileitime, str);
			Comp.histoit[n] = 0;
		}

		sprintf(str, "\n");
		gzwrite_str(Comp.transfile, str);
		gzwrite_str(Comp.transfiletop, str);
		gzwrite_str(Comp.transfiletprop, str);
		gzwrite_str(Comp.transfileitime, str);

        if ((Nav + 1) == Comp.limtrans){
            if(Comp.transfile){gzclose(Comp.transfile);Comp.transfile = NULL;}
            if(Comp.transfiletop){gzclose(Comp.transfiletop);Comp.transfiletop = NULL;}
            if(Comp.transfiletprop){gzclose(Comp.transfiletprop);Comp.transfiletprop = NULL;}
            if(Comp.transfileitime){gzclose(Comp.transfileitime);Comp.transfileitime = NULL;}
            /* printf("End Transiant\n"); */
        }
	}

    /* In the stationary state, we save the data every Comp.savestat steps*/
	if( ((Nav + 1) > Comp.limtrans) && ((Nav + 1) % Comp.savestat) == 0){ 
		
		
		sprintf(statpath, "%s/stat_histosize.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
		for(n = 0; n < (Par.N + 1); n++){fprintf(statfile, "%ld\n", Comp.histos[n]);}
		fclose(statfile);

		/* sprintf(statpath, "%s/stat_histotop.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
		for(n = 0; n < (Par.N * 1000); n++){fprintf(statfile, "%ld\n", Comp.histot[n]);}
		fclose(statfile);

		sprintf(statpath, "%s/stat_histotprop.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
		for(n = 0; n < (Par.N * 1000); n++){fprintf(statfile, "%ld\n", Comp.histotp[n]);}
		fclose(statfile); */

        sprintf(statpath, "%s/stat_histotop.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
        sprintf(statpath2, "%s/stat_histotprop.txt",Par.path);
		statfile2 = fopen(statpath2, "w");
		fprintf(statfile2, "#%ldNav\n", Nav + 1);

		for(n = 0; n < (Par.N * 1000); n++){fprintf(statfile, "%ld\n", Comp.histot[n]); fprintf(statfile2, "%ld\n", Comp.histotp[n]);}
		fclose(statfile);
        fclose(statfile2);

		/* sprintf(statpath, "%s/stat_histoitime.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
		for(n = 0; n < Comp.nbins; n++){fprintf(statfile, "%ld\n",Comp.histoit[n]);}
        fclose(statfile);

        
        sprintf(statpath, "%s/stat_histoitime2.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
		for(n = 0; n < Comp.nbins; n++){fprintf(statfile, "%ld\n",Comp.histoit2[n]);}
		fclose(statfile); */

        sprintf(statpath, "%s/stat_histoitime.txt",Par.path);
		statfile = fopen(statpath, "w");
		fprintf(statfile, "#%ldNav\n", Nav + 1);
        sprintf(statpath2, "%s/stat_histoitime2.txt",Par.path);
		statfile2 = fopen(statpath2, "w");
		fprintf(statfile2, "#%ldNav\n", Nav + 1);

		for(n = 0; n < Comp.nbins; n++){fprintf(statfile, "%ld\n",Comp.histoit[n]);fprintf(statfile2, "%ld\n",Comp.histoit2[n]);}
        fclose(statfile);
        fclose(statfile2);

        if (Fractal.sw){
            sprintf(statpath, "%s/stat_histofractal.txt",Par.path);
            statfile = fopen(statpath, "w");
            fprintf(statfile, "#%ldNav\n", Nav + 1);
            for(n = 0; n < Comp.nbins_fractal; n++){fprintf(statfile, "%ld\n",Comp.histo_fractal[n]);}
            fclose(statfile); statfile = NULL;
        }

        /* Take histogram as function of the position of the avalanche (epicenter and center of mass) */
        if(Comp.switch_pos_histo){
            printf("Test2\n");
            sprintf(statpath, "%s/stat_pos_histosize.txt", Par.path);
            statfile = fopen(statpath, "w");
            fprintf(statfile, "#%ldNav\n", Nav + 1);

            for(n = 0; n < (Par.N + 1); n++){
                for(x1 = 0; x1 < Par.L1; x1++){
                    fprintf(statfile, "%ld  ", Comp.pos_histo[x1][n]);
                }
                fprintf(statfile, "\n");
            }
            fclose(statfile); statfile = NULL;

            sprintf(statpath, "%s/stat_cm_histosize.txt", Par.path);
            statfile = fopen(statpath, "w");
            fprintf(statfile, "#%ldNav\n", Nav + 1);

            for(n = 0; n < (Par.N + 1); n++){
                for(x1 = 0; x1 < Par.L1; x1++){
                    fprintf(statfile, "%ld  ", Comp.cm_histo[x1][n]);
                }
                fprintf(statfile, "\n");
            }
            fclose(statfile); statfile = NULL;
        }

	}
}

/* Calculate coordinates of the center of mass for periodic boundary conditions */
void periodic_cm(int *x1_cm,int *x2_cm){
    double theta_x;
    int x1, x2;
    int i;
    double C2 = 0.0, S2 = 0.0;
    double C1 = 0.0;
    double x2_temp, x1_temp;

    const double two_pi = 2.0 * M_PI;

    for (i = 0; i < Nbmod; i++) {
        coorno(&x1, &x2, Modifsys[i]);
        theta_x = two_pi * x2 / (double)Par.L2;
        C2 += cos(theta_x);
        S2 += sin(theta_x);

        C1 += x1;
    }

    x1_temp = C1/Nbmod;
    C2 /= Nbmod;
    S2 /= Nbmod;

    x2_temp = (Par.L2 / two_pi) * atan2(S2, C2);

    /* ensure result in [0, L) */
    if (x2_temp < 0){x2_temp += Par.L2;}

    *x2_cm = ((int) lround(x2_temp)) % Par.L2;
    *x1_cm = ((int) lround(x1_temp)) % Par.L1;
}

/* To ensure that all the files are correctly closed at the end of the simulation*/
void file_closed(void){ 
    if(Comp.transfile){gzclose(Comp.transfile);Comp.transfile = NULL;}
    if(Comp.transfiletop){gzclose(Comp.transfiletop);Comp.transfiletop = NULL;}
    if(Comp.transfiletprop){gzclose(Comp.transfiletprop);Comp.transfiletprop = NULL;}
    if(Comp.transfileitime){gzclose(Comp.transfileitime);Comp.transfileitime = NULL;}
    if(F2){gzclose(F2);F2 = NULL;}
    if(Fractal.boxfile){gzclose(Fractal.boxfile);Fractal.boxfile = NULL;}

    if(Time.timefile){fclose(Time.timefile); Time.timefile = NULL;}
    if(Ft.ftfile){fclose(Ft.ftfile); Ft.ftfile = NULL;}
    if(Ft.ftlenfile){fclose(Ft.ftlenfile); Ft.ftlenfile = NULL;}

    if(Shape.Shapefile10){fclose(Shape.Shapefile10); Shape.Shapefile10 = NULL;}
    if(Shape.Shapefile100){fclose(Shape.Shapefile100); Shape.Shapefile100 = NULL;}
    if(Shape.Shapefile1000){fclose(Shape.Shapefile1000); Shape.Shapefile1000 = NULL;}


    if(Prediction.files1){fclose(Prediction.files1); Prediction.files1 = NULL;}
    if(Prediction.files2){fclose(Prediction.files2); Prediction.files2 = NULL;}
    if(Prediction.files3){fclose(Prediction.files3); Prediction.files3 = NULL;}


    if(Random.file1){printf("Test\n"); gzclose(Random.file1); Random.file1 = NULL;}
    if(Random.file2){gzclose(Random.file2); Random.file2 = NULL;}
    if(Random.file3){gzclose(Random.file3); Random.file3 = NULL;}

    if(Random.file4){gzclose(Random.file4); Random.file4 = NULL;}
    if(Random.file_histo){fclose(Random.file_histo); Random.file_histo = NULL;}
    if(Top.file){fclose(Top.file); Top.file = NULL;}

}

/* Free all dynamically allocated memory. */
void free_fct(void){
    int n;
	free(Site);
	free(Site2);
	free(Heap);
	free(Modifsys);
	
    if (Comp.sw){
        free(Comp.bins);
        free(Comp.histos);
        free(Comp.histot);
        free(Comp.histotp);
        free(Comp.histoit);
        free(Comp.histoit2);
        free(Comp.histo_fractal);
        if(Comp.switch_pos_histo){
            for (n = 0; n < Par.L1; n += 1){free(Comp.pos_histo[n]);}
            free(Comp.pos_histo);
            for (n = 0; n < Par.L1; n += 1){free(Comp.cm_histo[n]);}
            free(Comp.cm_histo);
        }
    }

    if (Noise.noise_switch && Noise.nloop != 0){
        free(Noise.dev);
        free(Noise.init_Heap);
        free(Noise.init_Site);
    }
    
    if (Prediction.sw){
        free(Prediction.Site_s1);
        free(Prediction.Site_s2);
    }

    if (Prediction.sw || Random.sw) {free(Prediction.Site_tempo);}

    if ((Prediction.sw == 2) || Random.sw){
        free(Random.Site); 
        free(Random.Site2); 
    }
    
    if (Random.sw == 2){
        free(Random.id_ava);
        free(Random.class_file);
        free(Random.count_class);
        free(Random.dz_class);
        free(Random.bins);
    }

    if (Patch.sw){
        free(Patch.sizes);
        free(Patch.id); 
        free(Patch.bins_area);
        free(Patch.bins_avasize);
        for (n = 0 ; n<Patch.nbins_area; n++){
            free(Patch.histos[n]);
        }
        free(Patch.histos);
    }

    if(Top.sw){ free(Top.prop);}
}

/* Release resources before exiting. Even with manual exit*/
void cleanup_and_exit(int signum) {
    printf("Caught SIGTERM, cleaning up...\n");
    free_fct();
    file_closed();
    printf("End Cleaning...\n");
    exit(0);
}