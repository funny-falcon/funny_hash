/*
   Chaskey reference C implementation (speed optimized)

   Written in 2014 by Nicky Mouha, based on SipHash

   To the extent possible under law, the author has dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

NOTE: This implementation assumes a little-endian architecture
that does not require aligned memory accesses.
*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define ROTL(x,b) (uint32_t)( ((x) >> (32 - (b))) | ( (x) << (b)) )

#define HALF_ROUND(a,b,c,d,s,t)			\
	a += b; c += d;				\
	b = ROTL(b, s) ^ a;			\
	d = ROTL(d, t) ^ c;			\
	a = ROTL(a, 16);

#define ROUND		\
	HALF_ROUND(v[0],v[1],v[2],v[3],5,8);		\
	HALF_ROUND(v[2],v[1],v[0],v[3],7,13);

#define PERMUTE \
	ROUND; \
ROUND;

static inline void chaskey(uint32_t *tag, const uint8_t *m, const uint32_t mlen, const uint32_t k[4]) {

	const uint32_t *M = (uint32_t*) m;
	const uint32_t *end = M + ((mlen/8)*2); /* pointer to last message block */

	const uint32_t *l;
	uint32_t v[4];

	int i;
	uint8_t *p;

	//v[0] = k[0] ^ 0x6c796765UL;
	v[0] = k[0] ^ 0xdeadbeef;
	v[1] = k[1];
	//v[2] = k[1] ^ 0x74653262UL;
	//v[2] = k[1] ^ 0xfee1bad0;
	v[2] = k[1] ^ 0xb19dea15;
	v[3] = k[0];

	if (mlen != 0) {
		for ( ; M != end; M += 2 ) {
			v[1] ^= M[0];
			v[3] ^= M[1];
			v[2] ^= 1;
			//ROUND;
			ROUND;
		}
	}

	i = mlen & 0x7;
	p = (uint8_t*)(M + i/4);
	switch (i) {
		case 7:  v[1] ^= (uint32_t)p[2] << 16;
		case 6:  v[1] ^= (uint32_t)p[1] << 8;
		case 5:  v[1] ^= p[0];
		case 4:
			 v[3] ^= M[0];
			 break;
		case 3:  v[3] ^= (uint32_t)p[2] << 16;
		case 2:  v[3] ^= (uint32_t)p[1] << 8;
		case 1:  v[3] ^= p[0];
		case 0:  break;
	}
	//v[1] ^= (mlen & 0xff) << 24;
	//v[0] ^= 1;
	//v[0] ^= mlen << 24;
	v[0] ^= mlen << 16;
	//v[0] ^= mlen;
	ROUND;
	/*
	ROUND;
	ROUND;
	ROUND;
	*tag = v[0] ^ v[1] ^ v[2] ^ v[3];
	*/

static const uint32_t C1 = 0xb8b34b2d;
static const uint32_t C2 = 0x52c6a2d9;
static const uint32_t C3 = 0xcc9e2d51;
//static const uint32_t C4 = 0x1b873593;
//static const uint32_t C4 = 0x7BDEC03B;
//static const uint32_t C4 = 0xC83A91E1;
static const uint32_t C4 = 0xc6a4a793;
	v[0] *= C2;
	v[1] *= C3;
	v[2] *= C4;
	v[3] *= C1;
	v[0] ^= v[0] >> 16;
	v[1] ^= v[1] >> 16;
	v[2] ^= v[2] >> 16;
	v[3] ^= v[3] >> 16;
	v[0] *= C3;
	v[1] *= C4;
	v[2] *= C1;
	v[3] *= C2;
	*tag = v[0] ^ ROTL(v[1], 8) ^ ROTL(v[2], 16) ^ ROTL(v[3], 24);
}

	void
chaskey_test(const void *input, int len, uint32_t seed, void *out)
{
	uint32_t k[2] = {seed, seed};
	chaskey(out, input, len, k);
}


