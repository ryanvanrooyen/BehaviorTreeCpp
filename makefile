
CC = g++ --std=c++11
HEADERS = source/nodes.h source/composites.h source/visitors.h
OBJS = source/nodes.o source/composites.o source/visitors.o
OUTDIR = bin

# Examples:
example1: examples/example1.o $(OBJS)
	$(CC) -o $(OUTDIR)/example1 examples/example1.o $(OBJS)

# Source files:
%.o: %.cpp $(HEADERS)
	$(CC) -o $@ -c $<

# Helpers:
clean:
	rm source/*.o examples/*.o bin/*
