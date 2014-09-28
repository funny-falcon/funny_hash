CC=gcc
CFLAGS=-g -g3 -ggdb -ggdb3 -std=gnu99
COPT=-O2

all: test32 test
clean:
	rm test32
	rm test

test32: test.c funny_hash.h
	$(CC) $(CFLAGS) $(COPT) -m32 test.c -o test32

test: test.c funny_hash.h
	$(CC) $(CFLAGS) $(COPT) test.c -o test
