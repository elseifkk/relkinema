BINTARG = rkcore
LIBTARG = librkcore.a librkcore.so.0.0.0
LIBDIR = /usr/local/lib
BINDIR = /usr/local/bin

#
FC  = /usr/local/bin/gfortran
CC  = /usr/local/bin/gcc
LD  = /usr/local/bin/gfortran
ASM = yasm
AR  = ar

#
CFLAGS = -c -Wall
FFLAGS = -c -Wall -cpp -fbounds-check\
 -fcray-pointer -fbackslash\
 -g -fPIC
AFLAGS = -g stabs
ARFLAGS = rv
LDFLAGS=-L/usr/local/lib -static-libgfortran
LD_RUN_PATH=/usr/local/lib
LD_LIBRARY_PATH=/usr/local/lib

.ifdef _NETWALKER_
    FFLAGS += -D_NO_REAL10_ -D_NO_REAL16_ -D_NO_ASM_ -D_USE32_
    OBJ = rkcore.o
.else
    USE64 != uname -m | grep -e x86_64 -e amd64 | wc -c
.   if( $(USE64) == 0 )
        FFLAGS += -D_USE32_
        AFLAGS += -f elf32 -m x86 -D_USE32_
.   else
        FFLAGS += -fdefault-integer-8
        AFLAGS += -f elf64 -m amd64
.   endif
    OBJ = rkcore.o
.endif

#
.PHONY: clean all install install-lib install-bin

.SUFFIXES:
.SUFFIXES: .o .s .f90 .mod .c

all: $(LIBTARG) $(BINTARG)

.s.o:
	$(ASM) $(AFLAGS) $<

.c.o:
	$(CC) $(CFLAGS) $<

.f90.o:
	$(FC) $(FFLAGS) $<

.f90.mod:
	$(FC) $(FFLAGS) $<

librkcore.a: $(OBJ)
	$(AR) $(ARFLAGS) $@ $(.ALLSRC)

librkcore.so.0.0.0: $(OBJ)
	$(LD) $(LDFLAGS) -shared -Wl,-soname,librkcore.so.0 -o $@ $(.ALLSRC)

rkcore: $(OBJ) rkcore.o
	$(FC) $(LDFLAGS) $(.ALLSRC) -o $@

test: test_rkcore.o
	gcc -g -Wall -lrkcore $(.ALLSRC) -o $@

install: install-lib install-bin

install-lib: $(LIBTARG)
	cp -pf $(.ALLSRC) $(LIBDIR)

install-bin: $(BINTARG)
	cp -pf $(.ALLSRC) $(BINDIR)

clean:
	rm -f *.o *.mod $(BINTARG) $(LIBTARG) test
