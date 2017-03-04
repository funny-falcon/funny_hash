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
static const uint32_t FH_C1 = 0xb8b34b2d;
static const uint32_t FH_C2 = 0x52c6a2d9;

#ifndef FH_READ_UNALIGNED
#define FH_READ_UNALIGNED 1
#endif
#define FH_ROTL(x, n) (((x) << (n)) | ((x) >> (sizeof(x)*8 - (n))))
#ifndef U64_CONSTANT
#define U64_CONSTANT(x) (x##ull)
#endif

#define fh_load_u32(v) (*(uint32_t*)(v))

static inline uint32_t
fh_load_u24(const uint8_t *v, unsigned len)
{
	uint32_t x = v[0];
	x |= (uint32_t)v[len/2] << 8;
	x |= (uint32_t)v[len-1] << 16;
	return x;
}


#define unlikely(a) (__builtin_expect(!!(a), 0))
#define likely(a) (__builtin_expect(!!(a), 1))
static inline uint32_t
fh32_permute_string(const uint8_t *v, size_t len, uint32_t seed)
{
	uint32_t a=0;
	uint32_t b=0;
	uint32_t c=seed;
	uint32_t d=0xdeadbeef^seed;
	uint32_t l = len;
	uint32_t t;
	switch (len) {
	case 0: break;
	case 1: case 2: case 3:
		t = fh_load_u24(v, len);
		a ^= t;
		b ^= t;
		break;
	case 4:
		t = fh_load_u32(v);
		a ^= t;
		b ^= t;
		break;
	case 5: case 6: case 7:
		a ^= fh_load_u32(v);
		b ^= fh_load_u24(v+4, len&3);
		break;
	case 8:
		a ^= fh_load_u32(v);
		b ^= fh_load_u32(v+4);
		break;
	default:
		for(; len>8; len-=8, v+=8) {
			a -= fh_load_u32(v);
			b -= fh_load_u32(v+4);
			c += a;
			d += b;
			a = FH_ROTL(a, 5) - d;
			b = FH_ROTL(b, 7) - c;
			c = FH_ROTL(c, 16) ^ a;
			d = FH_ROTL(d, 1) ^ b;
		}
		a -= fh_load_u32(v+len-8);
		b -= fh_load_u32(v+len-4);
	}
#define rota 13
#define rotb 22
#define rotd 31
	c ^= b; c -= FH_ROTL(a, rota);
	d ^= a; d -= FH_ROTL(b, rotb);
	t = (seed ^ l);
	t += c; t += FH_ROTL(d,rotd);
	d ^= c; d -= FH_ROTL(c,25);
	t ^= d; t -= FH_ROTL(d,16);
	c ^= t; c -= FH_ROTL(t,4);
	d ^= c; d -= FH_ROTL(c,14);
	t ^= d; t -= FH_ROTL(d,24);
	return t;
}

static inline uint32_t
fh32_string_hash(const void* d, size_t len, uint32_t seed)
{
	return fh32_permute_string((const uint8_t*)d, len, seed);
}

struct fh64st {
	uint64_t a, b, c, d;
};

static inline fh64st
fh64_permute(uint64_t v, uint64_t vv, fh64st s)
{
	s.a = FH_ROTL(s.a, 9) - v;
	s.b = FH_ROTL(s.b, 7) - vv;
	s.c = FH_ROTL((s.c*9) ^ s.a, 13) + s.b;
	s.d = FH_ROTL((s.d*5) ^ s.b, 11) + s.a;
	//s.c = FH_ROTL((s.c*9) ^ s.a, 21) + s.b;
	//s.d = FH_ROTL((s.d*5) ^ s.b, 19) + s.a;
	return s;
}

#define fh_load_u64(v) (*(uint64_t*)(v))

//static inline void
//fh64_permute_string(const uint8_t *v, size_t len, uint64_t *a, uint64_t *b)
	//fh64st st = {0xdeadbeef ^ *a, 0xc01e510d ^ *b, len ^ *a, len ^ *b};
static inline uint64_t
fh64_permute_string(const uint8_t *v, size_t len, uint32_t seed)
{
	//fh64st st = {0xdeadbeef ^ seed, 0xc01e510d ^ seed, len ^ seed, len ^ seed};
	fh64st st = {0xdeadbeef ^ seed, 0xc01e510d ^ seed, seed, seed};
	uint64_t t = len ^ seed;
	if (unlikely(len > 32)) {
		for(; len>16; len-=16, v+=16) {
			st = fh64_permute(fh_load_u64(v), fh_load_u64(v+8), st);
		}
		st = fh64_permute(fh_load_u64(v+len-16), fh_load_u64(v+len-8), st);
	} else if (len == 0) {
	} else if (len < 4) {
		st = fh64_permute(fh_load_u24(v, len), 0, st);
	} else if (len <= 8) {
		st = fh64_permute(fh_load_u32(v), fh_load_u32(v+len-4), st);
	} else if (len <= 16) {
		st = fh64_permute(fh_load_u64(v), fh_load_u64(v+len-8), st);
	} else {
		st = fh64_permute(fh_load_u64(v), fh_load_u64(v+8), st);
		st = fh64_permute(fh_load_u64(v+len-16), fh_load_u64(v+len-8), st);
	}
	//*a = st.c;
	//*b = st.d;
#define rot FH_ROTL
#define final64(a,b,c) \
{ \
  c ^= b; c -= rot(b,30); \
  a ^= c; a -= rot(c,22); \
  b ^= a; b -= rot(a,50); \
  c ^= b; c -= rot(b,31); \
  a ^= c; a -= rot(c,7);  \
  b ^= a; b -= rot(a,27); \
  c ^= b; c -= rot(b,48); \
}
	final64(st.c,st.d,t);
	return t;
}

/*
static inline uint64_t
fh64_finalize(uint64_t a, uint64_t b)
{
#define rot FH_ROTL
#define final64(a,b,c) \
{ \
  c ^= b; c -= rot(b,30); \
  a ^= c; a -= rot(c,22); \
  b ^= a; b -= rot(a,50); \
  c ^= b; c -= rot(b,31); \
  a ^= c; a -= rot(c,7);  \
  b ^= a; b -= rot(a,27); \
  c ^= b; c -= rot(b,48); \
}
	uint64_t t = 0;
	final64(a,b,t);
	return t;
}
*/

static inline uint64_t
fh64_string_hash(const void* d, size_t len, uint32_t seed)
{
	return fh64_permute_string((const uint8_t*)d, len, seed);
	//fh64_permute_string((const uint8_t*)d, len, &a, &b);
	//return fh64_finalize(a, b);
}

/*
static inline uint64_t
fh64_string_hash2(const void* d, size_t len, uint64_t seed1, uint64_t seed2)
{
	uint64_t a = seed1, b = seed2;
	fh64_permute_string((const uint8_t*)d, len, &a, &b);
	return fh64_finalize(a, b);
}
*/

#undef FH_ROTL
#undef FH_READ_UNALIGNED
#endif
