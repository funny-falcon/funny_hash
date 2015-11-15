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
funny32   |   0.94  |   0.95
funny64   |   0.90  |   0.94
fnv1a     |   0.96  |   0.96
murmur32  |   1.20  |   1.10
murmur128 |   1.45  |   1.60
sip24     |   2.02  |   1.88
sip13     |   1.78  |   1.61
sap24     |   2.08  |   1.86
sap13     |   1.71  |   1.51
bob24     |   1.70  |   1.76
bob13     |   1.39  |   1.47
lookup3   |   1.01  |   1.10
spooky    |   1.13  |   1.12

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.12  |   1.12
funny64   |   0.55  |   0.55
fnv1a     |   3.53  |   3.53
murmur32  |   1.12  |   1.12
murmur128 |   0.45  |   0.50
sip24     |   1.27  |   1.48
sip13     |   0.75  |   0.82
sap24     |   2.53  |   2.95
sap13     |   1.47  |   1.63
bob24     |   1.30  |   1.48
bob13     |   0.74  |   0.82
lookup3   |   1.19  |   1.10
spooky    |   0.75  |   0.73

x86 (by -m32)
-------------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.08  |   1.08
funny64   |   1.67  |   1.60
fnv1a     |   1.05  |   1.08
murmur32  |   1.29  |   1.29
murmur128 |   2.55  |   2.65
sip24     |   5.13  |   4.02
sip13     |   3.83  |   3.06
sap24     |   2.27  |   2.07
sap13     |   1.94  |   1.68
bob24     |   1.79  |   1.97
bob13     |   1.54  |   1.72
lookup3   |   1.31  |   1.15
spooky    |   2.91  |   2.16

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.12  |   1.12
funny64   |   2.02  |   1.22
fnv1a     |   3.53  |   3.53
murmur32  |   1.12  |   1.12
murmur128 |   2.07  |   2.03
sip24     |   5.78  |   4.78
sip13     |   3.41  |   2.59
sap24     |   2.62  |   2.95
sap13     |   1.47  |   1.63
bob24     |   1.33  |   1.48
bob13     |   0.77  |   0.82
lookup3   |   1.19  |   1.10
spooky    |   3.55  |   1.81
