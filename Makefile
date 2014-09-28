CC=gcc
CFLAGS=-g -g3 -ggdb -ggdb3 -std=gnu99
COPT=-O2

all: bench/bench_nat
clean:
	rm bench_nat
bench: test_file
bench: benchnat
benchnat: bench/bench_nat
	sh bench/run_bench.sh bench/bench_nat
ifeq ($(shell uname -p),x86_64)
all: bench/bench_32
clean: clean32
clean32:
	rm bench_32
bench: bench32
bench32: bench/bench_32
	sh bench/run_bench.sh bench/bench_32
endif

test_file:
	cat /dev/urandom | head -c 300000000 > bench/test_file

bench/bench_32: bench/bench_src.c funny_hash.h others/MurmurHash3.h others/csiphash.h others/lookup3.h
	$(CC) $(CFLAGS) $(COPT) -I. -m32 bench/bench_src.c -o bench/bench_32

bench/bench_nat: bench/bench_src.c funny_hash.h others/MurmurHash3.h others/csiphash.h others/lookup3.h
	$(CC) $(CFLAGS) $(COPT) -I. bench/bench_src.c -o bench/bench_nat
