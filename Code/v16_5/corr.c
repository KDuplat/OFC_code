#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./corr.h"

/* For more information see https://www.fftw.org/fftw3.pdf */

void Fftwinit(void){ /* Initialization of the variables */
    int x1, x2;

    Ft.tabi = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Par.L2);
    Ft.tabq = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Par.L2);
    Ft.tabr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Par.L2);
    Ft.tabcorr = (fftw_complex**)fftw_malloc(sizeof(fftw_complex*) * Par.L1);
    for (x1 = 0; x1 < Par.L1; x1 += 1){
        Ft.tabcorr[x1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Par.L2);
    }

    for(x1 = 0; x1 < Par.L2; x1 += 1){
        Ft.tabi[x1][0] = 0;
        Ft.tabi[x1][1] = 0;
        Ft.tabq[x1][0] = 0;
        Ft.tabq[x1][1] = 0;
        Ft.tabr[x1][0] = 0;
        Ft.tabr[x1][1] = 0;
    }

    for(x1 = 0; x1 < Par.L1; x1 += 1){
        for (x2 = 0; x2 < Par.L2; x2 += 1){
            Ft.tabcorr[x1][x2][0] = 0;
            Ft.tabcorr[x1][x2][1] = 0;
        }
    }   


    Ft.plf = fftw_plan_dft_1d(Par.L2, Ft.tabi, Ft.tabq, FFTW_FORWARD, FFTW_MEASURE);
    Ft.plb = fftw_plan_dft_1d(Par.L2, Ft.tabq, Ft.tabr, FFTW_BACKWARD, FFTW_MEASURE);

    Ft.var = (double*) calloc(Ft.nbl, sizeof(double));
    Ft.avg = (double *) calloc(Par.L1, sizeof(double));
}

void Fftwcov(void){ /* Covariance */
    int x1, x2; sitest *sit;
    double dev = 0;
    
    for (x1 = 0; x1 < Par.L1; x1 += 1){ 
            
        for (x2 = 0; x2 < Par.L2; x2 += 1){
            sit = &(Site[No(x1, x2)]);
            dev = sit->zm - (sit->z + zF - sit->zF);

            Ft.tabi[x2][0] = dev; /* Real part */
            Ft.tabi[x2][1] = 0. ;   /* Imaginary part */
            Ft.avg[x1] += dev;  /* Average */
        }

        /******* TF ******/
        fftw_execute(Ft.plf);
        /****** Covariance ******/
        for (x2 = 0; x2 < Par.L2; x2 += 1){ 
            Ft.tabq[x2][0] = Ft.tabq[x2][0] * Ft.tabq[x2][0] + Ft.tabq[x2][1] * Ft.tabq[x2][1];
            Ft.tabq[x2][1] = 0.;
        }
        /******* Reverse TF *******/
        fftw_execute(Ft.plb);
        
        for (x2 = 0; x2 < Par.L2; x2 += 1){
            Ft.tabcorr[x1][x2][0] += Ft.tabr[x2][0]; 
        }

    } 
}

