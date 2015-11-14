/*
 Copyright (c) 2015+ Sokolov Yuriy aka funny_falcon <funny.falcon@gmail.com>

 It is in public domain, free to use and modify by everyone for any usage.
 I will be glad, if you put copyright notice above in your code or distribution, but you don't ought to.
 I'm not responsible for any damage cause by this piece of code.

 Original location:

 Solution inspired by code from:
    Maker Majkowski https://github.com/majek/csiphash/
    Samuel Neves (supercop/crypto_auth/siphash24/little)
    djb (supercop/crypto_auth/siphash24/little2)
    Jean-Philippe Aumasson (https://131002.net/siphash/siphash24.c)
*/

#include <stdint.h>

#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
	__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define _le32toh(x) ((uint32_t)(x))
#elif defined(_WIN32)
/* Windows is always little endian, unless you're on xbox360
   http://msdn.microsoft.com/en-us/library/b0084kay(v=vs.80).aspx */
#  define _le32toh(x) ((uint32_t)(x))
#elif defined(__APPLE__)
#  include <libkern/OSByteOrder.h>
#  define _le32toh(x) OSSwapLittleToHostInt32(x)
#else

/* See: http://sourceforge.net/p/predef/wiki/Endianness/ */
#  if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#    include <sys/endian.h>
#  else
#    include <endian.h>
#  endif
#  if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
	__BYTE_ORDER == __LITTLE_ENDIAN
#    define _le32toh(x) ((uint32_t)(x))
#  else
#    define _le32toh(x) le32toh(x)
#  endif

#endif


#define ROTATE(x, b) (uint32_t)( ((x) << (b)) | ( (x) >> (32 - (b))) )

#define HALF_ROUND(a,b,c,d,s,t)			\
	a += b; c += d;				\
	b = ROTATE(b, s) ^ a;			\
	d = ROTATE(d, t) ^ c;			\
	a = ROTATE(a, 16);

#define ROUND(v0,v1,v2,v3)		\
	HALF_ROUND(v2,v1,v0,v3,7,13);   \
	HALF_ROUND(v0,v1,v2,v3,5,8);

#define DOUBLE_ROUND(v0,v1,v2,v3)		\
	ROUND(v0, v1, v2, v3);                  \
	ROUND(v0, v1, v2, v3);


static inline uint32_t bobhash24(const void *src, unsigned long src_sz, const char key[8]) {
	const uint32_t *_key = (uint32_t *)key;
	uint32_t k0 = _le32toh(_key[0]);
	uint32_t k1 = _le32toh(_key[1]);
	uint32_t sz = (uint32_t)src_sz;
	const uint32_t *in = (uint32_t*)src;

	uint32_t v0 = k0;
	uint32_t v1 = k1 ^ 0x326f7261UL;
	uint32_t v2 = k0 ^ 0x6c796765UL;
	uint32_t v3 = k1;

	while (sz >= 8) {
		in += 2; sz -= 8;
		v1 ^= _le32toh(*in);
		v3 ^= _le32toh(*(in+1));
		ROUND(v0,v1,v2,v3);
		ROUND(v0,v1,v2,v3);
	}

	uint8_t *m = (uint8_t *)(in + sz/4);
	switch (sz) {
	case 7: v3 ^= (uint32_t)m[2] << 16;
	case 6: v3 ^= (uint32_t)m[1] << 8;
	case 5: v3 ^= m[0];
	case 4: v1 ^= _le32toh(*in);
		break;
	case 3: v1 ^= (uint32_t)m[2] << 16;
	case 2: v1 ^= (uint32_t)m[1] << 8;
	case 1: v1 ^= m[0];
		break;
	}
	v0 ^= src_sz << 24;

	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);

	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	return (v0 ^ v1) ^ (v2 ^ v3);
}

static inline uint32_t bobhash13(const void *src, unsigned long src_sz, const char key[8]) {
	const uint32_t *_key = (uint32_t *)key;
	uint32_t k0 = _le32toh(_key[0]);
	uint32_t k1 = _le32toh(_key[1]);
	uint32_t sz = (uint32_t)src_sz;
	const uint32_t *in = (uint32_t*)src;

	uint32_t v0 = k0;
	uint32_t v1 = k1 ^ 0x326f7261UL;
	uint32_t v2 = k0 ^ 0x6c796765UL;
	uint32_t v3 = k1;

	while (sz >= 8) {
		in += 2; sz -= 8;
		v1 ^= _le32toh(*in);
		v3 ^= _le32toh(*(in+1));
		ROUND(v0,v1,v2,v3);
	}

	uint8_t *m = (uint8_t *)(in + sz/4);
	switch (sz) {
	case 7: v3 ^= (uint32_t)m[2] << 16;
	case 6: v3 ^= (uint32_t)m[1] << 8;
	case 5: v3 ^= m[0];
	case 4: v1 ^= _le32toh(*in);
		break;
	case 3: v1 ^= (uint32_t)m[2] << 16;
	case 2: v1 ^= (uint32_t)m[1] << 8;
	case 1: v1 ^= m[0];
		break;
	}
	v0 ^= src_sz << 24;

	ROUND(v0,v1,v2,v3);

	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	return (v0 ^ v1) ^ (v2 ^ v3);
}

#undef _le32toh
#undef ROTATE
#undef ROUND
#undef HALF_ROUND
#undef DOUBLE_ROUND
