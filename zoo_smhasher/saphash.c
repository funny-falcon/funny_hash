#include "saphash.h"
#include "saphash_impl.h"


#define U8TO32_LE(p) (*(const uint32_t*)(p))

/*
static void INLINE
U32TO8_LE(unsigned char *p, const uint32_t v) {
	*(uint32_t *)p = v;
}
*/

uint32_t
saphash(const unsigned char key[8], const unsigned char *m, size_t len) {
	uint32_t v0, v1, v2, v3;
	uint32_t mi, k0, k1;
	uint32_t last7;
	size_t i, blocks;

	k0 = U8TO32_LE(key + 0);
	k1 = U8TO32_LE(key + 4);
	v0 = k0 ^ 0x736f6d65ul;
	v1 = k1 ^ 0x326f7261ul;
	v2 = k0 ^ 0x6c796765ul;
	v3 = k1 ^ 0x74653262ul;

	last7 = (uint32_t)(len & 0xff) << 24;

#define sapcompress() \
	v0 += v1; v2 += v3; \
	v1 = ROTL32(v1,5);	v3 = ROTL32(v3,8); \
	v1 ^= v0; v3 ^= v2; \
	v0 = ROTL32(v0,16); \
	v2 += v1; v0 += v3; \
	v1 = ROTL32(v1,11); v3 = ROTL32(v3,13); \
	v1 ^= v2; v3 ^= v0; \
	v2 = ROTL32(v2,16);

	for (i = 0, blocks = (len & ~3); i < blocks; i += 4) {
		mi = U8TO32_LE(m + i);
		v3 ^= mi;
		sapcompress()
		v0 ^= mi;
	}

	switch (len - blocks) {
		case 3: last7 |= (uint32_t)m[i + 2] << 16;
		case 2: last7 |= (uint32_t)m[i + 1] <<  8;
		case 1: last7 |= (uint32_t)m[i + 0]      ;
		case 0:
		default:;
	};
	v3 ^= last7;
	sapcompress()
	v0 ^= last7;
	v2 ^= 0xff;
	sapcompress()
	sapcompress()
	sapcompress()
	return v0 ^ v1 ^ v2 ^ v3;
}

