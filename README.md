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
/* convinient function to hash buffer with 32bit seed */
uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed);
/* convinient function to hash buffer with 64bit seed */
uint32_t fh32_string_hash2(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

/* 128bit state for 64bit result */
typedef ... fh_u128_t;
/* mix 64bit integer into 128bit state */
fh_u128_t fh64_permute(fh_u128_t h, uint64_t v);
/* hash string, seed with and produce 128bit state */
fh_u128_t fh64_permute_string(fh_u128_t h, const uint8_t *v, size_t len);
/* state finalization to 64bit value */
uint64_t fh64_finalize(fh_u128_t h);
/* convinient function to hash buffer with 64bit seed */
uint64_t fh64_string_hash(const void *buf, size_t len, uint64_t seed);
/* convinient function to hash buffer with 128bit seed */
uint64_t fh64_string_hash2(const void *buf, size_t len, uint64_t seed1, uint64_t seed2);
````

Benchmark
=========

Some benchmark results on Core-i3 2330M 2000MHz 300M random file:

x86_64
------

By 1-20byte substrings twice

function  | gcc -O2  | clang -O2
----------|----------|---------
funny32   | 1.729355 | 1.602891
murmur32  | 1.990929 | 1.679176
funny64   | 1.848294 | 1.672171
murmur128 | 2.501298 | 2.355333
lookup3   | 2.210184 | 2.007941
sip13     | 3.173038 | 2.656347
sip24     | 3.761968 | 3.195745

10 times 300M at once

function  | gcc -O2  | clang -O2
----------|----------|----------
funny32   | 2.536092 | 2.229605
murmur32  | 2.150359 | 2.239955
funny64   | 1.284104 | 1.186942
murmur128 | 1.103798 | 1.352376
lookup3   | 2.774540 | 2.939180
sip13     | 1.872111 | 1.871094
sip24     | 3.431894 | 3.312722

x86 (by -m32)
-------------

By 1-20byte substrings twice

function  | gcc -O2  | clang -O2
----------|----------|----------
funny32   | 1.912340 | 1.779213
murmur32  | 2.189364 | 1.874402
funny64   | 3.432760 | 2.674275
murmur128 | 4.880114 | 4.175704
lookup3   | 2.515043 | 2.212417
sip13     | 6.412115 | 4.830465
sip24     | 8.840023 | 6.891309


10 times 300M at once

function  | gcc -O2   | clang -O2 
----------|-----------|----------
funny32   | 2.392824  | 2.229941
murmur32  | 2.180043  | 2.240384
funny64   | 3.851627  | 2.761048 
murmur128 | 4.354525  | 4.404073
lookup3   | 2.777132  | 2.952843
sip13     | 6.986343  | 7.399185
sip24     | 12.547801 | 15.172169
