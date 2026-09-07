
CC  = gcc 

#CFLAGS=   -std=gnu89 -Wall -pedantic -O3 -funroll-loops -I.
# CFLAGS=   -std=gnu89  -Wall -pg  -pedantic  -O0 -funroll-loops -I.  -Wformat-overflow=0 	#Profiling
CFLAGS=   -std=gnu89  -Wall  -pedantic  -O3 -funroll-loops -I.  -Wformat-overflow=0
# above: first line for home, second line for lab
# new: it seems

LDFLAGS= -lm
LDFFTFLAGS= -lfftw3 -lz -lm  

#-I/usr/local/include 
###

#PC = /data/detcheve/info/cprog/prog/pc
#PC=../../pc

#FP = /data/detcheve/info/cprog/fct
FP=../../fct

FHr = $(FP)/fct-above.h $(FP)/fct-cal.h $(FP)/fct-clu.h $(FP)/fct-fnum.h $(FP)/fct-hist.h $(FP)/fct-io.h $(FP)/fct-lmp.h $(FP)/fct-lattice.h $(FP)/fct-list.h $(FP)/fct-matrix.h $(FP)/fct-nbi.h $(FP)/fct-process.h $(FP)/fct-rand.h $(FP)/fct-string.h  $(FP)/fct-vec.h

FC := $(FHr:%.h=%.c)	
FO := $(FHr:%.h=%.o)	

FH  =  $(FHr) $(FP)/fct.h

FA = $(FH) $(FC) $(FO) 


$(FP)/fct-above.o   :   $(FP)/fct-above.c     $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-above_pc.c     -o  $@
$(FP)/fct-cal.o     :   $(FP)/fct-cal.c       $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-cal_pc.c       -o  $@
$(FP)/fct-clu.o     :   $(FP)/fct-clu.c       $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-clu_pc.c       -o  $@
$(FP)/fct-fnum.o    :   $(FP)/fct-fnum.c      $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-fnum_pc.c      -o  $@
$(FP)/fct-io.o      :   $(FP)/fct-io.c        $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-io_pc.c        -o  $@
$(FP)/fct-hist.o    :   $(FP)/fct-hist.c      $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-hist_pc.c      -o  $@
$(FP)/fct-lmp.o     :   $(FP)/fct-lmp.c       $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-lmp_pc.c       -o  $@
$(FP)/fct-lattice.o :   $(FP)/fct-lattice.c   $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-lattice_pc.c   -o  $@
$(FP)/fct-list.o    :   $(FP)/fct-list.c      $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-list_pc.c      -o  $@
$(FP)/fct-matrix.o  :   $(FP)/fct-matrix.c    $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-matrix_pc.c    -o  $@
$(FP)/fct-nbi.o     :   $(FP)/fct-nbi.c       $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-nbi_pc.c       -o  $@
$(FP)/fct-process.o :   $(FP)/fct-process.c   $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-process_pc.c   -o  $@
$(FP)/fct-rand.o    :   $(FP)/fct-rand.c      $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-rand_pc.c      -o  $@
$(FP)/fct-string.o  :   $(FP)/fct-string.c    $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-string_pc.c    -o  $@
$(FP)/fct-vec.o     :   $(FP)/fct-vec.c       $(FH);    #$(PC)  $< ;  $(CC) $(CFLAGS) -c  $(FP)/fct-vec_pc.c       -o  $@

FI = avafct.c  avafct.h  analysis.c  analysis.h  heap.c  heap.h  corr.c  corr.h  noise.c  noise.h  fractal.c  fractal.h\
	prediction.c  prediction.h  random.c  random.h  patch.c  patch.h


avafct.o     :   avafct.c  avafct.h $(FH);   $(CC) $(CFLAGS) -c  avafct.c  -o  $@
fct-wrt.o    :   analysis.c  analysis.h $(FH);   $(CC) $(CFLAGS) -c  analysis.c  -o  $@
heap.o       :   heap.c  heap.h $(FH);   $(CC) $(CFLAGS) -c  heap.c  -o  $@
corr.o       :   corr.c  corr.h $(FH);   $(CC) $(CFLAGS) -c  corr.c  -o  $@
noise.o      :   noise.c  noise.h $(FH);   $(CC) $(CFLAGS) -c  noise.c  -o  $@
fractal.o      :   fractal.c  fractal.h $(FH);   $(CC) $(CFLAGS) -c  fractal.c  -o  $@
prediction.o      :   prediction.c  prediction.h $(FH);   $(CC) $(CFLAGS) -c  prediction.c  -o  $@
random.o	 :	random.c  random.h $(FH);   $(CC) $(CFLAGS) -c  random.c  -o  $@
patch.o : 	patch.c  patch.h $(FH);   $(CC) $(CFLAGS) -c  patch.c  -o  $@

FIs = avafct.o analysis.o heap.o corr.o noise.o fractal.o prediction.o random.o patch.o
## Note behind bdd (../fct) functions,  I add avafct.h/o
ava.o : ava.c $(FH) $(FI)
	$(CC)  $(CFLAGS) -c  ava.c -o $@
ava :	ava.o  $(FA)  $(FIs)  
	$(CC)  $(CFLAGS) $(FO)  $(FIs)   $<  -o $@ $(LDFFTFLAGS) ; make clean; 


clean :
	rm -f *.o *~  *_pc.c ;

cleanfct :
	rm -f $(FP)/*.o $(FP)/*~  $(FP)/*_pc.c ;