/* Compute correlation */
void Fftwcorr(void){ 
    int x1, x2;
    double renorm = 0, avg2;
    FILE* file;
    char path[300];
    static int error_flag = 0;

    for (x1 = 0; x1 < Par.L1; x1 += 1){ 

        /* It needs to be averaged over Par.L2 */
        /* I don't normalize in a single line to be sure there is no problem with high numbers */
        for (x2 = 0; x2 < Par.L2; x2 += 1){
            Ft.tabcorr[x1][x2][0]/=Par.L2;
            Ft.tabcorr[x1][x2][0]/=(Ft.nava/Ft.davg);
        }
        /*The second Par.L2 is not well understood but is necessary to correspond to the correlation*/

        Ft.avg[x1] = Ft.avg[x1] / Par.L2;
        Ft.avg[x1] = Ft.avg[x1] / (Ft.nava/Ft.davg);
        avg2 = Ft.avg[x1] * Ft.avg[x1]; 
        renorm = Ft.tabcorr[x1][0][0];

        for (x2 = 0; x2 < Par.L2; x2 += 1){
            Ft.tabcorr[x1][x2][0] = (Ft.tabcorr[x1][x2][0]/Par.L2 - avg2) / (renorm/Par.L2 - avg2); /*The "renorm" term rescale the values between 0 and 1*/
        }

        /* To ensure correct values */
        if(Ft.tabcorr[x1][(int)Par.L2/2][0] > 1.0 || Ft.tabcorr[x1][(int)Par.L2/2][0] < -1.0){ 
            if (!error_flag){
                sprintf(path, "%s/Error.txt", Par.path);
                file = fopen(path, "w");
                if (file != NULL){
                    fprintf(file, "Correlation out of bounds at Nava = %ld,  %f\n", (Nav + 1), Ft.tabcorr[x1][Par.L2][0]);
                    fprintf(file, "tabcorr = %f  renorm=%f  avg2 = %f\n", Ft.tabcorr[x1][(int)Par.L2/2][0], renorm, avg2);
                    fclose(file);
                    error_flag = 1; 
                }
            }
            
        }
    }
}

/* Compute space average */
void Fftwspaceavg(void){
    int x1, x2, n, m, i, np;

    Ft.moycorr = (double**)calloc(Par.L2/2, sizeof(double*));
    for (x2 = 0; x2 < Par.L2/2; x2++){
        Ft.moycorr[x2] = (double*)calloc(Ft.nbl, sizeof(double));
    }
    /* printf("TEST MOY BEGIN Ft.moycorr[10] = %f\n", Ft.moycorr[10][0]); */

    for(x2 = 0; x2 < Par.L2/2; x2++){
        m = 0;
        switch(Par.bcmode){
            case 0:
            case 1: /* Make the space average at the center of the system */
                for(i = 0; i < Ft.nbl; i++){
                    /* x1 = (int)((Par.L1 / 2) * ((double)(i + 1) / Ft.nbl)) - 1;  */
                    x1 = (int)((Par.L1) * (double)(i + 1) / Ft.nbl) - 1;

                    if (Ft.nbl == 1) {x1 = (int)(Par.L1/2) - 1;}  /* Avoid to take the side of the system but the center instead */
                    
                    if (x1 == -1){ x1 = 0;} /* Open boundary conditions */

                    for (n = (int)ceil(-Ft.nl / 2.0); n <= (int)floor(Ft.nl / 2); n++){ /* Make the space average */
                        if ( (x1 + n) < 0 || (x1 + n) >= Par.L1 ){continue;} /* Open boundary conditions */
                        m += 1;
                        Ft.moycorr[x2][i] += (Ft.tabcorr[x1 + n][x2][0]);   /* Average over space */
                    }
                    Ft.moycorr[x2][i] =  Ft.moycorr[x2][i] / m; /* Normalise */
                    fprintf(Ft.ftfile, "%f ", Ft.moycorr[x2][i]);
                    m = 0;
                }
                break;
            case 2:/* Make the space average from the border of the system */
                for(i = 0; i < Ft.nbl; i++){
                    x1 = (int)(Par.L1 * (double)(i + 1) / Ft.nbl) - 1;

                    for (n = (int)ceil(-Ft.nl / 2.0); n <= (int)floor(Ft.nl / 2); n++){
                        np = n;
                        if ( (x1 + n) < 0){continue;}
                        if ( (x1 + n) >= Par.L1) {np = 2 * Par.L1 - 2 * x1 - n - 1 ;} /* Mirror coordinates*/
                        m += 1;
                        Ft.moycorr[x2][i] += (Ft.tabcorr[x1 + np][x2][0]);
                    }
                    Ft.moycorr[x2][i] =  Ft.moycorr[x2][i] / m;
                    fprintf(Ft.ftfile, "%f ", Ft.moycorr[x2][i]);
                    m = 0;
                }
                break;
            default: printf("[bcmode]: mode=%d not defined Corr.corrfile,corrfile write\n", Par.bcmode); exit(1);
        }
        fprintf(Ft.ftfile, "\n");
        

        for(i = 0; i < Ft.nbl; i++){  /* Check for the maximum variation in the correlation */
            if (x2 >= ((Par.L2 - 1) / 4) && fabs(Ft.moycorr[x2][i]) > Ft.var[i]){
                Ft.var[i] = fabs(Ft.moycorr[x2][i]);
            }
        }
    }    
    fprintf(Ft.ftfile, "\n\n");
    /* printf("TEST MOY END Ft.moycorr[10] = %f\n", Ft.moycorr[10][0]); */
}

