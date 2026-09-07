#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./heap.h"
#include"./noise.h"

void add_noise(void){ /* add_noise in the system */
	/* 
	mode == 0: get a value to keep the same epicenter as if there were no noise added
	mode == 1: get a random value bewteen 0 and zm;
	*/
	int *indices = (int*) calloc(Par.N, sizeof(int));
	int n, i, nin, temp;
	double min, max;
	sitest *sit, *sit2;

	long double dev, devnin;

    /* Copy the initial state */
    if (Noise.nloop !=0 && Noise.cloop == 0){
        for (n = 0; n < Par.N; n++){	/* Could be improve using mempcy()? */
            Noise.init_Site[n] = Site[n];
            Noise.init_Heap[n] = Heap[n];
        }
        Noise.init_zF = zF;
        Noise.init_StackzF = StackzF;
    }   

	/* Built an array with all the indices*/
	Bn(indices[n] = n;);
	/* Randomize the indices */
	Bn(
        i = randim(Par.N);
        temp = indices[n];
        indices[n] = indices[i];
        indices[i] = temp;
	);
	/* This indices array avoid to pick the same site twice when we add noise in the system*/

	/* Pick a site from the indices array and noise it */
	for (i = 0; i < Noise.nb_noise; i++){
		n = indices[i];
		sit = &(Site[n]);

		/* Compute the classic dz */
		nin = Heap[0].iSite;	
		sit2 = &(Site[nin]);
		devnin = sit2->zm - (sit2->z + zF - sit2->zF);
		
		dev = -1;
		if(Noise.noise_mode){
			sit->z = 0;
			while (dev < 0){ /* HAS TO BE REMOVED */
				sit->zF = zF - rand01() * sit->zm;	/* Draw a new value */
				dev = sit->zm - (sit->z + zF - sit->zF);
			}
			if(sit->mod == 0){Modifsys[Nbmod] = n; Nbmod++; sit->mod = 1;}

			if (dev > 1 || dev < 0 || dev < devnin || sit->zm == 0){/* To ensure correct values */
				printf("Error in noise fct: %d  %.10Lf  %d  %.10Lf  %10.Lf\n", n, dev, nin, devnin, zF ); 
				printf("%.10Lf  %.10Lf  %.10Lf  %.10Lf  %.10Lf  %.10Lf\n", sit->z, sit2->zm, sit2->z, sit2->zF, sit->zm, sit->zF );
				exit(0);
			}
		}else{
			
			while(nin == n){
				n = randim(Par.N);
				sit=&(Site[n]);
			}

			min = sit2->zm - (sit2->z - sit2->zF) - sit->zm;	/* To keep the same epicenter */
			max = zF;
			sit->z = 0;

			while(dev < 0){/* HAS TO BE REMOVED */
				sit->zF = (rand01() * (max - min)) + min;/* Draw a new value */
				dev = sit->zm - (sit->z + zF - sit->zF);
			}
			if(sit->mod == 0){Modifsys[Nbmod] = n; Nbmod++; sit->mod = 1;}
			
			if (dev > 1 || dev < 0 || dev < devnin || sit->zm == 0){
				printf("Error in noise fct: %d  %.10Lf  %d  %.10Lf  %.10Lf\n", n, dev, nin, devnin, zF );
				printf("%.10Lf  %.10Lf  %.10Lf  %.10Lf  %.10Lf  %.10Lf\n", sit->z, sit2->zm, sit2->z, sit2->zF, sit->zm, sit->zF );
			 	exit(0);
			}
		}

		Site2[n] = Site[n];
	}

	while(Nbmod != 0){
		reorganize();
	}
	free(indices);
}

void restart_noise(void){
    int n;

	FILE *noisefile;
	char buffer[_NL]; 
	char path[300];
    int offset = 0;
	double dev;
	sitest *sit;

	

    if (Nav == Noise.nbava){

		sprintf(path, "%s/Snapshots/loop%d_outputB.txt", Par.path, (Noise.cloop + 1));
		noisefile = fopen(path, "w");
		fprintf(noisefile, "#dev\n");

		/* Save the configuration at the end of the loop and Reload the initial configuration */
        Bn(
			sit = &(Site[n]);
			
			dev = sit->zm - (sit->z + zF - sit->zF ); 
			fprintf(noisefile, "%.10f\n", dev);

			Noise.dev[n] += dev;

			if (Noise.cloop != (Noise.nloop - 1)){
				Site[n] = Noise.init_Site[n];
				Site2[n] = Noise.init_Site[n];
				Heap[n] = Noise.init_Heap[n];
			}
		);    
		fclose(noisefile);

		/* Draw new noise values */
		if (Noise.cloop != (Noise.nloop - 1)){ 
			zF = Noise.init_zF;
			StackzF = Noise.init_StackzF;
			Nav = 0;

			add_noise();
		}
		Noise.cloop ++;
    }

	/* Enregistre les valeurs moyenne de chaque site */
	if (Noise.cloop == Noise.nloop){

		sprintf(path, "%s/Snapshots/Snapshot_avg.txt", Par.path);
		noisefile = fopen(path, "w");

		fprintf(noisefile, "#dev\n");

		Bn(
			offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%.10f\n", Noise.dev[n]/Noise.nloop);
			if (offset >= sizeof(buffer) - 200) {
				buffer[offset+1] = '\0';
				fprintf(noisefile, "%s", buffer);
				offset = 0;
			}
		);

		Noise.noise_switch = 0;
		fclose(noisefile);
	}

}