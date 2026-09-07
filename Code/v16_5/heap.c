#include"../../fct/fct.h" 
#include"./avafct.h"
#include"./heap.h"

void swap(int i1, int i2){ /* Swap two elements in the heap */
    int tempindex = -1;

    heapst temp = Heap[i1];
    Heap[i1] = Heap[i2];
    Heap[i2] = temp;

    /* Swap the index in the heap and in the system */
    tempindex = Site[Heap[i1].iSite].iHeap;
    Site[Heap[i1].iSite].iHeap = Site[Heap[i2].iSite].iHeap;
    Site[Heap[i2].iSite].iHeap = tempindex;
    
    /* Do it also in the parallel system */
    Site2[Heap[i1].iSite].iHeap = Site2[Heap[i2].iSite].iHeap;
    Site2[Heap[i2].iSite].iHeap = tempindex;
}

void insert(long double key, int index){ /* Insert an element in the heap */
    if( Hcount < Par.N){
        Heap[index].value = key;
        Heap[index].iSite = index;
        Site[index].iHeap = index;
        heapify_bottom_top(index);
        Hcount++;
    }
}

void heapify_bottom_top(int index){ /* To exchange the elements in the heap from bottom to top*/
    int parent_node = (index-1)/2;

    if(Heap[parent_node].value > Heap[index].value){
        swap(parent_node, index);
        heapify_bottom_top(parent_node);
    }
}

void heapify_top_bottom(int parent_node){   /* To exchange the elements in the heap from top to bottom*/
    int left = parent_node*2+1;
    int right = parent_node*2+2;
    int min = parent_node;

    /* Check if the children are in the heap */
    if(left >= Hcount || left <0)
        left = -1;
    if(right >= Hcount || right <0)
        right = -1;

    /* Check which child is smaller and exchange with the smaller */
    if(left != -1 && Heap[left].value <= Heap[parent_node].value){
        if (Heap[left].value == Heap[parent_node].value){
            if (Heap[left].iSite < Heap[parent_node].iSite){    /* Just to make a choice and make it deterministic*/
                min = left;
            }
        }else{ min=left; }
    }
    if(right != -1 && Heap[right].value <= Heap[min].value){
        if (Heap[right].value == Heap[min].value){
            if (Heap[right].iSite < Heap[min].iSite){
                min = right;
            }
        }else{ min=right; }
    }
    if(min != parent_node){
        swap(parent_node, min);
        heapify_top_bottom(min);
    }
}

void printheap(void){
    int i;
    printf("____________Print Heap_____________\n");
    for(i = 0; i < Hcount; i++){
        printf("%.20Lf -> \n",Heap[i].value - zF);
    }
    printf("END\n");
    for(i = 0; i < Hcount; i++){
        printf("%d -> ",Heap[i].iSite);
    }
    printf("END\n");
}

void printsys(void){
    int n;
    printf("________Print Sys________\n");
    for(n = 0; n<Hcount; n++){
      printf("%.20Lf ->\n", Site[n].zm - (Site[n].z - Site[n].zF));
    }
    printf("END\n");
    for(n = 0; n<Hcount; n++){
      printf("%d ->", Site[n].iHeap);
    }
    printf("END\n");
    /* printf("________Print Sys2________\n");
    for(n = 0; n<Hcount; n++){
      printf("%f ->", Site2[n].zm - (Site2[n].z - Site2[n].zF));
    }
    printf("END\n");
    for(n = 0; n<Hcount; n++){
      printf("%d ->", Site2[n].iHeap);
    }
    printf("END\n");
 */
}

void verifHeap(void){ /* Verify that min is at the root of the heap */
    int n;
    long double min = 1e7;
    for (n = 0; n < Par.N; n++){
        if (min >= Heap[n].value){
            min = Heap[n].value;
        } 
    }

    if(min != Heap[0].value){printf("Diff min Heap :%ld\n Min = %Lf, Heap[0] = %Lf\n", Nav, min, Heap[0].value);exit(0);}
}

void rescale(void){ /* Rescale the energy in the system to keep a good precision */
    int n; 
    long double temp = 0;
    
    if((Nav % 100000) == 0 ){
        temp = zF - 1;
        if (temp > 1){
        /* if (zF > 2){ 
            temp = zF - 1; */
            StackzF += temp;
            for(n = 0; n < Par.N; n++){
                Heap[n].value = Heap[n].value - temp;
                Site[Heap[n].iSite].zF = Site[Heap[n].iSite].zF - temp;
                Site2[Heap[n].iSite].zF = Site2[Heap[n].iSite].zF - temp;
            }
            zF = zF - temp;
        }
    }

}

void reorganize(void){ /*Reorganize the heap after an avalanche*/
    int i = 0;
    long double newval = 0;
    long double var = 0;
    i = Modifsys[Nbmod-1];  /* Take the last mofified element */

    newval = Site2[i].zm - (Site2[i].z - Site2[i].zF); /* Compute the new value */
    var = Heap[Site2[i].iHeap].value - newval;  /* Compute the difference with the old value */
    Heap[Site2[i].iHeap].value = newval;    /* Set the new value */
    
    /* Rearange in the heap */
    if (var < 0){
    heapify_top_bottom(Site2[i].iHeap);
    }else if (var > 0){
    heapify_bottom_top(Site2[i].iHeap);
    }
    Modifsys[Nbmod-1] = -1;
    Nbmod--;
    Site2[i].mod = 0;

}

void minheap(void){/*Search for the min force value in the heap (not deviation here )*/
    int n, lvl = 0;
    Energy.min = 2;

    lvl = (int)ceil(log2(Par.N)) - 1;

    for (n = pow(2,lvl); n < Par.N; n++){
        if (Energy.min > (Site[Heap[n].iSite].zm - (Heap[n].value - zF))){ 
            Energy.min = (Site[Heap[n].iSite].zm - (Heap[n].value - zF));
            if (Energy.min == 0){
                break;
            }
        }
    }
}