/* Compute the correlation length 
In analysis we now use the exponential fit to calculate the correlation length
*/
void Fftwcorrlen(void){
    int x2, i;

    for(i = 0; i < Ft.nbl; i++){ 
        for (x2 = 2; x2 <= ((Par.L2 - 1) / 4); x2++){ /* Find the first point under the maximum variation and set it as the coorelation length */
            if (Ft.moycorr[x2][i] <= Ft.var[i]){fprintf(Ft.ftlenfile, "%d  ", x2); break;}
            if (x2 == (Par.L2 - 1) / 4) {fprintf(Ft.ftlenfile, "%d  ", x2);}
        }
    }

    fprintf(Ft.ftlenfile,"%ld  %Lf  %Lf\n", Nav+1, zF, StackzF);
    for (x2 = 0; x2 < Par.L2/2; x2++){
        free(Ft.moycorr[x2]);
    }
    free(Ft.moycorr);
}

/* Sum the Correlation function since the beginning and save temporary correlation*/
void temp_Fftwcorr(void){
    int x1, x2;
    double renorm = 0, avg2;
    FILE* file;
    char path[300];
    static int error_flag = 0;

    double temp_avg;
    fftw_complex **temp_tabcorr;
    FILE* temp_file;
    char temp_path[300];

    temp_tabcorr = (fftw_complex**)fftw_malloc(sizeof(fftw_complex*) * Par.L1);
    for (x1 = 0; x1 < Par.L1; x1 += 1){
        temp_tabcorr[x1] = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * Par.L2);
    } 
    sprintf(temp_path, "%s/tf_temp_outputB.txt", Par.path);
    temp_file = fopen(temp_path, "w");
    fprintf(temp_file, "# ATTENTION LES LIGNES ET COLONNES SONT INVERSÉ PAR RAPPORT À tf_outputB.txt\n#Nav = %ld\n", Nav+1);

    
    for (x1 = 0; x1 < Par.L1; x1++){ 
        for (x2 = 0; x2 < Par.L2; x2 ++){
            temp_tabcorr[x1][x2][0]=Ft.tabcorr[x1][x2][0]/Par.L2;
            temp_tabcorr[x1][x2][0]/=(Ft.m/Ft.davg);
        }

        temp_avg = Ft.avg[x1] / Par.L2; /* I do not normalize in a single line to be sure there is no problem with high numbers */
        temp_avg /= (Ft.m/Ft.davg);
        avg2 = temp_avg * temp_avg; 
        renorm = temp_tabcorr[x1][0][0];

        for (x2 = 0; x2 < Par.L2/2; x2 += 1){
            temp_tabcorr[x1][x2][0] = (temp_tabcorr[x1][x2][0]/Par.L2 - avg2) / (renorm/Par.L2 - avg2); /*The "renorm" term rescale the values between 0 and 1*/
            fprintf(temp_file, "%f ", temp_tabcorr[x1][x2][0]);
        }
        fprintf(temp_file, "\n");


        if(temp_tabcorr[x1][(int)(Par.L2/2 - 1)][0] > 1.0 || temp_tabcorr[x1][(int)(Par.L2/2 - 1)][0] < -1.0){
            if (!error_flag){
                sprintf(path, "%s/Error.txt", Par.path);
                file = fopen(path, "w");
                if (file != NULL){
                    fprintf(file, "Correlation out of bounds at Nava = %ld,  %f\n", (Nav + 1), temp_tabcorr[x1][Par.L2][0]);
                    fprintf(file, "tabcorr = %f  renorm=%f  avg2 = %f\n  m = %ld", temp_tabcorr[x1][(int)Par.L2/2][0], renorm, avg2, Ft.m);
                    fclose(file);
                    error_flag = 1; 
                }
            }
            
        }
    }   

    for (x1 = 0; x1 < Par.L1; x1 += 1){
        fftw_free(temp_tabcorr[x1]);
    }
    fftw_free(temp_tabcorr);
    fclose(temp_file);
}

