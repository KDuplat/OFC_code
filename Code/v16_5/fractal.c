#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./analysis.h"
#include <stdbool.h>
/* #include"./heap.h" */

void fractalboxes(double *slope, double *chi2){
    int nblvl = log2(sqrt(Par.N)) + 1;
    int *fractalbox = (int*) calloc(nblvl, sizeof(int)); 
    int x1,x2, x1p, x2p, i, n;
    double a, b;
    int d; /* length of the box */
    int index_dim=1; /* Does not start at 0 because it will be the size of the avalanche */
    int nmax = 0;
    bool switch_incr;

    double sumX=0, sumY=0, sumXY=0, sumX2=0;
    double S1=0, S12 = 0, S2=0, S22 = 0;
    double *logy; double *logx; 

    char str[_NL];

    fractalbox[0] = S;
    for (i = 1; i<nblvl; i++){fractalbox[i]=0; }/* Why not malloc? */

    for (d = 2; d <= sqrt(Par.N)/2 + 1; d = d * 2){ /* For each box size */
        for(x1 = 0; x1 < Par.L1; x1 += d){  /* Check each box */
            for(x2 = 0; x2 < Par.L2; x2 += d){
                switch_incr = false;
                for(x1p = 0; x1p < d && !switch_incr; x1p++){ /* For each element in the box */
                    for (x2p = 0; x2p < d && !switch_incr; x2p++){
                        if (InBox(x1+x1p,x2+x2p)){  /* Avoid to take element outside the avalanche zone */
                            n = No(x1+x1p,x2+x2p);
                            if(Site[n].top >=1){
                                fractalbox[index_dim] += 1; /* Count the number of box full */
                                switch_incr = true; /* Avoid to continue to check the box if avalanche already detected in it*/
                            }
                        }
                    }
                }
            }
        }
        if (fractalbox[index_dim] == 1){nmax = index_dim - 1; break;}   /* Break the loop if the avalanche is contained in a single box */
        index_dim++;
    }


    nmax = (nmax == 0 && (fractalbox[index_dim] >1)) ? index_dim : (index_dim - 1); /* Set the max index for the linear regression*/
    
    logx = (double*) calloc(nmax, sizeof(double));
    logy = (double*) calloc(nmax, sizeof(double));

    *slope = 0;
    *chi2 =  0;
    str[0] = '\0'; /* IMPORTANT : vide la chaîne au début*/

    if (Fractal.sw_boxlist && (Nav + 1) >= Comp.limtrans && (Nav + 1) < (Comp.limtrans + Par.nbdata)){
        for (i=0; i<nblvl; i++){
            sprintf(str+strlen(str), "%d  ",fractalbox[i]);
        }
        sprintf(str+strlen(str), "%d\n",nmax);
        gzwrite_str(Fractal.boxfile, str);
    }

    if (nmax>2) {   /* If there is at least 3 sizes of box */
        
        for (i=0; i<nmax; i++){

            logy[i]=log2(fractalbox[i]);
            logx[i]=log2(2.)*(i);
        
            sumY+=logy[i];
            sumX+=logx[i];
            sumXY+=logy[i]*logx[i];
            sumX2+=logx[i]*logx[i];
        }
    
        b = (nmax*sumXY-sumX*sumY)/(nmax*sumX2-sumX*sumX); /* Linear regression*/ 
        a = (sumY-b*sumX)/nmax;
        
        for (i=0; i<nmax; i++){ /* Standard error of the slope*/ 
            S1 = (logy[i]-(a+b*logx[i]));
            S12 +=  (S1 * S1);
            S2 = (logx[i]-sumX/nmax); /* Variance*/ 
            S22 += S2 * S2;
        }
        
        *chi2=sqrt((1.0/(nmax-2)) * S12/S22);
        *slope=b;   

        /* linear_regression(fractalbox, nmax, slope, chi2); */ /* A TESTER */
    }
    free(logx); free(logy); free(fractalbox);
}


void linear_regression(int *fractalbox, int nmax, double *slope, double *chi2){
    int i;
    double sumX=0, sumY=0, sumXY=0, sumX2=0, a = 0, b = 0;
    double S1=0, S12 = 0, S2=0, S22 = 0;
    double *logy; double *logx;

    logx = (double*) calloc(nmax, sizeof(double));
    logy = (double*) calloc(nmax, sizeof(double));

    for (i=0; i<nmax; i++){

            logy[i]=log2(fractalbox[i]);
            logx[i]=log2(2.)*(i);
        
            sumY+=logy[i];
            sumX+=logx[i];
            sumXY+=logy[i]*logx[i];
            sumX2+=logx[i]*logx[i];
        }
    
        b = (nmax*sumXY-sumX*sumY)/(nmax*sumX2-sumX*sumX); /* Linear regression */
        a = (sumY-b*sumX)/nmax;
        
        for (i=0; i<nmax; i++){ /* Standard error of the slope */
            S1 = (logy[i]-(a+b*logx[i]));
            S12 +=  (S1 * S1);
            S2 = (logx[i]-sumX/nmax); /* Variance */
            S22 += S2 * S2;
        }
        
        *chi2=sqrt((1/(nmax-2)) * S12/S22);
        *slope=b;   

}
