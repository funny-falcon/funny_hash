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

Some benchmark results on Core-i7 2600 2400MHz 300M random blob:

x86_64
------

By 1-20byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.18  |   1.17 
funny64   |   1.32  |   1.17 
murmur32  |   1.45  |   1.23 
murmur128 |   1.80  |   1.64 
sip24     |   2.72  |   2.29 
sip13     |   2.34  |   1.88 
lookup3   |   1.57  |   1.43 
spooky    |   1.61  |   1.29 

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.68  |   1.53 
funny64   |   0.86  |   0.79 
murmur32  |   1.49  |   1.57 
murmur128 |   0.72  |   0.89 
sip24     |   2.45  |   2.41 
sip13     |   1.31  |   1.28 
lookup3   |   1.93  |   2.09 
spooky    |   1.04  |   0.99 

x86 (by -m32)
-------------

By 1-20byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.27  |   1.25 
funny64   |   2.42  |   1.98 
murmur32  |   1.56  |   1.34 
murmur128 |   3.36  |   2.93 
sip24     |   6.45  |   4.99 
sip13     |   4.68  |   3.50 
lookup3   |   1.76  |   1.59 
spooky    |   4.19  |   2.79 

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.68  |   1.53 
funny64   |   2.75  |   1.77 
murmur32  |   1.51  |   1.55 
murmur128 |   3.12  |   3.13 
sip24     |   9.07  |  11.18 
sip13     |   4.88  |   5.37 
lookup3   |   2.03  |   1.95 
spooky    |   5.42  |   3.01 
