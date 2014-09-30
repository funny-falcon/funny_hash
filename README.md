funny_hash
==========

Simple multiplicative hash function like Murmur3 but a bit safer.

It is safer cause it uses state twice larger than block size, feeds block twice at time and every bit affects at least 10/20 bits of a state. So it is impossible to revert change without introducing other change, and I think it is hardly possible to make seed independent collisions (which is the main defect of Murmur2/Murmur3).

So it is safe for use in a internal hash table implementations when nor seed nor hashsum is known to attacker.

(but it is certainly not cryptographic: I think attack with choosen plaintext and known hashsum will succeed
with little effort)

There is two boxes of building blocks: 32bit hash for 32bit cpu and 64bit hash for 64bit cpu.

````C
/* 64bit state for 32bit result */
typedef ... fh_u64_t;
/* mix 32bit integer into 64bit state */
fh_u64_t fh32_permute(fh_u64_t h, uint32_t v);
/* hash string, seed with and produce 64bit state */
fh_u64_t fh32_permute_string(fh_u64_t h, const uint8_t *v, size_t len);
/* state finalization to 32bit value */
uint32_t fh32_finalize(fh_u64_t h);
/* convenient function to hash buffer with 32bit seed */
uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed);
/* convenient function to hash buffer with 64bit seed */
uint32_t fh32_string_hash2(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

/* 128bit state for 64bit result */
typedef ... fh_u128_t;
/* mix 64bit integer into 128bit state */
fh_u128_t fh64_permute(fh_u128_t h, uint64_t v);
/* hash string, seed with and produce 128bit state */
fh_u128_t fh64_permute_string(fh_u128_t h, const uint8_t *v, size_t len);
/* state finalization to 64bit value */
uint64_t fh64_finalize(fh_u128_t h);
/* convenient function to hash buffer with 64bit seed */
uint64_t fh64_string_hash(const void *buf, size_t len, uint64_t seed);
/* convenient function to hash buffer with 128bit seed */
uint64_t fh64_string_hash2(const void *buf, size_t len, uint64_t seed1, uint64_t seed2);
````

Benchmark
=========

Some benchmark results on Core-i7 2600 2400MHz 300M random blob:

x86_64
------

By 1-20byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.21  |   1.15 
funny64   |   1.33  |   1.21 
murmur32  |   1.45  |   1.24 
murmur128 |   1.77  |   1.65 
sip24     |   2.77  |   2.26 
sip13     |   2.34  |   1.92 
lookup3   |   1.76  |   1.40 
spooky    |   1.60  |   1.29 

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.68  |   1.53 
funny64   |   0.86  |   0.79 
murmur32  |   1.49  |   1.57 
murmur128 |   0.72  |   0.89 
sip24     |   2.45  |   2.41 
sip13     |   1.27  |   1.28 
lookup3   |   1.93  |   2.09 
spooky    |   1.04  |   0.99 

x86 (by -m32)
-------------

By 1-20byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.28  |   1.23 
funny64   |   2.58  |   1.89 
murmur32  |   1.54  |   1.34 
murmur128 |   3.31  |   2.94 
sip24     |   6.40  |   5.02 
sip13     |   4.68  |   3.53 
lookup3   |   1.77  |   1.60 
spooky    |   4.17  |   2.83 

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.68  |   1.53 
funny64   |   2.76  |   1.82 
murmur32  |   1.51  |   1.55 
murmur128 |   3.12  |   3.13 
sip24     |   9.05  |  11.18 
sip13     |   5.05  |   5.37 
lookup3   |   2.03  |   1.95 
spooky    |   5.30  |   3.01 
