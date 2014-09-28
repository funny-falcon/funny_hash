#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "funny_hash.h"
#include "others/MurmurHash3.h"

struct by_piece {
	size_t off;
	uint32_t gen;
	uint32_t len;
	uint32_t cnt;
};
static inline int step(struct by_piece *p, size_t max, size_t limit) {
	if (p->cnt == 5) {
		p->off += p->len;
		p->cnt = 0;
	}
	else
		p->cnt++;
	p->gen = p->gen * 5 + 1;
	p->len = p->gen % max + 1;
	if (p->off + p->len > limit) p->len = limit - p->off;
	return p->off < limit;
}

static const char *kinds[] = {"funny32", "funny64", "murmur32", "murmur128"};
static const char *chunks[] = {"piece", "whole"};
#define arcnt(a) (sizeof(a)/ sizeof(a[0]))
int main(int argc, char **argv)
{
	int i, kind, chunk;
	size_t off = 0;
	struct by_piece bp = {0, 0, 0};
	struct timeval tstart, tstop;
	if (argc < 4) goto help;
	for(kind = 0; kind < arcnt(kinds); kind++) {
		if (strcmp(argv[1], kinds[kind]) == 0)
			break;
	}
	if (kind == arcnt(kinds)) goto help;
	for(chunk = 0; chunk < arcnt(chunks); chunk++) {
		if (strcmp(argv[2], chunks[chunk]) == 0)
			break;
	}
	if (chunk == arcnt(chunks)) goto help;

	int fd = open(argv[3], O_RDONLY);
	if (fd == -1) {
		printf("open(%s): %s\n", argv[2], strerror(errno));
		exit(1);
	}
	struct stat stat;
	if (fstat(fd, &stat) == -1) {
		printf("fstat(%s): %s\n", argv[2], strerror(errno));
		exit(1);
	}
	void *src = mmap(NULL, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (src == MAP_FAILED) {
		printf("mmap(%s): %s\n", argv[2], strerror(errno));
		exit(1);
	}
	if (madvise(src, stat.st_size, MADV_SEQUENTIAL) == -1) {
		printf("madvise(%s): %s\n", argv[2], strerror(errno));
		exit(1);
	}
	/* loop to load file in a cache */
	i = 0;
	for(off = 0; off < stat.st_size; off += 4096) {
		i += ((char*)src)[off];
	}
	if (gettimeofday(&tstart, NULL) == -1) {
		printf("gettimeofday(): %s\n", strerror(errno));
		exit(1);
	}
	if (kind == 0) { /* funny32 */
		uint32_t res = 0;
		if (chunk == 0) { /* by piece */
			while (step(&bp, 20, stat.st_size)) {
				res = fh32_string_hash(src+bp.off, bp.len, res);
			}
		} else {
			for(i = 0; i < 10; i++)
				res = fh32_string_hash(src, stat.st_size, res);
		}
		printf("hash: %08x\n", res);
	} else if (kind == 1) { /* funny64 */
		uint64_t res = 0;
		if (chunk == 0) { /* by piece */
			while (step(&bp, 20, stat.st_size)) {
				res = fh64_string_hash(src+bp.off, bp.len, res);
			}
		} else {
			for(i = 0; i < 10; i++)
				res = fh64_string_hash(src, stat.st_size, res);
		}
		printf("hash: %08x%08x\n", (uint32_t)(res>>32), (uint32_t)res);
	} else if (kind == 2) { /* murmur32 */
		uint32_t res = 0;
		if (chunk == 0) { /* by piece */
			while (step(&bp, 20, stat.st_size)) {
				MurmurHash3_x86_32(src+bp.off, bp.len, res, &res);
			}
		} else {
			for(i = 0; i < 10; i++)
				MurmurHash3_x86_32(src, stat.st_size, res, &res);
		}
		printf("hash: %08x\n", res);
	} else if (kind == 3) { /* murmur128 as 64 */
		uint64_t res[2] = {0, 0};
		if (chunk == 0) { /* by piece */
			while (step(&bp, 20, stat.st_size)) {
				MurmurHash3_x64_128(src+bp.off, bp.len, res[0]^res[1], res);
			}
		} else {
			for(i = 0; i < 10; i++)
				MurmurHash3_x64_128(src, stat.st_size, res[0]^res[1], res);
		}
		printf("hash: %08x%08x\n", (uint32_t)(res[0]>>32), (uint32_t)res[0]);
	}
	if (gettimeofday(&tstop, NULL) == -1) {
		printf("gettimeofday(): %s\n", strerror(errno));
		exit(1);
	}
	if (tstop.tv_usec < tstart.tv_usec) {
		tstop.tv_sec -= 1;
		tstop.tv_usec += 1000000;
	}
	tstop.tv_usec -= tstart.tv_usec;
	tstop.tv_sec -= tstart.tv_sec;
	printf("time: %d.%06d\n", (int)tstop.tv_sec, (int)tstop.tv_usec);
	return 0;
help:
	printf("%s (funny32|funny64) (piece|whole) filename\n", argv[0]);
	printf("\tfunny32|funny64 - function to test\n");
	printf("\tpiece|whole - whole file at once or by 1-20 byte pieces\n");
	printf("\t\t(since i don't use incremental implementations, hashsum by pieces will differ\n)");
	printf("\tfilename - filename to hash\n");
	exit(1);
}
