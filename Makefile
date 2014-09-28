CC=gcc
CFLAGS=-g -g3 -ggdb -ggdb3 -std=gnu99
COPT=-O2 -fPIE

all: bench_nat
clean:
	rm bench_nat
bench: bench_nat test_file
	sh run_bench.sh ./bench_nat
cpu=$(shell uname -p)
ifeq ($(cpu),x86_64)
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

bench_32: bench.c funny_hash.h others/MurmurHash3.h others/csiphash.h
	$(CC) $(CFLAGS) $(COPT) -m32 bench.c -o bench_32

bench_nat: bench.c funny_hash.h others/MurmurHash3.h others/csiphash.h
	$(CC) $(CFLAGS) $(COPT) bench.c -o bench_nat
