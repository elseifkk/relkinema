TARG = ucalc

CC = gcc
FC = gfortran46
LD = gfortran46

#
CFLAGS = -c -Wall
FFLAGS = -msse -msse2 -msse3 -m3dnow -mfpmath=sse \
-fdefault-real-8 -fno-range-check \
-pipe -fcray-pointer -fbackslash -masm=att\
-c -Wall -cpp -fdefault-integer-8
OMPFLAGS = -fopenmp
LDFLAGS =

#
FFLAGS += -s -fomit-frame-pointer -O3

.PHONY: clean all

.SUFFIXES:
.SUFFIXES: .o .s .f90 .mod .c

all: $(TARG)

.c.o:
	$(CC) $(CFLAGS) $<

.f90.o:
	$(FC) $(FFLAGS) $<

.f90.mod:
	$(FC) $(FFLAGS) $<

ucalc: ucalc.o rpn.mod
	$(LD) $(LDFLAGS) ucalc.o -o ucalc

clean:
	rm -f *.o *.mod
