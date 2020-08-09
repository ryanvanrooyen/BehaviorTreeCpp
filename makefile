
CC = g++
CFLAGS = --std=c++11
DEBUG = n
OUTDIR = bin/release

ifeq ($(DEBUG), y)
	CFLAGS += -g
	OUTDIR = bin/debug
endif

all: tests
	# Print newline for space
	echo ""
	# Run tests
	tests/tests

behavior_tree.hpp: $(wildcard source/*)
	./build.py behavior_tree.hpp include/all.hpp

# Examples:
example%: $(OBJS) mk_dir
	$(CC) $(CFLAGS) examples/$@.cpp -o $(OUTDIR)/$@

# Source files:
# mk_dir:
#	mkdir -p $(OUTDIR)

# $(OUTDIR)/%.o: source/%.cpp source/*.h mk_dir
	# $(CC) $(CFLAGS) -c $< -o $@

# $(OUTDIR)/%.o: examples/%.cpp source/*.h mk_dir
	# $(CC) $(CFLAGS) -c $< -o $@

tests: behavior_tree.hpp $(wildcard tests/*.*pp)
	$(CC) $(CFLAGS) tests/tests.cpp -o tests/tests

clean:
	rm -rf bin
	rm behavior_tree.hpp
