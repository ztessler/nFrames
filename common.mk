UNIX=$(shell uname)

ifeq ($(UNIX),Linux)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/usr/local/include
export UNIXLIBS=-rdynamic -L/usr/local/lib -lexpat -ldl -ludunits2 -lm
export UNIXPLGLNK=-shared -nostartfiles -L/usr/local/lib -lnetcdf
export UNIXMAKE=make
endif
ifeq ($(UNIX),Darwin)
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -I/sw/include
export UNIXLIBS=-L/usr/local/lib -lexpat -ldl -ludunits2 -lm
export UNIXPLGLNK=-dynamiclib -flat_namespace -undefined suppress -L/sw/lib -lnetcdf
export UNIXMAKE=make
endif
ifeq ($(UNIX),SunOS)
export UNIXAR=ar -ru
export UNIXCC=gcc
export UNIXCCOPS=-g -Wall -fsigned-char -D_GNU_SOURCE -L../../CMlib/lib -I/usr/local/netcdf/include -I/usr/local/udunits/include
export UNIXLIBS=-rdynamic -lexpat -ldl -lm
export UNIXPLGLNK=-shared -nostartfiles
export UNIXMAKE=make
endif
