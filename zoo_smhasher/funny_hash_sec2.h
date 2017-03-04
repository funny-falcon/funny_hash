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

struct fh32st {
	uint32_t a, b, c, d;
};

static inline fh32st
fh32_permute(uint32_t v, uint32_t vv, fh32st s)
{
#if 0
#if 0
	s.a = FH_ROTL(s.a, 9) - v;
	s.b = FH_ROTL(s.b, 7) - vv;
	s.c = (FH_ROTL(s.c + s.a, 13)*9) ^ s.b;
	s.d = (FH_ROTL(s.d + s.b, 11)*5) ^ s.a;
	//s.c = FH_ROTL((s.c*9) ^ s.a, 13) + s.b;
	//s.d = FH_ROTL((s.d*5) ^ s.b, 11) + s.a;
	//s.c = FH_ROTL((s.c*9) ^ s.a, 21) + s.b;
	//s.d = FH_ROTL((s.d*5) ^ s.b, 19) + s.a;
#elif 0
	s.a = FH_ROTL(s.a, 13) * 9 - v;
	s.b = FH_ROTL(s.b, 11) * 5 - vv;
	//s.c = FH_ROTL(s.c ^ s.a, 17) + (s.b ^ 0xdeadbeef);
	s.c = FH_ROTL(s.c ^ s.a, 17) + s.b;
	//s.d = FH_ROTL(s.c, 11) * FH_C1 + (s.a * FH_C2 - FH_ROTL(s.b, 1) * 9);
	s.d = s.a - FH_ROTL(s.b, 1);
#elif 0
	// панель вверху
	s.a += v;
	s.b += vv;
	s.c ^= FH_ROTL(s.a, 13);
	s.d ^= FH_ROTL(s.b, 17);
	s.a -= s.d;
	s.b -= s.c;
	s.c ^= FH_ROTL(s.a, 8);
	s.d ^= FH_ROTL(s.b, 5);
#elif 1
	// панель внизу
	s.a ^= v;
	s.b ^= vv;
	s.c += s.a;
	s.d += FH_ROTL(s.b, 1);
	s.a ^= s.d;
	s.b ^= s.c;
	//s.c -= FH_ROTL(s.a, 7); // внизу GOOD!!!!
	s.c -= FH_ROTL(s.a, 5); // вторая сверху
	s.d -= FH_ROTL(s.b, 15);
#else
	// треться сверху
	s.a ^= v;
	s.b ^= vv;
	s.c += FH_ROTL(s.a, 13);
	s.d += FH_ROTL(s.b, 17);
	s.a ^= s.d;
	s.b ^= s.c;
	s.c -= FH_ROTL(s.a, 8);
	s.d -= FH_ROTL(s.b, 5);
#endif
#else
	s.a ^= v;
	s.b ^= vv;
	s.c += s.a;
	s.d += s.b;
	s.a = FH_ROTL(s.a, 21);
	s.b = FH_ROTL(s.b, 7);
	s.a -= s.d;
	s.b -= s.c;
	s.c = FH_ROTL(s.c, 16);
	s.d = FH_ROTL(s.d, 1);
	s.c ^= s.a;
	s.d ^= s.b;
#endif
	return s;
}

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
	//fh32st st = {0xdeadbeef ^ seed, 0xc01e510d ^ seed, seed, seed};
	//fh32st st = {FH_C1, FH_C2, seed, seed};
	fh32st st = {0, 0, seed, 0xdeadbeef^seed};
	//fh32st st = {seed, seed, 0, 0};
	if (len == 0) {
	} else if (len < 4) {
		st = fh32_permute(fh_load_u24(v, len), 0, st);
	} else if (len <= 8) {
		st = fh32_permute(fh_load_u32(v), fh_load_u32(v+len-4), st);
	} else if (len <= 16) {
		st = fh32_permute(fh_load_u32(v), fh_load_u32(v+4), st);
		st = fh32_permute(fh_load_u32(v+len-8), fh_load_u32(v+len-4), st);
	} else {
		for(; len>8; len-=8, v+=8) {
			st = fh32_permute(fh_load_u32(v), fh_load_u32(v+4), st);
		}
		st = fh32_permute(fh_load_u32(v+len-8), fh_load_u32(v+len-4), st);
	}
	uint32_t t = seed ^ len;
#define rot FH_ROTL
#define final_(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}
	//final_(st.c,st.d,t);
#define final2(a,b,c) \
{ \
  c += rot(a,2); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}
	final2(st.c,st.d,t);
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
