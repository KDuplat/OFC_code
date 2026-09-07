#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./analysis.h"
#include"./random.h"


void prediction(int nin){
    int n;
    double dev;
    FILE* file;
    char path[300];

    int trash1; double trash2;

    /* On fait 2 classes: taille 1 , entre  size_th[0] et size_th[1] et au dessus de size_th[1].
    Si petite ava, près un temps assez long on enrgistre l'avalanche.
    Si déjà enregistrer, on regarde si on a attendu assez longtemps sans tomber sur une grande. Si c'est le cas on test si on peut trouver une grande avalanche.
    Si l'avalanche est grande on fait le test directement.*/
    if (S < Prediction.size_th[1]){
		Prediction.Nava_last_large++;
		Prediction.N_ava_s1++;
        Prediction.N_ava_s2++;
        
        if ((Prediction.Nava_last_large > Prediction.mean_t/2)){
            /* Size 1 avalanche */
            if (Prediction.c1 < Prediction.nb_snap){
                if ((S == 1) && (Prediction.sw_s1 == 1)){   /* If no avalanche saved, save one */
                    memcpy(Prediction.Site_s1, Prediction.Site_tempo, Par.N*sizeof(sitest));
                    Prediction.N_ava_s1 = 0;
                    Prediction.sw_s1 = 0;
                    Prediction.Nin_s1 = nin; 
                    Prediction.zF_s1 = zF;
                    Prediction.stack_zF_s1 = StackzF;
                    Prediction.Nav_s1 = Nav + 1;
                    Prediction.s1 = S;
                }
                else if ((Prediction.N_ava_s1 > (int)(Prediction.mean_t/4)) && (Prediction.sw_s1 == 0)){    /* If far enough from large avalanche, write the configuration */
                    
                    fprintf(Prediction.files1,  "%ld  %d  %d\n", Prediction.Nav_s1, Prediction.s1, Prediction.Nin_s1);

                    sprintf(path, "%s/Snapshots/nav%ld.txt", Par.path, Prediction.Nav_s1);
                    file = fopen(path, "w");
                    fprintf(file, "#dev\n");
                    Bn(dev = Prediction.Site_s1[n].zm - (Prediction.Site_s1[n].z + Prediction.zF_s1 - Prediction.Site_s1[n].zF ); fprintf(file, "%f\n", dev););
                    fclose(file);

                    Prediction.sw_s1 = 1;   /* Can save a new avalanche */
                    Prediction.N_ava_s1 = 0;    /* Waiting counter reset */

                    if(Prediction.sw == 2){ /* Search for the largest possible avalanche */
                        random_toppling_nostop(Prediction.Nin_s1, Prediction.zF_s1, Prediction.stack_zF_s1, Prediction.Nav_s1,
                                               Prediction.Site_s1, Prediction.s1, Random.file1, &trash1, &trash2);
                    } 
                    Prediction.c1++;
                    /* if ((Prediction.c1 % 100) == 0){fflush(Random.file1);} */ /* Ne marche pas avec les .gz */
                    
                }
            }
            /* Medium size avalanche */
            if (Prediction.c2 < Prediction.nb_snap){
                if ((S > Prediction.size_th[0]) && (Prediction.sw_s2 == 1)){
                    memcpy(Prediction.Site_s2, Prediction.Site_tempo, Par.N*sizeof(sitest));
                    Prediction.N_ava_s2 = 0;
                    Prediction.sw_s2 = 0;
                    Prediction.Nin_s2 = nin; 
                    Prediction.zF_s2 = zF; 
                    Prediction.stack_zF_s2 = StackzF;
                    Prediction.Nav_s2 = Nav + 1;
                    Prediction.s2 = S;  
                }
                else if ((Prediction.N_ava_s2 > Prediction.mean_t/4) && (Prediction.sw_s2 == 0)){
                    fprintf(Prediction.files2,  "%ld  %d  %d\n", Prediction.Nav_s2, Prediction.s2, Prediction.Nin_s2);

                    sprintf(path, "%s/Snapshots/nav%ld.txt", Par.path, Prediction.Nav_s2);
                    file = fopen(path, "w");
                    fprintf(file, "#dev\n");
                    Bn(dev = Prediction.Site_s2[n].zm - (Prediction.Site_s2[n].z + Prediction.zF_s2 - Prediction.Site_s2[n].zF ); fprintf(file, "%f\n", dev););
                    fclose(file);

                    if(Prediction.sw == 2){
                        random_toppling_nostop(Prediction.Nin_s2, Prediction.zF_s2, Prediction.stack_zF_s2, Prediction.Nav_s2,
                                               Prediction.Site_s2, Prediction.s2, Random.file2, &trash1, &trash2);
                    } 

                    Prediction.sw_s2 = 2;
                    Prediction.N_ava_s2 = 0;
                    Prediction.c2++;

                    /* if ((Prediction.c2 % 100) == 0){fflush(Random.file2);} */ /* Ne marche pas avec les .gz */
                }
            }
        }
	}   /* large size avalanche */
    else if (S >= Prediction.size_th[1]){
        /* Reset to zero waiting counter*/
        Prediction.Nava_last_large = 0;
        Prediction.sw_s1 = 1;
        Prediction.sw_s2 = 1;
        Prediction.N_ava_s1 = 0;
        Prediction.N_ava_s2 = 0;

        if (Prediction.c3 < Prediction.nb_snap){
            /*************Save the shape *************/
            fprintf(Prediction.files3, "%ld  %d  %d\n", Nav + 1, S, nin);

            /* Save the snapshot before the ava*/
            sprintf(path, "%s/Snapshots/nav%ld.txt", Par.path, Nav + 1);
            file = fopen(path, "w");
            fprintf(file, "#dev\n");
            Bn(dev = Prediction.Site_tempo[n].zm - (Prediction.Site_tempo[n].z + zF - Prediction.Site_tempo[n].zF ); fprintf(file, "%f\n", dev););
            fclose(file);

            if(Prediction.sw == 2){
                random_toppling_nostop(nin, zF, StackzF, (Nav + 1), Prediction.Site_tempo, S, Random.file3, &trash1, &trash2);
            }

            Prediction.c3++;
            /* if ((Prediction.c3 % 100) == 0){fflush(Random.file3);} */ /* Ne marche pas avec les .gz */
            
        }
    }

    
    if (Prediction.c1 == Prediction.nb_snap){
        fclose(Prediction.files1); 
        Prediction.files1 = NULL; 
        Prediction.c1++; 
        /* if (Prediction.sw == 2){printf("Test\n"); gzclose(Random.file1); Random.file1 = NULL;} */
    } 
    if (Prediction.c2 == Prediction.nb_snap){fclose(Prediction.files2); Prediction.files2 = NULL; Prediction.c2++;}
    if (Prediction.c3 == Prediction.nb_snap){fclose(Prediction.files3); Prediction.files3 = NULL; Prediction.c3++;}

    if ((Prediction.c1 == (Prediction.nb_snap + 1)) &&
        (Prediction.c2 == (Prediction.nb_snap + 1)) && 
        (Prediction.c3 == (Prediction.nb_snap + 1))){

        Nav = Par.navmax - 1;
        Prediction.sw = 0;
    }
    /* if ((Random.sw &&
        (Prediction.c1 == (Prediction.nb_snap + 1)) &&
        (Prediction.c2 == (Prediction.nb_snap + 1)) && 
        (Prediction.c3 == (Prediction.nb_snap + 1))) ||

        (!Random.sw &&
        (Prediction.c1 == (Prediction.nb_snap + 1)) &&
        (Prediction.c3 == (Prediction.nb_snap + 1)))
        ){

        Shape.sw = 0;
        Nav = Par.navmax - 1;
    } */
}


