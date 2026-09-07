

CC = gcc 
CFLAGS=   -std=gnu89 -Wall -pedantic -O3 -funroll-loops -I. 

###

PC=/data/detcheve/info/cprog/prog/pc


FHr = $(wildcard fct-*.h)
FC := $(FHr:%.h=%.c)	
FO := $(FHr:%.h=%.o)	

FH  =  $(FHr) fct.h

fct-above.o   :   fct-above.c     $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-above_pc.c     -o  $@
fct-cal.o     :   fct-cal.c       $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-cal_pc.c       -o  $@
fct-fnum.o    :   fct-fnum.c      $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-fnum_pc.c      -o  $@
fct-io.o      :   fct-io.c        $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-io_pc.c        -o  $@
fct-hist.o    :   fct-hist.c      $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-hist_pc.c      -o  $@
fct-lmp.o     :   fct-lmp.c       $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-lmp_pc.c       -o  $@
fct-lattice.o :   fct-lattice.c   $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-lattice_pc.c   -o  $@
fct-list.o    :   fct-list.c      $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-list_pc.c      -o  $@
fct-matrix.o  :   fct-matrix.c    $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-matrix_pc.c    -o  $@
fct-nbi.o     :   fct-nbi.c       $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-nbi_pc.c       -o  $@
fct-process.o :   fct-process.c   $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-process_pc.c   -o  $@
fct-rand.o    :   fct-rand.c      $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-rand_pc.c      -o  $@
fct-string.o  :   fct-string.c    $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-string_pc.c    -o  $@
fct-vec.o     :   fct-vec.c       $(FH);    $(PC)  $< ;  $(CC) $(CFLAGS) -c  fct-vec_pc.c       -o  $@

fct-all: $(FO) 

###


clean :
	rm -f  *.o *~ *_pc.c
