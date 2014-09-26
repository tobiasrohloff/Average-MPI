CC=mpic++
CFLAGS=-w -Ofast -std=c++11 -DDEBUG
SOURCES=mpiavg.cpp
EXECUTABLE=mpiavg

prog: $(SOURCES)
	$(CC) $(SOURCES) -o $(EXECUTABLE) $(CFLAGS)

clear:
	rm -r *.o