#
INST_DIR = .

BIN1 = disp_curve_eq

BIN2 = disp_curve

all  : disp_curve disp_curve_eq

CC = gcc

DBG = -g

FLAG = -lm

OBJS1 = disp_curv_eq.o geocentric_geographic.o surfpath.o ttime.o
OBJS2 = disp_curv.o geocentric_geographic.o surfpath.o ttime.o

$(BIN1) : $(OBJS1)
	$(CC) $(DBG) $(OBJS1) $(FLAG) -o $(BIN1) 

$(BIN2) : $(OBJS2)
	$(CC) $(DBG) $(OBJS2) $(FLAG) -o $(BIN2)

install :: 
	install -s $(BIN1) $(BIN2) $(INST_DIR)


clean :: 
	rm -f $(BIN1) $(BIN2) core *.o $(INST_DIR)/$(BIN1) $(INST_DIR)/$(BIN2)
