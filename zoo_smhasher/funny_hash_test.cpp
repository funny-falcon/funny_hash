#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "funny_hash_test.h"
#include "funny_hash.h"
#include "marvin.h"


#define swap16(x) (((x) << 16) | ((x) >> 16))
#define C1 0xb8b34b2d
#define C2 0x52c6a2d9
static inline uint32_t memhash(const void *s, int len, int seed)
{
	uint32_t a = seed, b = seed ^ 0xdeadbeef;
	uint32_t v = 0;
	uint8_t const *ch = (uint8_t const*)s;
	int i = len / 4;
	for (;i; i--, ch+=4) {
		v = ch[3] |
			((uint32_t)ch[2] << 8) |
			((uint32_t)ch[1] << 16) |
			((uint32_t)ch[0] << 24);
		a ^= v; a = swap16(a); a *= C1;
		b = swap16(b); b ^= v; b *= C2;
	}
	v = 0;
	for (i = len & 3; i; i--, ch++) v = (v << 8) | ch[0];
	a = swap16(a) ^ v;
	b = swap16(b) ^ (v + len);
	a *= C1; a ^= a >> 16; a *= C1;
	b *= C2; b ^= b >> 16; b *= C2;
	return a ^ b ^ (a >> 16) ^ (b >> 17);
}

void FunnyHash_x86_32 ( const void * key, int len, uint32_t seed, void * out )
{
	*(uint32_t*)out = fh32_string_hash(key, len, seed);
	//*(uint32_t*)out = memhash(key, len, seed);
}
void FunnyHash_x86_64 ( const void * key, int len, uint32_t seed, void * out )
{
	*(uint64_t*)out = fh64_string_hash(key, len, seed);
}
void FunnyHash_x86_64a( const void * key, int len, uint32_t seed, void * out )
{
	uint64_t r = fh64_string_hash(key, len, seed);
	*(uint32_t*)out = (uint32_t)r;
}
void FunnyHash_x86_64b( const void * key, int len, uint32_t seed, void * out )
{
	uint64_t r = fh64_string_hash(key, len, seed);
	*(uint32_t*)out = (uint32_t)(r>>32);
}

void Marvin32 ( const void * key, int len, uint32_t seed, void * out )
{
	uint64_t sd = ((uint64_t)(seed ^ FH_C1) << 32) | (seed ^ 0xdeadbeef);
	*(uint32_t*)out = Marvin32_Hash((const unsigned char*)key, len, sd);
}
