#include "Hashes.h"
#include "Random.h"

#include <stdlib.h>
//#include <stdint.h>
#include <assert.h>
//#include <emmintrin.h>
//#include <xmmintrin.h>

// ----------------------------------------------------------------------------
//fake / bad hashes

void
BadHash(const void *key, int len, uint32_t seed, void *out)
{
  uint32_t	  h = seed;
  const uint8_t  *data = (const uint8_t *)key;

  for (int i = 0; i < len; i++) {
    h ^= h >> 3;
    h ^= h << 5;
    h ^= data[i];
  }

  *(uint32_t *) out = h;
}

void
sumhash(const void *key, int len, uint32_t seed, void *out)
{
  uint32_t	  h = seed;
  const uint8_t  *data = (const uint8_t *)key;

  for (int i = 0; i < len; i++) {
    h += data[i];
  }

  *(uint32_t *) out = h;
}

void
sumhash32(const void *key, int len, uint32_t seed, void *out)
{
  uint32_t	  h = seed;
  const uint32_t *data = (const uint32_t *)key;

  for (int i = 0; i < len / 4; i++) {
    h += data[i];
  }

  *(uint32_t *) out = h;
}

void
DoNothingHash(const void *, int, uint32_t, void *)
{
}

void
NoopOAATReadHash(const void *key, int len, uint32_t seed, void *out)
{
	volatile uint8_t c;
	const uint8_t *ptr = (uint8_t *)key;

	for(int i=0; i < len; i++)
	{
			c=ptr[i];
	}
}

//-----------------------------------------------------------------------------
//One - byte - at - a - time hash based on Murmur 's mix

uint32_t MurmurOAAT(const void *key, int len, uint32_t seed)
{
  const uint8_t  *data = (const uint8_t *)key;
  uint32_t	  h = seed;

  for (int i = 0; i < len; i++) {
    h ^= data[i];
    h *= 0x5bd1e995;
    h ^= h >> 15;
  }

  return h;
}

void
MurmurOAAT_test(const void *key, int len, uint32_t seed, void *out)
{
  *(uint32_t *) out = MurmurOAAT(key, len, seed);
}

//----------------------------------------------------------------------------

void
FNV32a(const void *key, int len, uint32_t seed, void *out)
{
  unsigned int	  h = seed;
  const uint8_t  *data = (const uint8_t *)key;

  h ^= BIG_CONSTANT(2166136261);

  for (int i = 0; i < len; i++) {
    h ^= data[i];
    h *= 16777619;
  }

  *(uint32_t *) out = h;
}

void
FNV32a_YoshimitsuTRIAD(const void *key, int len, uint32_t seed, void *out)
{
  const uint8_t  *p = (const uint8_t *)key;
  const uint32_t  PRIME = 709607;
  uint32_t	  hash32A = seed ^ 2166136261u;
  uint32_t	  hash32B = 2166136261u + len;
  uint32_t	  hash32C = 2166136261u;

  for (; len >= 3 * 2 * sizeof(uint32_t); len -= 3 * 2 * sizeof(uint32_t), p += 3 * 2 * sizeof(uint32_t)) {
    hash32A = (hash32A ^ (ROTL32(*(uint32_t *) (p + 0), 5)  ^ *(uint32_t *) (p + 4)))  * PRIME;
    hash32B = (hash32B ^ (ROTL32(*(uint32_t *) (p + 8), 5)  ^ *(uint32_t *) (p + 12))) * PRIME;
    hash32C = (hash32C ^ (ROTL32(*(uint32_t *) (p + 16), 5) ^ *(uint32_t *) (p + 20))) * PRIME;
  }
  if (p != key) {
    hash32A = (hash32A ^ ROTL32(hash32C, 5)) * PRIME;
  }
  //Cases 0. .31
  if (len & 4 * sizeof(uint32_t)) {
    hash32A = (hash32A ^ (ROTL32(*(uint32_t *) (p + 0), 5) ^ *(uint32_t *) (p + 4))) * PRIME;
    hash32B = (hash32B ^ (ROTL32(*(uint32_t *) (p + 8), 5) ^ *(uint32_t *) (p + 12))) * PRIME;
    p += 8 * sizeof(uint16_t);
  }
  //Cases 0. .15
  if (len & 2 * sizeof(uint32_t)) {
    hash32A = (hash32A ^ *(uint32_t *) (p + 0)) * PRIME;
    hash32B = (hash32B ^ *(uint32_t *) (p + 4)) * PRIME;
    p += 4 * sizeof(uint16_t);
  }
  //Cases:0. .7
  if (len & sizeof(uint32_t)) {
    hash32A = (hash32A ^ *(uint16_t *) (p + 0)) * PRIME;
    hash32B = (hash32B ^ *(uint16_t *) (p + 2)) * PRIME;
    p += 2 * sizeof(uint16_t);
  }
  //Cases:0. .3
  if (len & sizeof(uint16_t)) {
    hash32A = (hash32A ^ *(uint16_t *) p) * PRIME;
    p += sizeof(uint16_t);
  }
  if (len & 1)
    hash32A = (hash32A ^ *p) * PRIME;

  hash32A = (hash32A ^ ROTL32(hash32B, 5)) * PRIME;
  *(uint32_t *) out = hash32A ^ (hash32A >> 16);
}

