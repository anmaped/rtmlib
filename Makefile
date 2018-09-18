CC = g++
RANLIB = ranlib

LIBSRC = Monitor.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

CFLAGS = -Wall -g -O0 -std=c++0x -D__x86__ --verbose
LOADLIBS = -L./

RTMLLIB = librtml.a
TARGETS = $(RTMLLIB)

all: $(TARGETS)

rtml.o: RTML_monitor.cpp
	$(CC) $(CFLAGS) $(LOADLIBS) -c RTML_monitor.cpp -o rtml.o

$(TARGETS): rtml.o
	ar rcs $(RTMLLIB) rtml.o
	ranlib $(RTMLLIB)

clean:
	rm rtml.o $(TARGETS) $(RTMLLIB) $(LIBOBJ)
