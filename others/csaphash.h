/* <MIT License>
 Copyright (c) 2013  Marek Majkowski <marek@popcount.org>
 Copyright (c) 2015  Sokolov Yuriy aka funny_falcon <funny.falcon@gmail.com>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 </MIT License>

 Original location:
    https://github.com/majek/csiphash/

 Solution inspired by code from:
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
	HALF_ROUND(v0,v1,v2,v3,7,8);		\
	HALF_ROUND(v2,v1,v0,v3,9,13);

#define DOUBLE_ROUND(v0,v1,v2,v3)		\
	HALF_ROUND(v0,v1,v2,v3,7,8);		\
	HALF_ROUND(v2,v1,v0,v3,9,13);           \
	HALF_ROUND(v0,v1,v2,v3,7,8);		\
	HALF_ROUND(v2,v1,v0,v3,9,13);


static inline uint32_t saphash24(const void *src, unsigned long src_sz, const char key[16]) {
	const uint32_t *_key = (uint32_t *)key;
	uint32_t k0 = _le32toh(_key[0]);
	uint32_t k1 = _le32toh(_key[1]);
	uint32_t b = (uint32_t)src_sz << 24;
	const uint32_t *in = (uint32_t*)src;

	uint32_t v0 = k0 ^ 0x736f6d65UL;
	uint32_t v1 = k1 ^ 0x326f7261UL;
	uint32_t v2 = k0 ^ 0x6c796765UL;
	uint32_t v3 = k1 ^ 0x74653262UL;

	while (src_sz >= 4) {
		uint32_t mi = _le32toh(*in);
		in += 1; src_sz -= 4;
		v3 ^= mi;
		DOUBLE_ROUND(v0,v1,v2,v3);
		v0 ^= mi;
	}

	uint32_t t = 0; uint8_t *pt = (uint8_t *)&t; uint8_t *m = (uint8_t *)in;
	switch (src_sz) {
	case 3: pt[2] = m[2];
	case 2: pt[1] = m[1];
	case 1: pt[0] = m[0];
	}
	b |= _le32toh(t);

	v3 ^= b;
	DOUBLE_ROUND(v0,v1,v2,v3);
	v0 ^= b; v2 ^= 0xff;
	DOUBLE_ROUND(v0,v1,v2,v3);
	DOUBLE_ROUND(v0,v1,v2,v3);
	return (v0 ^ v1) ^ (v2 ^ v3);
}

static inline uint32_t saphash13(const void *src, unsigned long src_sz, const char key[16]) {
	const uint32_t *_key = (uint32_t *)key;
	uint32_t k0 = _le32toh(_key[0]);
	uint32_t k1 = _le32toh(_key[1]);
	uint32_t b = (uint32_t)src_sz << 24;
	const uint32_t *in = (uint32_t*)src;

	uint32_t v0 = k0 ^ 0x736f6d65UL;
	uint32_t v1 = k1 ^ 0x326f7261UL;
	uint32_t v2 = k0 ^ 0x6c796765UL;
	uint32_t v3 = k1 ^ 0x74653262UL;

	while (src_sz >= 4) {
		uint32_t mi = _le32toh(*in);
		in += 1; src_sz -= 4;
		v3 ^= mi;
		ROUND(v0,v1,v2,v3);
		v0 ^= mi;
	}

	uint32_t t = 0; uint8_t *pt = (uint8_t *)&t; uint8_t *m = (uint8_t *)in;
	switch (src_sz) {
	case 3: pt[2] = m[2];
	case 2: pt[1] = m[1];
	case 1: pt[0] = m[0];
	}
	b |= _le32toh(t);

	v3 ^= b;
	ROUND(v0,v1,v2,v3);
	v0 ^= b; v2 ^= 0xff;
	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	ROUND(v0,v1,v2,v3);
	return (v0 ^ v1) ^ (v2 ^ v3);
}

#undef ROTATE
#undef ROUND
#undef HALF_ROUND
#undef DOUBLE_ROUND
