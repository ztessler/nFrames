include ../common.mk
INC     = ../include
SRC     = ../modules
OBJ     = ../obj
PLUGINS = ../plugins
CC      = $(UNIXCC) $(UNIXCCOPS) -I$(INC) -I$(GHAASDIR)//include
LINK    = $(UNIXCC) $(UNIXPLGLNK)

INCLUDES = $(INC)/NF.h

TARGETS = $(PLUGINS)/wbm.mplugin

OBJS = $(OBJ)/PETlib.o

all: $(TARGETS)

clean:
	rm $(TARGETS) $(OBJS)

$(PLUGINS)/wbm.mplugin: $(OBJS) $(SRC)/Makefile
	$(LINK) $(OBJS) -o $(PLUGINS)/wbm.mplugin

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) -c $(SRC)/$*.c -o $(OBJ)/$*.o