void prediction2(int nin){ /* Fonction temporaire pour avoir les configurations avant et après une grande avalanche */
    int n;
    double dev;
    FILE* file;
    char path[300];

    /* Si petite ava, près un temps assez long on enrgistre l'avalanche.
    Si déjà enregistrer, on regarde si on a attendu assez longtemps sans tomber sur une grande. Si c'est le cas on test si on peut trouver une grande avalanche.
    Si l'avalanche est grande on fait le test directement.*/
    
    if (S >= Prediction.size_th[1]){

        if (Prediction.c3 < Par.shapetime){
            /*************Save the shape *************/
            fprintf(Prediction.files3, "%ld  %d  %d\n", Nav + 1, S, nin);

            /* Save the snapshot before the ava*/
            sprintf(path, "%s/Snapshots/nav%ld_before.txt", Par.path, Nav + 1);
            file = fopen(path, "w");
            fprintf(file, "#dev\n");
            Bn(dev = Prediction.Site_tempo[n].zm - (Prediction.Site_tempo[n].z + zF - Prediction.Site_tempo[n].zF ); fprintf(file, "%f\n", dev););
            fclose(file);

            snapsave();
            Prediction.c3++;
            /* if ((Prediction.c3 % 100) == 0){fflush(Random.file2);} */
            
        }
    }
    
    if (Prediction.c3 == Prediction.nb_snap){fclose(Prediction.files3); Prediction.files3 = NULL; Prediction.c3++;}
    
    if (Prediction.c3 == (Prediction.nb_snap + 1)){
        Shape.sw = 0;
        Nav = Par.navmax - 1;
    }
}




