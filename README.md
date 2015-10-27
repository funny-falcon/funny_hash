funny_hash
==========

Simple multiplicative hash function like Murmur3 but a bit safer.

It is safer cause it uses state twice larger than block size, feeds block twice at time and every bit affects at least 10/20 bits of a state. So it is impossible to revert change without introducing other change, and I think it is hardly possible to make seed independent collisions (which is the main defect of Murmur2/Murmur3).

So it is safe for use in a internal hash table implementations when nor seed nor hashsum is known to attacker.

(but it is certainly not cryptographic: I think attack with choosen plaintext and known hashsum will succeed
with little effort)

There is two boxes of building blocks: 32bit hash for 32bit cpu and 64bit hash for 64bit cpu.

````C
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
````

Benchmark
=========

Some benchmark results on Core-i7 4770 3400MHz 300M random blob:

x86_64
------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   0.97  |   0.96
funny64   |   0.92  |   1.01
fnv1a     |   0.96  |   0.96
murmur32  |   1.20  |   1.10
murmur128 |   1.47  |   1.60
sip24     |   2.08  |   1.89
sip13     |   1.77  |   1.69
sap24     |   2.06  |   1.86
sap13     |   1.76  |   1.58
lookup3   |   1.02  |   1.12
spooky    |   1.14  |   1.14

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.19  |   1.16
funny64   |   0.69  |   0.67
fnv1a     |   3.55  |   3.54
murmur32  |   1.19  |   1.19
murmur128 |   0.57  |   0.55
sip24     |   1.33  |   1.51
sip13     |   0.88  |   0.91
sap24     |   2.56  |   2.97
sap13     |   1.53  |   1.66
lookup3   |   1.27  |   1.17
spooky    |   0.82  |   0.83

x86 (by -m32)
-------------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.08  |   1.11
funny64   |   1.74  |   1.64
fnv1a     |   1.03  |   1.08
murmur32  |   1.30  |   1.32
murmur128 |   2.69  |   2.73
sip24     |   5.31  |   4.11
sip13     |   3.95  |   3.14
sap24     |   2.30  |   2.12
sap13     |   2.03  |   1.72
lookup3   |   1.33  |   1.17
spooky    |   3.03  |   2.13

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.14  |   1.21
funny64   |   2.05  |   1.31
fnv1a     |   3.55  |   3.56
murmur32  |   1.14  |   1.18
murmur128 |   2.13  |   2.10
sip24     |   6.03  |   4.91
sip13     |   3.44  |   2.65
sap24     |   2.60  |   3.01
sap13     |   1.65  |   1.68
lookup3   |   1.40  |   1.15
spooky    |   3.73  |   1.83
