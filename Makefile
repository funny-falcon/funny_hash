CC=gcc
CFLAGS=-g -g3 -ggdb -ggdb3 -std=gnu99
COPT=-O2
OTHERS=others/MurmurHash3.h others/csiphash.h others/lookup3.h others/spooky-c.h

clean: clean_bench_exe
	@-rm -f bench/test_file
ifeq ($(shell uname -p),x86_64)
all: bench/bench_32 bench/bench_64
bench/bench_32: bench/bench_src.c funny_hash.h $(OTHERS)
	$(CC) $(CFLAGS) $(COPT) -I. -m32 bench/bench_src.c -o bench/bench_32

bench/bench_64: bench/bench_src.c funny_hash.h $(OTHERS)
	$(CC) $(CFLAGS) $(COPT) -I. bench/bench_src.c -o bench/bench_64

clean_bench_exe:
	@-rm -f bench/bench_32 bench/bench_64
bench: bench/bench_32 bench/bench_64 bench/test_file
	@echo "{\"res\":["
	@cd bench; sh run_bench.sh ./bench_32
	@cd bench; sh run_bench.sh ./bench_64
	@echo "{}]}"
else
all: bench/bench_32
bench/bench_32: bench/bench_src.c funny_hash.h $(OTHERS)
	$(CC) $(CFLAGS) $(COPT) -I. bench/bench_src.c -o bench/bench_64
clean_bench_exe:
	@-rm -f bench/bench_32
bench: bench/bench_32 bench/test_file
	@echo "{\"res\":["
	@cd bench; sh run_bench.sh ./bench_32
	echo "{}]}"
endif

bench/test_file:
	@$(CC) -O3 bench/test_file_s.c -o bench/test_file_s
	@bench/test_file_s 300000000 > bench/test_file

