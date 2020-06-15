
CC = g++
CFLAGS = --std=c++11
DEBUG = n
OUTDIR = bin/release

ifeq ($(DEBUG), y)
	CFLAGS += -g
	OUTDIR = bin/debug
endif

OBJS = $(OUTDIR)/bt.o $(OUTDIR)/memory.o $(OUTDIR)/nodes.o $(OUTDIR)/composites.o $(OUTDIR)/decorators.o $(OUTDIR)/visitors.o

mk_dir:
	mkdir -p $(OUTDIR)

# Examples:
example1: $(OUTDIR)/example1.o $(OBJS) mk_dir
	$(CC) $(CFLAGS) -o $(OUTDIR)/example1 $(OUTDIR)/example1.o $(OBJS)

# Source files:
$(OUTDIR)/%.o: source/%.cpp source/*.h mk_dir
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTDIR)/%.o: examples/%.cpp source/*.h mk_dir
	$(CC) $(CFLAGS) -c $< -o $@

# Helpers:
clean:
	rm -rf bin
