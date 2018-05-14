CPP= g++
CPPFLAGS= -c -g -Wextra -Wvla -Wall -std=c++11 -DNDEBUG
TAR_FILES= Makefile README uthreads.cpp Thread.cpp Thread.h 
# All Target
all: uthreads

#Library
uthreads: uthreads.o Thread.o
	ar rcs libuthreads.a uthreads.o Thread.o


# Object Files
uthreads.o: uthreads.h uthreads.cpp Thread.h 
	$(CPP) $(CPPFLAGS) uthreads.cpp -o uthreads.o
	
Thread.o: Thread.h
	$(CPP) $(CPPFLAGS) Thread.h -o Thread.o

tar:
	tar cvf ex1.tar $(TAR_FILES)
 
# Other Targets
clean:
	-rm -f *.o libuthreads.a 
