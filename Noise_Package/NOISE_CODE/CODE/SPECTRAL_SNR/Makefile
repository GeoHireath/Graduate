#
INST_DIR = $(HOME)/bin
BIN  = find_rms_per_v2

FC = gfortran

$(BIN) : $(BIN).f
	$(FC) -o $(BIN) $(BIN).f

install :: $(BIN)
	install -s $(BIN) $(INST_DIR)

clean ::
	rm -f $(BIN)
