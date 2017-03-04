#ifndef FUNNY_HASH_H
#define FUNNY_HASH_H
/*
 * Copyright (C) 2014 Sokolov Yura aka funny_falcon
 *
 * It is in public domain, free to use and modify by everyone for any usage.
 * I will be glad, if you put copyright notice above in your code or distribution, but you don't ought to.
 * I'm not responsible for any damage cause by this piece of code.
 */

/* API */

/* mix 32bit integer into 64bit state */
static inline void fh32_permute(uint32_t v, uint32_t *a, uint32_t *b);
/* hash string, seed with and produce 64bit state */
static inline void fh32_permute_string(const uint8_t *v, size_t len, uint32_t *a, uint32_t *b);
/* state finalization to 32bit value */
static inline uint32_t fh32_finalize(uint32_t a, uint32_t b);
/* convenient function to hash buffer with 32bit seed */
static inline uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed);
/* convenient function to hash buffer with 64bit seed */
static inline uint32_t fh32_string_hash2(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

/* mix 64bit integer into 128bit state */
static inline void fh64_permute(uint64_t v, uint64_t *a, uint64_t *b);
/* hash string, seed with and produce 128bit state */
static inline void fh64_permute_string(const uint8_t *v, size_t len, uint64_t *a, uint64_t *b);
/* state finalization to 64bit value */
static inline uint64_t  fh64_finalize(uint64_t a, uint64_t b);
/* convenient function to hash buffer with 64bit seed */
static inline uint64_t  fh64_string_hash(const void *buf, size_t len, uint64_t seed);
/* convenient function to hash buffer with 128bit seed */
static inline uint64_t  fh64_string_hash2(const void *buf, size_t len, uint64_t seed1, uint64_t seed2);

/* IMPLEMENTATION */

#ifndef FH_READ_UNALIGNED
#define FH_READ_UNALIGNED 1
#endif
#define FH_ROTL(x, n) (((x) << (n)) | ((x) >> (sizeof(x)*8 - (n))))
#ifndef U64_CONSTANT
#define U64_CONSTANT(x) (x##ull)
#endif

static const uint32_t FH_C1 = 0xb8b34b2d;
static const uint32_t FH_C2 = 0x52c6a2d9;
static inline void
fh32_permute(uint32_t v, uint32_t vv, uint32_t *a, uint32_t *b, uint32_t *c)
{
	//*c = FH_ROTL(*c, 16) - *a - *b;
	//*a = FH_ROTL(*a+v, 24) + *b;
	//*b = FH_ROTL(*b+v, 9) ^ (*a*9);
	//*a = FH_ROTL(*a+vv, 24) + *b;
	//*b = FH_ROTL(*b+vv, 9) ^ (*a*9);
	//*b = FH_ROTL(*b+vv, 9) ^ *a;
	//*b = FH_ROTL(*b+vv, 9) ^ (*a*9);
	/*
	*c -= *a ^ *b;
	*c *= FH_C1;
	//*c = FH_ROTL(*c ^ *a, 17) + (*b ^ 0xdeadbeef);
	*a = FH_ROTL(*a, 13); *a -= v;
	*b = FH_ROTL(*b, 11); *b -= vv;
	//*/
	//*
	//*c = FH_ROTL(*c^*a, 25) - (*b^0xdeadbeef);
	*c = FH_ROTL(*c-*a, 25) - (*b^0xdeadbeef);
	*a = FH_ROTL(*a+v, 24) + *b;
	*b = FH_ROTL(*b+vv, 3) ^ *c;
	//*c *= 0xdeafbeef;
	//*/
}

#if FH_READ_UNALIGNED
#define fh_load_u32(v) (*(uint32_t*)(v))
#else
static inline uint32_t
fh_load_u32(const uint8_t *v)
{
	return v[0]|(v[1]<<8)|(v[2]<<16)|(v[3]<<24);
}
#endif

static inline uint32_t
fh_load_u24(const uint8_t *v, unsigned len)
{
	uint32_t x = 0;
	switch(len) {
	case 3: x |= v[2] << 16;
	case 2: x |= v[1] << 8;
	case 1: x |= v[0];
	}
	return x;
}


static inline void
fh32_permute_string(const uint8_t *v, size_t len, uint32_t *a, uint32_t *b)
{
	uint32_t t = len;
	if (len > 8) {
		size_t n = (len-1) / 8;
		*a += fh_load_u32(v + len - 4);
		*b += fh_load_u32(v + len - 8);
		for(; n; n--, v+=8) {
			//fh32_permute(fh_load_u32(v), fh_load_u32(v+4), a, b, &t);
			uint32_t v1 = fh_load_u32(v);
			uint32_t v2 = fh_load_u32(v+4);
			//t = FH_ROTL(t ^ *a, 11) - (*b ^ FH_C1);
			//t = FH_ROTL(t - *a, 11) - FH_ROTL(*b, 23);
			//t -= FH_ROTL(*a, 11);
			t -= FH_ROTL(*a, 11);
			*a += v1; *a -= t;
			t -= FH_ROTL(*b, 23);
			*b += v2; *b -= t;
			//*a = FH_ROTL(*a, 16) - v1;
			//*b = FH_ROTL(*b, 16) - v2;
			//*b ^= *a; *b -= FH_ROTL(*a, 11);
			//*a ^= t; *a -= FH_ROTL(t, 24);
			/*
			t -= *a;
			*a += v1; *a -= FH_ROTL(t, 15);
			t -= *b;
			*b += v2; *b -= FH_ROTL(t, 17);
			*/
		}
		//t -= FH_ROTL(*a, 11);
	       	//t -= FH_ROTL(*b, 23);
	} else if (len >= 4) {
		*a ^= fh_load_u32(v + len - 4);
		*b ^= fh_load_u32(v);
		t -= FH_ROTL(*a, 19);
		t -= FH_ROTL(*b, 15);
		//fh32_permute(fh_load_u32(v), fh_load_u32(v+len-4), a, b, &t);
		//fh32_permute(fh_load_u32(v), fh_load_u32(v+len-4), a, b, &t);
		//t = FH_ROTL(t-*a, 17);
		/*
		*a ^= fh_load_u32(v);
		t ^= fh_load_u32(v+len-4);
		*b ^= fh_load_u32(v+(len>>1)-2);
		t ^= *b; t -= FH_ROTL(*b, 14);
		*b += fh_load_u32(v+(len>>2)-1);
		*/
	} else {
		t = fh_load_u24(v, len);
	}
		//t -= FH_ROTL(*a, 11) ^ FH_ROTL(*b, 23);
		t -= FH_ROTL(*a, 23);
		t -= FH_ROTL(*b, 11);
//#define FH_ADDITIVE_FINALIZE
#ifdef FH_ADDITIVE_FINALIZE
	*b ^= len;
	fh32_permute(t, a, b);
#else
	/*
	*a ^= FH_ROTL(t, 16);
	*b ^= len+t;
	/*/
#define final(a,b,c) \
{ \
  a ^= c; a -= FH_ROTL(c,11); \
  b ^= a; b -= FH_ROTL(a,25); \
  c ^= b; c -= FH_ROTL(b,16); \
  a ^= c; a -= FH_ROTL(c,4);  \
}
	final(*a,*b,t);
	//*a ^= t; *a -= FH_ROTL(t, 11);
	//*b ^= *a; *b -= FH_ROTL(*a, 25);
	//t ^= *b; t -= FH_ROTL(*b, 16);
	//*a = t;
	//*/
#endif
}

#ifdef FH_ADDITIVE_FINALIZE
static inline void
fh32_permute_final(uint32_t *a, uint32_t *b)
{
	*a = (FH_ROTL(*a, 24) + *b) * 9;
	*b = FH_ROTL(*b*5, 5) ^ *a;
}
static inline uint32_t
fh32_finalize(uint32_t a, uint32_t b)
{
	fh32_permute_final(&a, &b);
	fh32_permute_final(&a, &b);
	fh32_permute_final(&a, &b);
	return b;
}
#else
//*
static inline uint32_t
fh32_finalize(uint32_t a, uint32_t b)
{
	return a;
}
/*/
static inline uint32_t
fh32_finalize(uint32_t a, uint32_t b)
{
	a *= FH_C1;
	b *= FH_C2;
	a ^= a >> 16;
	b ^= b >> 16;
	a *= FH_C1;
	b *= FH_C2;
	return a ^ b ^ (a >> 16) ^ (b >> 17);
}
//*/
#endif

static inline uint32_t
fh32_string_hash(const void* d, size_t len, uint32_t seed)
{
	uint32_t a = 0, b = seed;
	fh32_permute_string((const uint8_t*)d, len, &a, &b);
	return fh32_finalize(a, b);
}

static inline uint32_t
fh32_string_hash2(const void* d, size_t len, uint32_t seed1, uint32_t seed2)
{
	uint32_t a = seed1, b = seed2;
	fh32_permute_string((const uint8_t*)d, len, &a, &b);
	return fh32_finalize(a, b);
}

static inline void
fh64_permute(uint64_t v, uint64_t *a, uint64_t *b)
{
	*a = FH_ROTL(*a + v, 56) + *b;
	*b = FH_ROTL(*b + v, 15) ^ (*a * 9);
}

#if FH_READ_UNALIGNED
#define fh_load_u64(v) (*(uint64_t*)(v))
#else
static inline uint64_t
fh_load_u64(const uint8_t *v)
{
	return (uint64_t)fh_load_u32(v) | ((uint64_t)fh_load_u32(v+4) << 32);
}
#endif

static inline uint64_t
fh_load_u56(const uint8_t *v, unsigned len)
{
	uint64_t x;
	if (len < 4) {
		x = fh_load_u24(v, len & 3);
	} else {
		x = fh_load_u32(v);
		x |= (uint64_t)fh_load_u24(v+4, len&3) << 32;
	}
	return x;
}

static inline void
fh64_permute_string(const uint8_t *v, size_t len, uint64_t *a, uint64_t *b)
{
	uint64_t t;
	if (len < 7) {
		t = fh_load_u56(v, len);
	} else {
		size_t n = len / 8;
		for(; n; n--, v+=8) {
			fh64_permute(fh_load_u64(v), a, b);
		}
		t = fh_load_u64(v-8+(len&7));
	}
	*b ^= (uint64_t)len;
	fh64_permute(t, a, b);
}

#ifdef FH_ADDITIVE_FINALIZE
static inline void
fh64_permute_final(uint64_t *a, uint64_t *b)
{
	*a = (FH_ROTL(*a, 48) + *b) * 9;
	*b = FH_ROTL(*b*5, 17) ^ *a;
}
static inline uint64_t
fh64_finalize(uint64_t a, uint64_t b)
{
	fh64_permute_final(&a, &b);
	fh64_permute_final(&a, &b);
	fh64_permute_final(&a, &b);
	return b;
}
#else
static const uint64_t FH_BC1 = U64_CONSTANT(0xacd5ad43274593b9);
static const uint64_t FH_BC2 = U64_CONSTANT(0x6956abd6ed268a3d);
static inline uint64_t
fh64_finalize(uint64_t a, uint64_t b)
{
	a *= FH_BC2;
	b *= FH_BC1;
	a ^= a >> 32;
	b ^= b >> 32;
	a *= FH_BC2;
	b *= FH_BC1;
	return a ^ b ^ (a >> 32) ^ (b >> 33);
}
#endif

static inline uint64_t
fh64_string_hash(const void* d, size_t len, uint64_t seed)
{
	uint64_t a = 0, b = seed;
	fh64_permute_string((const uint8_t*)d, len, &a, &b);
	return fh64_finalize(a, b);
}

static inline uint64_t
fh64_string_hash2(const void* d, size_t len, uint64_t seed1, uint64_t seed2)
{
	uint64_t a = seed1, b = seed2;
	fh64_permute_string((const uint8_t*)d, len, &a, &b);
	return fh64_finalize(a, b);
}

#undef FH_ROTL
#undef FH_READ_UNALIGNED
#endif
