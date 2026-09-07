#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./analysis.h"

void load_patch(void){ /* load du snapshot et detection des patches */
    char command[_NL];
    char path[_NL];
    FILE *file;
    int temp;
    int n; 
    double dev;
    
    if(Patch.sum_dz >= Patch.delta || Nav == 0 || S > Par.N/4){
        if (Patch.sizes != NULL) {
            free(Patch.sizes);
            Patch.sizes = NULL;
        }
        
        if (Patch.c_snap < Par.shapetime){
            snapsave();
        }
        Patch.c_snap ++;
        
        /* Save current system state */
        sprintf(path, "%s/temp_Snapshot.txt", Par.path);
        file = fopen(path, "w");
        fprintf(file, "#dev\n");
        Bn(dev = Site[n].zm - (Site[n].z + zF - Site[n].zF ); fprintf(file, "%f\n", dev););
        fclose(file);

        /* Patch detection */
        sprintf(command, "python3 %s/patch_detection_script.py %s/temp_Snapshot.txt %f %d %d", Par.path_code, Par.path, Par.nu, Par.L1, Par.L2);
        file = popen(command, "r");
        if(!file) { perror("popen"); printf("Not able to open Python script\n"); exit(1); }

        /* Récupère le numéro lié à chaque patch de chaque site, le nombre de patch, et la taille des patches */
        temp = fread(Patch.id, sizeof(int), Par.N, file);
        temp = fread(&Patch.len_arr, sizeof(int), 1, file);

        Patch.sizes = (int*) calloc(Patch.len_arr, sizeof(int));
        temp= fread(Patch.sizes, sizeof(int), Patch.len_arr, file);

        (void) temp;
        pclose(file);
        Patch.sum_dz = 0;

        /* Bn(printf("Patch id: %d\n", Patch.id[n]););
        printf("len arr%d\n", len_arr);
        for(n = 0; n < len_arr; n++){printf("size: %d\n", Patch.sizes[n]);} */

    }
}

void patch_histo(int nin){
    int id_patch;
    int area_patch;
    int n, m;
    FILE *file;
    char path[_NL];

    /* Save histogram every  Patch.navsave*/
    if ((Nav!= 0) && ((Nav+1) % Patch.navsave == 0)){

        sprintf(path, "%s/patch_ava_histo.txt", Par.path);
        file = fopen(path, "w");
        for(m = 0; m < Patch.nbins_avasize; m++){
            for(n = 0; n < Patch.nbins_area; n++){
                fprintf(file, "%ld  ", Patch.histos[n][m]);
            }
            fprintf(file, "\n");
        }
        fclose(file);
    }

    /* Fait l'histogram des avalanches en fonction de la taille du patch */
    id_patch = Patch.id[nin];
    if (id_patch != 0){
        area_patch = Patch.sizes[id_patch];
        if (area_patch > S){
            for(n = 0; n <= Patch.nbins_area; n++){
                if ((Patch.bins_area[n]<= area_patch) && (area_patch < Patch.bins_area[n+1])){
                    for(m = 0; m <= Patch.nbins_avasize; m++){
                        if ((Patch.bins_avasize[m]<= S) && (S < Patch.bins_avasize[m+1])){
                            Patch.histos[n][m]++;

                            if ((S> 2* area_patch) && (area_patch >50)){
                                printf("%d, %d, %.2f, %.2f, %.2f, %.2f\n", S, area_patch, Patch.bins_area[n],Patch.bins_area[n+1], Patch.bins_avasize[m], Patch.bins_avasize[m+1]);
                            }
                            return;
                        }
                    }
                }    
            }
        }
    }

    
}