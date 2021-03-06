CC=gcc
CFLAGS=-g -g3 -ggdb -ggdb3 -std=gnu99
COPT=-O2
OTHERS=others/MurmurHash3.h others/csiphash.h others/csaphash.h others/cbobhash.h others/lookup3.h others/spooky-c.h others/fnv.h

ifeq ($(shell uname -p),x86_64)
all: bench/bench_32 bench/bench_64
bench/bench_32: bench/bench_src.c funny_hash.h $(OTHERS)
	$(CC) $(CFLAGS) $(COPT) -I. -m32 bench/bench_src.c -o bench/bench_32

bench/bench_64: bench/bench_src.c funny_hash.h $(OTHERS)
	$(CC) $(CFLAGS) $(COPT) -I. bench/bench_src.c -o bench/bench_64

clean:
	@-rm -f bench/bench_32 bench/bench_64
bench: bench/bench_32 bench/bench_64
	@echo "{\"res\":["
	@cd bench; sh run_bench.sh ./bench_32
	@cd bench; sh run_bench.sh ./bench_64
	@echo "{}]}"
else
all: bench/bench_32
bench/bench_32: bench/bench_src.c funny_hash.h $(OTHERS)
	$(CC) $(CFLAGS) $(COPT) -I. bench/bench_src.c -o bench/bench_64
clean:
	@-rm -f bench/bench_32
bench: bench/bench_32
	@echo "{\"res\":["
	@cd bench; sh run_bench.sh ./bench_32
	echo "{}]}"
endif

