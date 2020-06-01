
CC = g++ --std=c++11
HEADERS = source/nodes.h source/composites.h source/visitors.h
OBJS = source/nodes.o source/composites.o source/visitors.o
OUTDIR = bin

# Examples:
example1: examples/example1.o $(OBJS)
	$(CC) -o $(OUTDIR)/example1 examples/example1.o $(OBJS)

examples/example1.o: examples/example1.cpp $(HEADERS)
	$(CC) -c examples/example1.cpp -o examples/example1.o

# Source files:
# source/nodes.o: source/nodes.cpp $(HEADERS)
source/nodes.o: source/nodes.cpp $(HEADERS)
	$(CC) -c source/nodes.cpp -o source/nodes.o

source/composites.o: source/composites.cpp $(HEADERS)
	$(CC) -c source/composites.cpp -o source/composites.o

source/visitors.o: source/visitors.cpp $(HEADERS)
	$(CC) -c source/visitors.cpp -o source/visitors.o

# Helpers:
clean:
	rm source/*.o examples/*.o bin/*