void
FNV64a(const void *key, int len, uint32_t seed, void *out)
{
  uint64_t	  h = (uint64_t) seed;
  const uint8_t  *data = (const uint8_t *)key;

  h ^= BIG_CONSTANT(0xcbf29ce484222325);

  for (int i = 0; i < len; i++) {
    h ^= data[i];
    h *= 0x100000001b3ULL;
  }

  *(uint64_t *) out = h;
}

//-----------------------------------------------------------------------------

uint32_t x17(const void *key, int len, uint32_t h)
{
  const uint8_t  *data = (const uint8_t *)key;

  for (int i = 0; i < len; ++i) {
    h = 17 * h + (data[i] - ' ');
  }

  return h ^ (h >> 16);
}

void
x17_test(const void *key, int len, uint32_t seed, void *out)
{
  *(uint32_t *) out = x17(key, len, seed);
}

//-----------------------------------------------------------------------------

//also used in perl5 as djb2
void
Bernstein(const void *key, int len, uint32_t seed, void *out)
{
  const uint8_t  *data = (const uint8_t *)key;

  for (int i = 0; i < len; ++i) {
    //seed = ((seed << 5) + seed) + data[i];
    seed = 33 * seed + data[i];
  }

  *(uint32_t *) out = seed;
}

//as used in perl5
void
sdbm(const void *key, int len, uint32_t hash, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  //note that perl5 adds the seed to the end of key, which looks like cargo cult
    while (str < end) {
    hash = (hash << 6) + (hash << 16) - hash + *str++;
  }
  *(uint32_t *) out = hash;
}

//as used in perl5 as one_at_a_time_hard
void
JenkinsOOAT(const void *key, int len, uint32_t hash, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint64_t	  s = (uint64_t) hash;
  unsigned char  *seed = (unsigned char *)&s;
  //unsigned char seed[8];
  //note that perl5 adds the seed to the end of key, which looks like cargo cult
    while (str < end) {
    hash += (hash << 10);
    hash ^= (hash >> 6);
    hash += *str++;
  }

  hash += (hash << 10);
  hash ^= (hash >> 6);
  hash += seed[4];

  hash += (hash << 10);
  hash ^= (hash >> 6);
  hash += seed[5];

  hash += (hash << 10);
  hash ^= (hash >> 6);
  hash += seed[6];

  hash += (hash << 10);
  hash ^= (hash >> 6);
  hash += seed[7];

  hash += (hash << 10);
  hash ^= (hash >> 6);

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash = hash + (hash << 15);
  *(uint32_t *) out = hash;
}

