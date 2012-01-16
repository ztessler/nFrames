UNIX=$(shell uname)
ifndef GHAASDIR
export GHAASDIR=/usr/local/share/ghaas
endif

ifeq ($(UNIX),Linux)
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-rdynamic
endif
export UNIXPLGLNK=-shared -nostartfiles -lnetcdf
endif

ifeq ($(UNIX),Darwin)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/sw/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-L/sw/lib
endif
export UNIXPLGLNK=-dynamiclib -flat_namespace -undefined suppress $(CUSTOM_LIB) -lnetcdf
endif

ifeq ($(UNIX),SunOS)
ifndef ($(CUSTOM_INC))
	CUSTOM_INC=-I/usr/local/netcdf/include -I/usr/local/udunits/include
endif
ifndef ($(CUSTOM_LIB))
	CUSTOM_LIB=-rdynamic
endif
export UNIXPLGLNK=-shared -nostartfiles
endif

export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE $(CUSTOM_INC)
export UNIXLIBS=$(CUSTOM_LIB) -lexpat -ldl -ludunits2 -lm
export UNIXMAKE=make
