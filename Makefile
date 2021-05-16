CXX      ?= c++
CXXFLAGS ?= -std=c++1z -Wall -Wextra -O3 -g3

all: benchmark tests

benchmark: test/benchmark.cc utf8.hh test/utf8-encode.hh test/bh-utf8.h
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test/benchmark.cc $(LDLIBS)

tests: test/tests.cc utf8.hh test/utf8-encode.hh
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test/tests.cc $(LDLIBS)

bench: benchmark
	./benchmark

check: tests
	./tests

clean:
	rm -f benchmark tests
