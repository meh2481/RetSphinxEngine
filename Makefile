# Entry point for Pony48 makefiles
# Based loosely off FreeImage makefiles - thanks, doods
# Default to 'make -f Makefile.unix' for Linux and for unknown OS. 

OS = $(shell uname -s)
MAKEFILE = Makefile.unix
PROCESSOR = $(shell uname -m)
ifneq ($(PROCESSOR), x86_64)
    MAKEFILE = Makefile_32.unix
endif
ifeq ($(OS), Darwin)
    MAKEFILE = Makefile.osx
endif
ifneq (,$(findstring MINGW,$(OS)))
    MAKEFILE = Makefile.mingw
endif

default:
	$(MAKE) -f $(MAKEFILE) 

all:
	$(MAKE) -f $(MAKEFILE) all 

clean:
	$(MAKE) -f $(MAKEFILE) clean 

release:
	make "BUILD=release"

