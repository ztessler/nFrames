include ./common.mk
OBJ     = ./obj
BIN     = ./bin
PLUGINS = ./plugins

all: $(OBJ) $(LIB) $(PLUGINS) $(BIN) src_target modules_target inputs_target

clean: clean_src clean_modules clean_inputs

clean_src:
	$(UNIXMAKE) -C src     clean

clean_modules:
	$(UNIXMAKE) -C modules clean

clean_inputs:
	$(UNIXMAKE) -C inputs  clean

src_target:
	$(UNIXMAKE) -C src     all

modules_target:
	$(UNIXMAKE) -C modules all

inputs_target:
	$(UNIXMAKE) -C inputs  all

$(OBJ):
	mkdir -p $(OBJ)

$(PLUGINS):
	mkdir -p $(PLUGINS)

$(BIN):
	mkdir -p $(BIN)
