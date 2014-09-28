CC=gcc
CFLAGS=-g -g3 -ggdb -ggdb3 -std=gnu99
COPT=-O2

all: bench_nat
clean:
	rm bench_nat
bench: test_file
bench: benchnat
benchnat: bench_nat
	sh run_bench.sh ./bench_nat
ifeq ($(shell uname -p),x86_64)
all: bench_32
clean: clean32
clean32:
	rm bench_32
bench: bench32
bench32: bench_32
	sh run_bench.sh ./bench_32
endif

test_file:
	cat /dev/urandom | head -c 600000000 > test_file

bench_32: bench_src.c funny_hash.h others/MurmurHash3.h others/csiphash.h
	$(CC) $(CFLAGS) $(COPT) -m32 bench_src.c -o bench_32

bench_nat: bench_src.c funny_hash.h others/MurmurHash3.h others/csiphash.h
	$(CC) $(CFLAGS) $(COPT) bench_src.c -o bench_nat
