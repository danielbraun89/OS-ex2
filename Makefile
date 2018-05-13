CC=g++
CXX=g++
RANLIB=ranlib

LIBSRC=uthreads.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I.
CFLAGS = -Wall -std=c++11 -g $(INCS)
CXXFLAGS = -Wall -std=c++11 -g $(INCS)

UTHREADLIB = libuthreads.a
TARGETS = $(UTHREADLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS)

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(TARGETS) $(UTHREADLIB) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
# DO NOT DELETE

uthreads.o: Thread.h uthreads.h /usr/include/stdio.h /usr/include/features.h
uthreads.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
uthreads.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
uthreads.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
uthreads.o: /usr/include/libio.h /usr/include/_G_config.h
uthreads.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
uthreads.o: /usr/include/bits/sys_errlist.h /usr/include/signal.h
uthreads.o: /usr/include/bits/sigset.h /usr/include/bits/signum.h
uthreads.o: /usr/include/time.h /usr/include/bits/siginfo.h
uthreads.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
uthreads.o: /usr/include/bits/sigstack.h /usr/include/sys/ucontext.h
uthreads.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h
uthreads.o: /usr/include/sys/time.h /usr/include/bits/time.h
uthreads.o: /usr/include/sys/select.h /usr/include/bits/select.h
uthreads.o: /usr/include/setjmp.h /usr/include/bits/setjmp.h
