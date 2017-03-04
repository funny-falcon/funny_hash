#ifndef SIPHASH_IMPL_H
#define SIPHASH_IMPL_H

#include "siphash.h"

#if defined(_MSC_VER)
	#include <intrin.h>

	#define INLINE __forceinline
	#define NOINLINE __declspec(noinline)
	#define ROTL32(a,b) _rotl32(a,b)
	#define MM16 __declspec(align(16))

	typedef unsigned int uint32_t;
	
	#if (_MSC_VER >= 1500)
		#define __SSSE3__
	#endif
	#if (_MSC_VER > 1200) || defined(_mm_free)
		#define __SSE2__
	#endif
#else
	#define INLINE __attribute__((always_inline))
	#define NOINLINE __attribute__((noinline))
	#define ROTL32(a,b) (((a)<<(b))|((a)>>(32-b)))
	#define MM16 __attribute__((aligned(16)))
#endif

#endif // SIPHASH_IMPL_H

