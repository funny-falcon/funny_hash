// SpookyHash: a 128-bit noncryptographic hash function
//
// This is part for short-length messages - fast for small strings,
// but not as fast for big strings as full SpookyHash.
//
// Also, I change it to take 64bit seed and return 64bit hash.
//
// C-version was taken from https://github.com/andikleen/spooky-c
//

#if defined(__i386__) || defined(__x86_64__) // add more architectures here
#define ALLOW_UNALIGNED_READS 1
#else
#define ALLOW_UNALIGNED_READS 0
#endif

#define SC_CONST 0xdeadbeefdeadbeefLL
#define rot64(x, r) (((x) << (r)) | ((x) >> (64 - (r))))

static inline void short_mix
(
	uint64_t *h0,
	uint64_t *h1,
	uint64_t *h2,
	uint64_t *h3
)
{
	*h2 = rot64(*h2, 50);	*h2 += *h3;  *h0 ^= *h2;
	*h3 = rot64(*h3, 52);	*h3 += *h0;  *h1 ^= *h3;
	*h0 = rot64(*h0, 30);	*h0 += *h1;  *h2 ^= *h0;
	*h1 = rot64(*h1, 41);	*h1 += *h2;  *h3 ^= *h1;
	*h2 = rot64(*h2, 54);	*h2 += *h3;  *h0 ^= *h2;
	*h3 = rot64(*h3, 48);	*h3 += *h0;  *h1 ^= *h3;
	*h0 = rot64(*h0, 38);	*h0 += *h1;  *h2 ^= *h0;
	*h1 = rot64(*h1, 37);	*h1 += *h2;  *h3 ^= *h1;
	*h2 = rot64(*h2, 62);	*h2 += *h3;  *h0 ^= *h2;
	*h3 = rot64(*h3, 34);	*h3 += *h0;  *h1 ^= *h3;
	*h0 = rot64(*h0, 5);	*h0 += *h1;  *h2 ^= *h0;
	*h1 = rot64(*h1, 36);	*h1 += *h2;  *h3 ^= *h1;
}

static inline void short_end
(
	uint64_t *h0,
	uint64_t *h1,
	uint64_t *h2,
	uint64_t *h3
)
{
	*h3 ^= *h2;  *h2 = rot64(*h2, 15);  *h3 += *h2;
	*h0 ^= *h3;  *h3 = rot64(*h3, 52);  *h0 += *h3;
	*h1 ^= *h0;  *h0 = rot64(*h0, 26);  *h1 += *h0;
	*h2 ^= *h1;  *h1 = rot64(*h1, 51);  *h2 += *h1;
	*h3 ^= *h2;  *h2 = rot64(*h2, 28);  *h3 += *h2;
	*h0 ^= *h3;  *h3 = rot64(*h3, 9);   *h0 += *h3;
	*h1 ^= *h0;  *h0 = rot64(*h0, 47);  *h1 += *h0;
	*h2 ^= *h1;  *h1 = rot64(*h1, 54);  *h2 += *h1;
	*h3 ^= *h2;  *h2 = rot64(*h2, 32);  *h3 += *h2;
	*h0 ^= *h3;  *h3 = rot64(*h3, 25);  *h0 += *h3;
	*h1 ^= *h0;  *h0 = rot64(*h0, 63);  *h1 += *h0;
}

static inline uint64_t spooky_shorthash ( const void *message, size_t length, uint64_t seed)
{
#if !ALLOW_UNALIGNED_READS
	uint64_t buf[4];
#endif
	union
	{
		const uint8_t *p8;
		uint32_t *p32;
		uint64_t *p64;
		size_t i;
	} u;
	size_t remainder;
	uint64_t a, b, c, d;
	u.p8 = (const uint8_t *)message;

	remainder = length % 32;
	a = seed;
	b = length;
	c = SC_CONST;
	d = SC_CONST;

	if (length <= 15)
	{
#if !ALLOW_UNALIGNED_READS
		if (u.i & 0x7) {
			memcpy(buf, u.p64, length);
			u.p64 = buf;
		}
#endif
	} else {
		const uint64_t *endp = u.p64 + (length/32)*4;
#if !ALLOW_UNALIGNED_READS
		if (u.i & 0x7) {
			const uint64_t *nowp = u.p64;
			u.p64 = buf;

			// handle all complete sets of 32 bytes
			for (; nowp < endp; nowp += 4)
			{
				memcpy(buf, nowp, 32);
				c += u.p64[0];
				d += u.p64[1];
				short_mix(&a, &b, &c, &d);
				a += u.p64[2];
				b += u.p64[3];
			}
			memcpy(buf, nowp, remainder);
		} else  {
#endif
			// handle all complete sets of 32 bytes
			for (; u.p64 < endp; u.p64 += 4)
			{
				c += u.p64[0];
				d += u.p64[1];
				short_mix(&a, &b, &c, &d);
				a += u.p64[2];
				b += u.p64[3];
			}
#if !ALLOW_UNALIGNED_READS
		}
#endif

		// Handle the case of 16+ remaining bytes.
		if (remainder >= 16)
		{
			c += u.p64[0];
			d += u.p64[1];
			short_mix(&a, &b, &c, &d);
			u.p64 += 2;
			remainder -= 16;
		}
	}

	// Handle the last 0..15 bytes, and its length
	d = ((uint64_t)length) << 56;
	switch (remainder)
	{
		case 15: d += ((uint64_t)u.p8[14]) << 48;
		case 14: d += ((uint64_t)u.p8[13]) << 40;
		case 13: d += ((uint64_t)u.p8[12]) << 32;
		case 12: d += u.p32[2];
			c += u.p64[0];
			break;
		case 11: d += ((uint64_t)u.p8[10]) << 16;
		case 10: d += ((uint64_t)u.p8[9]) << 8;
		case 9: d += (uint64_t)u.p8[8];
		case 8: c += u.p64[0];
			break;
		case 7: c += ((uint64_t)u.p8[6]) << 48;
		case 6: c += ((uint64_t)u.p8[5]) << 40;
		case 5: c += ((uint64_t)u.p8[4]) << 32;
		case 4: c += u.p32[0];
			break;
		case 3: c += ((uint64_t)u.p8[2]) << 16;
		case 2: c += ((uint64_t)u.p8[1]) << 8;
		case 1: c += (uint64_t)u.p8[0];
			break;
		case 0:
			c += SC_CONST;
			d += SC_CONST;
	}
	short_end(&a, &b, &c, &d);
	return a;
}