//as used in perl5 until 5.17(one_at_a_time_old)
  void		  _JenkinsOOAT_perl(const void *key, int len, uint32_t hash, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  hash ^= 0x3b00;
  while (str < end) {
    hash += *str++;
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash = hash + (hash << 15);
  *(uint32_t *) out = hash;
}

#define jrol(x,k) (((x)<<(k))|((x)>>(32-(k))))
#define jror(x,k) (((x)>>(k))|((x)<<(32-(k))))
  void		  JenkinsOOAT_perl(const void *key, int len, uint32_t seed, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint32_t h1 = seed ^ 0x3b00;
  uint32_t h2 = jrol(seed, 15);
  for (;len; len--, str++) {
    h1 += str[0];
    h1 *= 9;
    h2 += h1;
    h2 = jrol(h2, 7);
    h2 *= 5;
  }
  h1 ^= h2;
  /* If your table uses prime numbers,
   * then `h1` already contains good value */
  h1 += jrol(h2, 14);
  *(uint32_t *) out = h1;
  return;
  h2 ^= h1; h2 += jror(h1, 6);
  h1 ^= h2; h1 += jrol(h2, 5);
  h2 ^= h1; h2 += jror(h1, 8);
  *(uint32_t *) out = h2;
  return;
  //h1 += jrol(h2, 9);
  h2 ^= h1; h2 -= jror(h1, 6);
  /* If you table is power2 larger than 256 elements,
   * than `h2` already contains good value */
  h1 ^= h2; h1 += jrol(h2, 5);
  h2 ^= h1; h2 += jror(h1, 7);
  //*(uint32_t *) out = h2;
  /* Avalanche of h2 is under 2% and DiffDist passed, so it is practically usable */
  h1 ^= h2; h1 += jrol(h2, 16);
  /* Avalanche (and whole SMHasher) passed */
  *(uint32_t *) out = h1;
}
  void		  JenkinsOOAT_perl2(const void *key, int len, uint32_t seed, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint32_t h1 = seed ^ 0x3b0000;
  uint32_t h2 = jrol(seed, 15);
  for (;len>1; len-=2, str+=2) {
    h1 += str[0]|(str[1]<<8);
    h1 *= 9;
    h2 -= h1;
    h1 = jrol(h1, 7) - h2;
    h2 = jror(h2, 12);
  }
  //for (;len; len--, str++) {
  if (len) {
    h1 += str[0];
    h1 *= 9;
    h2 -= h1;
    h1 = jrol(h1, 7);
    h2 = jror(h2, 12);
  }
  h1 ^= h2;
  /* If your table uses prime numbers,
   * then `h1` already contains good value */
  h1 += jrol(h2, 9);
  h2 ^= h1; h2 -= jror(h1, 6);
  /* If you table is power2 larger than 256 elements,
   * than `h2` already contains good value */
  h1 ^= h2; h1 += jrol(h2, 5);
  h2 ^= h1; h2 += jror(h1, 7);
  /* Avalanche of h2 is under 2% and DiffDist passed, so it is practically usable */
  h1 ^= h2; h1 += jrol(h2, 16);
  /* Avalanche (and whole SMHasher) passed */
  *(uint32_t *) out = h1;
}
  void		  JenkinsOOAT_perl3(const void *key, int len, uint32_t seed, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint32_t h1 = seed ^ 0x3b0000;
  uint32_t h2 = jrol(seed, 15);
  for (;len>1; len-=2, str+=2) {
    h1 += str[0]|(str[1]<<8);
    h1 *= 9;
    h2 += h1;
    h1 = jrol(h1, 11);
    h2 = jror(h2, 8);
  }
  //for (;len; len--, str++) {
  if (len) {
    h1 += str[0];
    h1 *= 9;
    h2 -= h1;
    h1 = jrol(h1, 7);
    h2 = jror(h2, 12);
  }
  h1 ^= h2;
  /* If your table uses prime numbers,
   * then `h1` already contains good value */
  h1 += jrol(h2, 9);
  h2 ^= h1; h2 -= jror(h1, 6);
  /* If you table is power2 larger than 256 elements,
   * than `h2` already contains good value */
  h1 ^= h2; h1 += jrol(h2, 5);
  h2 ^= h1; h2 += jror(h1, 7);
  /* Avalanche of h2 is under 2% and DiffDist passed, so it is practically usable */
  h1 ^= h2; h1 += jrol(h2, 16);
  /* Avalanche (and whole SMHasher) passed */
  *(uint32_t *) out = h1;
}
  void		  JenkinsOOAT_perl5(const void *key, int len, uint32_t seed, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint32_t h1 = seed;
  uint32_t h2 = seed ^ 0xff;
  for (;len>3; len-=4, str+=4) {
    h1 += str[0]|(str[1]<<8)|(str[2]<<16)|(str[3]<<24);
    h2 ^= h1;
    h1 = jrol(h1, 7) + h2;
    h2 = jror(h2, 7) ^ h1;
    h1 = jrol(h1, 7);
  }
  h2 ^= len;
  if (len) {
    h1 += str[0]|(str[len/2]<<8)|(str[len-1]<<16);
    h2 ^= h1;
    h2 -= jrol(h1, 19);
  }
  h1 ^= h2; h1 += jrol(h2, 9);
  h2 ^= h1; h2 -= jror(h1, 6);
  h1 ^= h2; h1 += jrol(h2, 5);
  h2 ^= h1; h2 += jror(h1, 7);
  h1 ^= h2; h1 += jrol(h2, 16);
  *(uint32_t *) out = h1;
}
  void		  JenkinsOOAT_perl8(const void *key, int len, uint32_t seed, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint32_t h1 = seed;
  uint32_t h2 = seed ^ 0xff;
  for (;len>3; len-=4, str+=4) {
    h1 += str[0]|(str[1]<<8)|(str[2]<<16)|(str[3]<<24);
    h2 ^= h1;
    h1 = jrol(h1, 7) + h2;
    h2 = jror(h2, 7) ^ h1;
    h1 = jrol(h1, 7);
  }
  if (len) {
    h1 += str[0]|(str[len/2]<<8)|(str[len-1]<<16);
    h2 ^= h1; h2 -= jrol(h1, 19);
  }
  h1 ^= h2; h1 += jrol(h2, 9);
  h2 ^= h1; h2 -= jror(h1, 6);
  h1 ^= h2; h1 += jrol(h2, 5);
  h2 ^= h1; h2 += jror(h1, 7);
  //h1 ^= h2; h1 += jrol(h2, 16);
  *(uint32_t *) out = h2;
}

  void		  JenkinsOOAT_perl4(const void *key, int len, uint32_t seed, void *out)
{
  unsigned char  *str = (unsigned char *)key;
  const unsigned char *const end = (const unsigned char *)str + len;
  uint32_t h1 = seed ^ 0x3b0000;
  uint32_t h2 = jrol(seed, 15);
  for (;len>3; len-=4, str+=4) {
    h1 += str[0]|(str[1]<<8)|(str[2]<<16)|(str[3]<<24);
    h2 ^= h1;
    h1 = jrol(h1, 7) + h2;
    h2 = jror(h2, 7) ^ h1;
    h1 = jrol(h1, 7);
  }
  h2 ^= len;
  if (len) {
    h1 += str[0]|(str[len/2]<<8)|(str[len-1]<<16);
    h2 ^= h1;
    h2 += jrol(h1, 19);
  }
#if 0
    h1 = jrol(h1, 9) + h2;
    h2 = jror(h2, 13) ^ h1;
    h1 = jrol(h1, 16) - h2;
    h2 = jror(h2, 11) ^ h1;
    h1 = jrol(h1, 6) + h2;
    h2 = jror(h2, 11) ^ h1;
    h1 = jrol(h1, 17) - h2;
    h2 = jror(h2, 8) ^ h1;
#else
    h1 = jrol(h1, 1) + h2;
    h2 = jror(h2, 5) ^ h1;
    h1 = jrol(h1, 24) - h2;
    h2 = jror(h2, 7) ^ h1;
    h1 = jrol(h1, 2) + h2;
    h2 = jror(h2, 9) ^ h1;
    h1 = jrol(h1, 19) - h2;
    h2 = jror(h2, 8) ^ h1;
    h1 = jrol(h2, 11) + h2;
#endif
  *(uint32_t *) out = h1;
  return;
#define r1 11
    h1 = jrol(h1, r1) + h2;
  *(uint32_t *) out = h1;
}
//-----------------------------------------------------------------------------
//Crap8 hash from http://www.team5150.com / ~andrew / noncryptohashzoo / Crap8.html

uint32_t Crap8(const uint8_t * key, uint32_t len, uint32_t seed)
{
#define c8fold( a, b, y, z ) { p = (uint32_t)(a) * (uint64_t)(b); y ^= (uint32_t)p; z ^= (uint32_t)(p >> 32); }
#define c8mix( in ) { h *= m; c8fold( in, m, k, h ); }

  const uint32_t  m = 0x83d2e73b, n = 0x97e1cc59, *key4 = (const uint32_t *)key;
  uint32_t	  h = len + seed, k = n + len;
  uint64_t	  p;

  while (len >= 8) {
    c8mix(key4[0]) c8mix(key4[1]) key4 += 2;
    len -= 8;
  }
  if (len >= 4) {
    c8mix(key4[0]) key4 += 1;
    len -= 4;
  }
  if (len) {
    c8mix(key4[0] & ((1 << (len * 8)) - 1))
  }
  c8fold(h ^ k, n, k, k)
    return k;
}

void
Crap8_test(const void *key, int len, uint32_t seed, void *out)
{
  *(uint32_t *) out = Crap8((const uint8_t *)key, len, seed);
}

extern		"C" {
#ifdef __SSE2__
  void		  hasshe2 (const void *input, int len, uint32_t seed, void *out);
#endif
#if defined(__SSE4_2__) && defined(__x86_64__)
  uint32_t	  crc32c_hw(const void *input, int len, uint32_t seed);
  uint32_t	  crc32c(const void *input, int len, uint32_t seed);
  uint64_t	  crc64c_hw(const void *input, int len, uint32_t seed);
#endif
}

#ifdef __SSE2__
void
hasshe2_test(const void *input, int len, uint32_t seed, void *out)
{
  if (!len) {
    *(uint32_t *) out = 0;
    return;
  }
  if (len % 16) {
    //add pad NUL
      len += 16 - (len % 16);
  }
  hasshe2(input, len, seed, out);
}
#endif

#if defined(__SSE4_2__) && defined(__x86_64__)
/* Compute CRC-32C using the Intel hardware instruction. */
void
crc32c_hw_test(const void *input, int len, uint32_t seed, void *out)
{
  if (!len) {
    *(uint32_t *) out = 0;
    return;
  }
  *(uint32_t *) out = crc32c_hw(input, len, seed);
}
/* Compute CRC-64C using the Intel hardware instruction. */
void
crc64c_hw_test(const void *input, int len, uint32_t seed, void *out)
{
  if (!len) {
    *(uint64_t *) out = 0;
    return;
  }
  *(uint64_t *) out = crc64c_hw(input, len, seed);
}
/* Faster Adler SSE4.2 crc32 in HW */
inline void
crc32c_hw1_test(const void *input, int len, uint32_t seed, void *out)
{
  if (!len) {
    *(uint32_t *) out = 0;
    return;
  }
  *(uint32_t *) out = crc32c(input, len, seed);
}
#endif

/* Cloudflare optimized zlib crc32 with PCLMUL */
#if 0
inline void
zlib_crc32_test(const void *input, int len, uint32_t seed, void *out)
{
    if (!len) {
      *(uint32_t *) out = 0;
      return;
    }
    *(uint32_t *) out = crc32(seed, input, (unsigned)len);
}
#endif

extern		"C" {
  uint64_t	  siphash(const unsigned char key[16], const unsigned char *m, size_t len);
  uint32_t	  saphash(const unsigned char key[8], const unsigned char *m, size_t len);
}
/* https://github.com/floodyberry/siphash */
void
siphash_test(const void *input, int len, uint32_t seed, void *out)
{
  unsigned char	  key[16] = {0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0};
  if (!len) {
    *(uint64_t *) out = 0;
    return;
  }
  memcpy(key, &seed, sizeof(seed));
  *(uint64_t *) out = siphash(key, (const unsigned char *)input, (size_t) len);
}

/* https://github.com/floodyberry/saphash */
void
saphash_test(const void *input, int len, uint32_t seed, void *out)
{
  unsigned char	  key[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (!len) {
    *(uint32_t *) out = 0;
    return;
  }
  memcpy(key, &seed, sizeof(seed));
  *(uint32_t *) out = saphash(key, (const unsigned char *)input, (size_t) len);
}

/* https://github.com/gamozolabs/falkhash */
#if defined(__SSE4_2__) && defined(__x86_64__)
extern "C" {
  uint64_t falkhash_test(uint8_t *data, uint64_t len, uint32_t seed, void *out);
}
void
falkhash_test_cxx(const void *input, int len, uint32_t seed, void *out)
{
  uint64_t hash[2] = {0ULL, 0ULL};
  if (!len) {
    *(uint32_t *) out = 0;
    return;
  }
  falkhash_test((uint8_t *)input, (uint64_t)len, seed, hash);
  *(uint64_t *) out = hash[0];
}
#endif
