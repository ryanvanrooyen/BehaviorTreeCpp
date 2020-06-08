
CC = g++ --std=c++11
HEADERS = source/bt.h source/memory.h source/nodes.h source/composites.h source/decorators.h source/visitors.h
OBJS = source/bt.o source/memory.o source/nodes.o source/composites.o source/decorators.o source/visitors.o
OUTDIR = bin

# Examples:
example1: examples/example1.o $(OBJS)
	$(CC) -o $(OUTDIR)/example1 examples/example1.o $(OBJS)

# Source files:
%.o: %.cpp $(HEADERS)
	$(CC) -c $< -o $@

# Helpers:
clean:
	rm source/*.o examples/*.o bin/*
