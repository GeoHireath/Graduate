#
INST_DIR = $(HOME)/bin
#BIN  = spectral_snr_eq
BIN  = spectral_snr_f_V2

fflags = -O -ffixed-line-length-none -lgfortran
cflags = -O 

#LDLIBS = -L/PARA/pp466/Software/FFTW/lib -lfftw3

FFLAGS = $(DBG) $(fflags)
CFLAGS = $(DBG) $(cflags)

FC = gfortran
CC = gcc

DBG =
#FOBJS = spectral_snr_eq.o filter4.o swapn.o
FOBJS = spectral_snr_f_V2.o filter4.o swapn.o

$(BIN) : $(FOBJS)
	$(CC) $(FFLAGS) $(CFLAGS) $(FOBJS) -o $(BIN) $(LDLIBS) -lm

install :: $(BIN)
	install -s $(BIN) $(INST_DIR)

clean ::
	rm -f $(BIN) core $(FOBJS)
