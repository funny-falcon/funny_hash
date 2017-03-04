#include <stdint.h>

#define ROTL32(x,k) (((x) << (k)) | ((x) >> (32 - k)))

/*
   static uint32_t U8TO32_LE(const uint8_t *p) {
   return p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
   }
   */
#define U8TO32_LE(p) (*(uint32_t*)p)

typedef struct Marvin32State_t {
	uint32_t lo, hi;
} Marvin32State;

static inline void
Marvin32_Mix(Marvin32State *st, uint32_t v) {
	st->lo += v;
	st->hi ^= st->lo;
	st->lo = ROTL32(st->lo, 20) + st->hi;
	st->hi = ROTL32(st->hi,  9) ^ st->lo;
	st->lo = ROTL32(st->lo, 27) + st->hi;
	st->hi = ROTL32(st->hi, 19);
}

static inline uint32_t
Marvin32_Hash(const unsigned char *ptr, size_t len, uint64_t seed) {
	Marvin32State st;
	uint32_t final;

	st.lo = (uint32_t)(seed      );
	st.hi = (uint32_t)(seed >> 32);

	while (len >= 4) {
		Marvin32_Mix(&st, U8TO32_LE(ptr));
		ptr += 4;
		len -= 4;
	}

	/* pad the final 0-3 bytes with 0x80 */
	final = 0x80;
	switch (len) {
		case 3: final = (final << 8) | ptr[2];
		case 2: final = (final << 8) | ptr[1];
		case 1: final = (final << 8) | ptr[0];
		case 0:
		default:;
	}
	Marvin32_Mix(&st, final);
	Marvin32_Mix(&st, 0);
	Marvin32_Mix(&st, 0);
	//Marvin32_Mix(&st, 0);

	return st.lo ^ st.hi;
}
