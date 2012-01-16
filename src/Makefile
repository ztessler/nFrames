include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CC     = $(UNIXCC) $(UNIXCCOPS) -I$(INC) -I$(GHAASDIR)/include
LINK   = $(UNIXCC)
LIBS   = $(UNIXLIBS) -L$(GHAASDIR)/lib -lCM10
TARGET = $(BIN)/nFrames.bin

INCLUDES = $(INC)/NF.h $(INC)/NFio.h $(INC)/NFobjects.h $(INC)/NFtime.h

OBJS = $(OBJ)/NFmain.o\
       $(OBJ)/NFlist.o\
       $(OBJ)/NFio.o\
       $(OBJ)/NFcompAggregate.o\
       $(OBJ)/NFcompContainer.o\
       $(OBJ)/NFcompInput.o\
       $(OBJ)/NFcompModel.o\
       $(OBJ)/NFcompRegion.o\
       $(OBJ)/NFcontext.o\
       $(OBJ)/NFcoupler.o\
       $(OBJ)/NFdomain.o\
       $(OBJ)/NFexecute.o\
       $(OBJ)/NFmisc.o\
       $(OBJ)/NFmodDerivative.o\
       $(OBJ)/NFmodEquation.o\
       $(OBJ)/NFmodInterface.o\
       $(OBJ)/NFmodProcess.o\
       $(OBJ)/NFmodRoute.o\
       $(OBJ)/NFnumParameter.o\
       $(OBJ)/NFnumVariable.o\
       $(OBJ)/NFobjAlias.o\
       $(OBJ)/NFobjCategory.o\
       $(OBJ)/NFobjOutput.o\
       $(OBJ)/NFobjects.o\
       $(OBJ)/NFparse.o\
       $(OBJ)/NFplugin.o\
       $(OBJ)/NFreport.o\
       $(OBJ)/NFresolve.o\
       $(OBJ)/NFtime.o

all: ${TARGET}

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(LINK) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) -c $(SRC)/$*.c -o $(OBJ)/$*.o