/*Main function fo the correlation computation
Start at ini_ava. We compute the correlation every delta_ava. On a sample of nb_ava BEFORE ini_ava we average the correlation every delta_avg. 
nb_ava/ delta_ava gives the number of configuration we average on. 
*/
void Fftw(void){ 
    int64_t start = Ft.itime - Ft.nava + Ft.ndata * Ft.dtime ;
    int64_t end = Ft.itime + Ft.ndata * Ft.dtime;
    int x1;
    
    /* if (start < 0){start = 0;}
    if (end <= 0){end = 1;} */
    
    if(((Nav + 1) > start) && ((Nav + 1) <= end) && (Nav + 1 <= Ft.endava)){ 
        
        if (Ft.m == 0){
            Fftwinit();
        }   
        
        if (Ft.m < Ft.nava && Ft.m % Ft.davg == 0){ /*Calculation of the covariance*/
            Fftwcov();
        }
        Ft.m += 1;
        /* if ((Nav+1)%1000 ==0){printf("%d, %d, %d\n", Ft.m, (int)(floor(Ft.m/Ft.davg)), (int)(floor(Ft.m/Ft.davg))%Ft.temp_dt_save);} */

        if (Ft.m == Ft.nava){
            printf("Test1\n");
            if(Ft.sw == 1){
                Fftwcorr();
                fprintf(Ft.ftfile, "# Correlation between %ld and %ld\n", start, end);
                Fftwspaceavg();
                Fftwcorrlen();
                Ft.m = 0;
                /* Ft.ndata += 1; */

                fflush(Ft.ftfile);
                fflush(Ft.ftlenfile);

                free(Ft.avg); free(Ft.var);
                fftw_destroy_plan(Ft.plb); fftw_destroy_plan(Ft.plf);
                fftw_free(Ft.tabi); fftw_free(Ft.tabr); fftw_free(Ft.tabq); 
                for (x1 = 0; x1 < Par.L1; x1 += 1){
                    fftw_free(Ft.tabcorr[x1]);
                }
                fftw_free(Ft.tabcorr);
            }
            else if(Ft.sw == 2){
                temp_Fftwcorr();
            }
            Ft.ndata += 1;
            Ft.m = 0;
        }
    }

    if(Nav + 1 == Ft.endava){
       
		if(Ft.sw == 1){fclose(Ft.ftfile); Ft.ftfile = NULL;  fclose(Ft.ftlenfile); Ft.ftlenfile  = NULL;}

        else if(Ft.sw == 2){
            free(Ft.avg); free(Ft.var);
            fftw_destroy_plan(Ft.plb); fftw_destroy_plan(Ft.plf);
            fftw_free(Ft.tabi); fftw_free(Ft.tabr); fftw_free(Ft.tabq); 
            for (x1 = 0; x1 < Par.L1; x1 += 1){
                fftw_free(Ft.tabcorr[x1]);
            }
            fftw_free(Ft.tabcorr);
        }
	}
}


