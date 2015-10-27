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
#include "others/csiphash.h"
#include "others/csaphash.h"
#include "others/lookup3.h"
#include "others/spooky-c.h"
#include "others/fnv.h"

void fill_rand(void* buf, size_t size) {
	uint64_t var = 1, v;

	while (size >= 8) {
		var = var * FH_BC1 + 1;
		v = var ^ (var >> 32);
		memcpy(buf, &v, 8);
		size -= 8;
		buf += 8;
	}
}

struct by_piece {
	size_t off;
	uint32_t gen;
	uint32_t len;
	uint32_t cnt;
};
static inline int
step_small(struct by_piece *p, size_t limit) {
	p->off += p->cnt * p->len;
	p->cnt ^= 1;
	p->gen = p->gen * 5 + 1;
	p->len = p->gen % 14 + 1;
	if (p->len > limit - p->off) p->len = limit - p->off;
	return p->off < limit;
}

static inline int
step_big(struct by_piece *p, size_t limit) {
	p->off = 0;
	p->len = limit;
	p->cnt++;
	return p->cnt <= 10;
}

static void __attribute__((noinline))
bench_fh32(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint32_t res = 0;
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		res = fh32_string_hash(src+bp.off, bp.len, res);
	}
	printf("\"hash\": \"%08x\",\t\t", res);
}
static void __attribute__((noinline))
bench_fh64(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint64_t res = 0;
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		res = fh64_string_hash(src+bp.off, bp.len, res);
	}
	printf("\"hash\": \"%08x%08x\",\t", (uint32_t)(res>>32), (uint32_t)res);
}

static void __attribute__((noinline))
bench_murmur32(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint32_t res = 0;
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		res = MurmurHash3_x86_32(src+bp.off, bp.len, res);
	}
	printf("\"hash\": \"%08x\",\t\t", res);
}
static void __attribute__((noinline))
bench_murmur128(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint64_t res[2] = {0, 0};
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		MurmurHash3_x64_128(src+bp.off, bp.len, res[0]^res[1], res);
	}
	printf("\"hash\": \"%08x%08x\",\t", (uint32_t)(res[0]>>32), (uint32_t)res[0]);
}
static void __attribute__((noinline))
bench_sip24(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	union {
		char key[16];
		uint64_t kkey[2];
	} r = {0};
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		r.kkey[0] = siphash24(src+bp.off, bp.len, r.key);
	}
	printf("\"hash\": \"%08x%08x\",\t", (uint32_t)(r.kkey[0]>>32), (uint32_t)r.kkey[0]);
}
static void __attribute__((noinline))
bench_sip13(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	union {
		char key[16];
		uint64_t kkey[2];
	} r = {0};
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		r.kkey[0] = siphash13(src+bp.off, bp.len, r.key);
	}
	printf("\"hash\": \"%08x%08x\",\t", (uint32_t)(r.kkey[0]>>32), (uint32_t)r.kkey[0]);
}
static void __attribute__((noinline))
bench_sap24(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	union {
		char key[8];
		uint32_t kkey[2];
	} r = {0};
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		r.kkey[0] = saphash24(src+bp.off, bp.len, r.key);
	}
	printf("\"hash\": \"%08x\",\t", r.kkey[0]);
}
static void __attribute__((noinline))
bench_sap13(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	union {
		char key[8];
		uint32_t kkey[2];
	} r = {0};
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		r.kkey[0] = saphash13(src+bp.off, bp.len, r.key);
	}
	printf("\"hash\": \"%08x\",\t", r.kkey[0]);
}
static void __attribute__((noinline))
bench_lookup3(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint32_t res = 0;
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		res = hashlittle(src+bp.off, bp.len, res);
	}
	printf("\"hash\": \"%08x\",\t\t", res);
}
static void __attribute__((noinline))
bench_spooky(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint64_t res = 0;
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		res = spooky_shorthash(src+bp.off, bp.len, res);
	}
	printf("\"hash\": \"%08x%08x\",\t", (uint32_t)(res>>32), (uint32_t)res);
}
static void __attribute__((noinline))
bench_fnv1a(int chunk, void *src, size_t size) {
	struct by_piece bp = {0, 0, 0};
	uint32_t res = 0;
	while (chunk ? step_big(&bp, size) : step_small(&bp, size)) {
		res = fnv1a(src+bp.off, bp.len, res);
	}
	printf("\"hash\": \"%08x\",\t", res);
}
static const char *kinds[] = {"funny32", "funny64", "murmur32", "murmur128", "sip24", "sip13", "sap24", "sap13", "lookup3", "spooky", "fnv1a"};
static const char *chunks[] = {"piece", "whole"};
#define arcnt(a) (sizeof(a)/ sizeof(a[0]))
int main(int argc, char **argv)
{
	int i, kind, chunk;
	size_t size = 0;
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
	size = atoll(argv[3]);
	if (size == 0) goto help;

	void *src = calloc(1, size);
	fill_rand(src, size);

	if (gettimeofday(&tstart, NULL) == -1) {
		printf("gettimeofday(): %s\n", strerror(errno));
		exit(1);
	}
	printf("{\"exe\":\"%s\", \"fun\":\"%s\", \"chunk\":\"%s\",\t", argv[0], argv[1], argv[2]);
	if (strcmp(argv[1], "funny32") == 0) {
		bench_fh32(chunk, src, size);
	} else if (strcmp(argv[1], "funny64") == 0) {
		bench_fh64(chunk, src, size);
	} else if (strcmp(argv[1], "murmur32") == 0) {
		bench_murmur32(chunk, src, size);
	} else if (strcmp(argv[1], "murmur128") == 0) {
		bench_murmur128(chunk, src, size);
	} else if (strcmp(argv[1], "sip24") == 0) {
		bench_sip24(chunk, src, size);
	} else if (strcmp(argv[1], "sip13") == 0) {
		bench_sip13(chunk, src, size);
	} else if (strcmp(argv[1], "sap24") == 0) {
		bench_sap24(chunk, src, size);
	} else if (strcmp(argv[1], "sap13") == 0) {
		bench_sap13(chunk, src, size);
	} else if (strcmp(argv[1], "lookup3") == 0) {
		bench_lookup3(chunk, src, size);
	} else if (strcmp(argv[1], "spooky") == 0) {
		bench_spooky(chunk, src, size);
	} else if (strcmp(argv[1], "fnv1a") == 0) {
		bench_fnv1a(chunk, src, size);
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
	printf("\"time\": %d.%02d},\n", (int)tstop.tv_sec, (int)tstop.tv_usec/10000);
	return 0;
help:
	printf("%s (funny32|funny64|murmur32|murmur128|sip24|sip13|sap24|sap13|lookup3|spooky) (piece|whole) worksetsize\n", argv[0]);
	printf("\tfunny32|funny64|murmur32|murmur128|sip24|sip13|sap24|sap13|lookup3|spooky - function to test\n");
	printf("\tpiece|whole - whole file at once or by 1-20 byte pieces\n");
	printf("\t\t(since i don't use incremental implementations, hashsum by pieces will differ\n)");
	printf("\tworksetsize - size of memory blob to hash\n");
	exit(1);
}
