#define MAIN  


#include"../../fct/fct.h" 
#include"./avafct.h" 
#include"./analysis.h"
#include"./heap.h"
#include"./noise.h"	

int main(int argc , char *argv[]){ 
    signal(SIGTERM, cleanup_and_exit);
    parameters(argc, argv[1]);  
    memoryallocation();
    initialize();  
    
    do{ 
        Time.start1 = clock();
        /* oneavalancheEn(); */
        oneavalancheB(); 
        Nav++;
        Time.end1 = clock();
        savetime();
        savesys();
        if (Noise.noise_switch && Noise.nloop != 0) restart_noise();
    } while(Nav<Par.navmax);    
    
    file_closed();
    free_fct();
    fprintf(stderr,"B: zFfinal=%.10Lf \n",zF + StackzF); printf("#\n\n");
    
    exit(0);
}