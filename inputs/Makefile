include ../common.mk
INC     = ../include
SRC     = ../inputs
OBJ     = ../obj
PLUGINS = ../plugins
CC      = $(UNIXCC) $(UNIXCCOPS) -I$(INC) -I$(GHAASDIR)/include
LINK    = $(UNIXCC) $(UNIXPLGLNK)

INCLUDES = $(INC)/NF.h $(INC)/NFio.h $(SRC)/NCfunctions.h

TARGETS = $(PLUGINS)/NCgrid.iplugin

all: $(TARGETS)

clean:
	rm $(TARGETS) $(OBJ)/NC*.o

$(PLUGINS)/NC%.iplugin: $(SRC)/NC%.c $(OBJ)/NCfunctions.o $(SRC)/Makefile
	$(CC) -c $(SRC)/NC$*.c -o $(OBJ)/NC$*.o
	$(LINK) $(OBJ)/NC$*.o $(OBJ)/NCfunctions.o -o $(PLUGINS)/NC$*.iplugin

$(OBJ)/NCfunctions.o: $(SRC)/NCfunctions.c $(INCLUDES)
	$(CC) -c $(SRC)/NCfunctions.c -o $(OBJ)/NCfunctions.o
