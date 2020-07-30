
CC = g++
CFLAGS = --std=c++11
DEBUG = n
OUTDIR = bin/release

ifeq ($(DEBUG), y)
	CFLAGS += -g
	OUTDIR = bin/debug
endif

mk_dir:
	mkdir -p $(OUTDIR)

# Examples:
example%: $(OBJS) mk_dir
	$(CC) $(CFLAGS) examples/$@.cpp -o $(OUTDIR)/$@


# Source files:
# $(OUTDIR)/%.o: source/%.cpp source/*.h mk_dir
	# $(CC) $(CFLAGS) -c $< -o $@

# $(OUTDIR)/%.o: examples/%.cpp source/*.h mk_dir
	# $(CC) $(CFLAGS) -c $< -o $@

# Tests:
tests: test/nodes test/composites

test/%: test/%.cpp
	$(CC) $(CFLAGS) $< -o $@

# Helpers:
clean:
	rm -rf bin
