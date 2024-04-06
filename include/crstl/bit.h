#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#if defined(CRSTL_COMPILER_MSVC)

#include "crstl/platform/common_win32.h"

#if defined(CRSTL_ARCH_ARM64)
	#include <arm64_neon.h>
#elif defined(CRSTL_ARCH_ARM32)
	#include <arm_neon.h>
#endif

extern "C"
{
	unsigned char _BitScanForward(unsigned long* Index, unsigned long Mask);
	unsigned char _BitScanForward64(unsigned long* Index, unsigned __int64 Mask);
	unsigned char _BitScanReverse(unsigned long* Index, unsigned long Mask);
	unsigned char _BitScanReverse64(unsigned long* Index, unsigned __int64 Mask);

	unsigned char    _rotl8(unsigned char, unsigned char);
	unsigned short   _rotl16(unsigned short, unsigned char);
	unsigned int     _rotl(unsigned int, int);
	unsigned __int64 _rotl64(unsigned __int64, int);

	unsigned char    _rotr8(unsigned char, unsigned char);
	unsigned short   _rotr16(unsigned short, unsigned char);
	unsigned int     _rotr(unsigned int, int);
	unsigned __int64 _rotr64(unsigned __int64, int);

	unsigned short   _byteswap_ushort(unsigned short);
	unsigned long    _byteswap_ulong(unsigned long);
	unsigned __int64 _byteswap_uint64(unsigned __int64);

#if defined(CRSTL_ARCH_X86)
	unsigned short __popcnt16(unsigned short);
	unsigned int __popcnt(unsigned int);
	unsigned __int64 __popcnt64(unsigned __int64);
#endif
}

#endif

crstl_module_export namespace crstl
{
	template<typename T>
	crstl_constexpr int bitsize() { return sizeof(T) * 8; }
};

#if defined(CRSTL_COMPILER_MSVC)

	#define crstl_rotate_left8(x, s) _rotl8(x, s)
	#define crstl_rotate_left16(x, s) _rotl16(x, s)
	#define crstl_rotate_left32(x, s) _rotl(x, s)
	#define crstl_rotate_left64(x, s) _rotl64(x, s)

	#define crstl_rotate_right8(x, s) _rotr8(x, s)
	#define crstl_rotate_right16(x, s) _rotr16(x, s)
	#define crstl_rotate_right32(x, s) _rotr(x, s)
	#define crstl_rotate_right64(x, s) _rotr64(x, s)

	#define crstl_byteswap16(x) _byteswap_ushort(x)
	#define crstl_byteswap32(x) _byteswap_ulong(x)
	#define crstl_byteswap64(x) _byteswap_uint64(x)

#elif crstl_has_builtin(__builtin_rotateleft8) // Mostly the Clang compiler

	#define crstl_rotate_left8(x, s) __builtin_rotateleft8(x, s)
	#define crstl_rotate_left16(x, s) __builtin_rotateleft16(x, s)
	#define crstl_rotate_left32(x, s) __builtin_rotateleft32(x, s)
	#define crstl_rotate_left64(x, s) __builtin_rotateleft64(x, s)

	#define crstl_rotate_right8(x, s) __builtin_rotateright8(x, s)
	#define crstl_rotate_right16(x, s) __builtin_rotateright16(x, s)
	#define crstl_rotate_right32(x, s) __builtin_rotateright32(x, s)
	#define crstl_rotate_right64(x, s) __builtin_rotateright64(x, s)

	#define crstl_byteswap16(x) __builtin_bswap16(x)
	#define crstl_byteswap32(x) __builtin_bswap32(x)
	#define crstl_byteswap64(x) __builtin_bswap64(x)

#else

namespace crstl
{
	namespace detail
	{
		template<typename T>
		T rotl(const T x, int s)
		{
			crstl_constexpr const unsigned int digits = bitsize<T>();
			const unsigned int us = s;
			return (x << (us % digits)) | (x >> ((-us) % digits));
		}

		template<typename T>
		T rotr(const T x, int s)
		{
			crstl_constexpr const unsigned int digits = bitsize<T>();
			const unsigned us = s;
			return (x >> (us % digits)) | (x << ((-us) % digits));
		}
	};
};

	#define crstl_rotate_left8(x, s)  crstl::detail::rotl<uint8_t>(x, s)
	#define crstl_rotate_left16(x, s) crstl::detail::rotl<uint16_t>(x, s)
	#define crstl_rotate_left32(x, s) crstl::detail::rotl<uint32_t>(x, s)
	#define crstl_rotate_left64(x, s) crstl::detail::rotl<uint64_t>(x, s)

	#define crstl_rotate_right8(x, s)  crstl::detail::rotr<uint8_t>(x, s)
	#define crstl_rotate_right16(x, s) crstl::detail::rotr<uint16_t>(x, s)
	#define crstl_rotate_right32(x, s) crstl::detail::rotr<uint32_t>(x, s)
	#define crstl_rotate_right64(x, s) crstl::detail::rotr<uint64_t>(x, s)

	#define crstl_byteswap16(x) __builtin_bswap16(x)
	#define crstl_byteswap32(x) __builtin_bswap32(x)
	#define crstl_byteswap64(x) __builtin_bswap64(x)

#endif

crstl_module_export namespace crstl
{
#if defined(CRSTL_COMPILER_MSVC)

	namespace detail
	{
		template<typename T>
		unsigned long bitscan_forward(const T x)
		{
			unsigned long index;
			crstl_constexpr_if(bitsize<T>() <= 32)
			{
				_BitScanForward(&index, static_cast<DWORD>(x));
			}
			else
			{
				_BitScanForward64(&index, static_cast<DWORD64>(x));
			}

			return index;
		}

		template<typename T>
		unsigned long bitscan_reverse(const T x)
		{
			unsigned long index;
			crstl_constexpr_if(bitsize<T>() <= 32)
			{
				_BitScanReverse(&index, static_cast<DWORD>(x));
			}
			else
			{
				_BitScanReverse64(&index, static_cast<DWORD64>(x));
			}

			return index;
		}
	};
	

	template<typename T>
	crstl_nodiscard
	inline int firstbitlow(const T x)
	{
		unsigned long index = detail::bitscan_forward(x);
		return static_cast<int>(x != 0 ? index : -1);
	}

	template<typename T>
	crstl_nodiscard
	inline int firstbithigh(const T x)
	{
		unsigned long index = detail::bitscan_reverse(x);
		return static_cast<int>(x != 0 ? index : -1);
	}

	template<typename T>
	crstl_nodiscard
	inline int countr_zero(const T x)
	{
		crstl_constexpr const int digits = bitsize<T>();
		unsigned long index = detail::bitscan_forward(x);
		return static_cast<int>(x != 0 ? index : digits);
	}

	template<typename T>
	crstl_nodiscard
	inline int countl_zero(const T x)
	{
		crstl_constexpr const int digits = bitsize<T>();
		unsigned long index = detail::bitscan_reverse(x);
		return static_cast<int>(x != 0 ? (digits - 1 - index) : digits);
	}

	template<typename T>
	crstl_nodiscard
	inline int popcount(const T x)
	{
#if defined(CRSTL_ARCH_ARM)
		__n64 temp;
		temp.n64_u64[0] = x;
		return neon_addv8(neon_cnt(temp)).n8_i8[0];
#else
		crstl_constexpr const int digits = bitsize<T>();
		crstl_constexpr_if(digits <= 16)
		{
			return static_cast<int>(__popcnt16(static_cast<unsigned short>(x)));
		}
		else crstl_constexpr_if(digits <= 32)
		{
			return static_cast<int>(__popcnt(static_cast<unsigned int>(x)));
		}
		else
		{
			return static_cast<int>(__popcnt64(static_cast<unsigned __int64>(x)));
		}
#endif
	}

#else

	namespace detail
	{
		template<typename T>
		int ffs(const T x)
		{
			crstl_constexpr_if(sizeof(T) <= sizeof(int))
			{
				return __builtin_ffs(static_cast<unsigned int>(x));
			}
			else crstl_constexpr_if(sizeof(T) <= sizeof(long))
			{
				return __builtin_ffsl(static_cast<unsigned long>(x));
			}
			else
			{
				return __builtin_ffsll(static_cast<unsigned long long>(x));
			}
		}

		template<typename T>
		int ctz(const T x)
		{
			crstl_constexpr_if(sizeof(T) <= sizeof(unsigned int))
			{
				return __builtin_ctz(static_cast<unsigned int>(x));
			}
			else crstl_constexpr_if(sizeof(T) <= sizeof(unsigned long))
			{
				return __builtin_ctzl(static_cast<unsigned long>(x));
			}
			else
			{
				return __builtin_ctzll(static_cast<unsigned long long>(x));
			}
		}

		template<typename T>
		int clz(const T x)
		{
			crstl_constexpr_if(sizeof(T) <= sizeof(unsigned int))
			{
				return __builtin_clz(static_cast<unsigned int>(x)) - (bitsize<unsigned int>() - bitsize<T>());
			}
			else crstl_constexpr_if(sizeof(T) <= sizeof(unsigned long))
			{
				return __builtin_clzl(static_cast<unsigned long>(x));
			}
			else
			{
				return __builtin_clzll(static_cast<unsigned long long>(x));
			}
		}

		template<typename T>
		int popcount(const T x)
		{
			crstl_constexpr_if(sizeof(T) <= sizeof(unsigned int))
			{
				return __builtin_popcount(static_cast<unsigned int>(x));
			}
			else crstl_constexpr_if(sizeof(T) <= sizeof(unsigned long))
			{
				return __builtin_popcountl(static_cast<unsigned long>(x));
			}
			else
			{
				return __builtin_popcountll(static_cast<unsigned long long>(x));
			}
		}
	};

	template<typename T>
	inline int firstbitlow(const T x)
	{
		return x != 0 ? detail::ctz(x) : -1;
	}

	template<typename T>
	inline int firstbithigh(const T x)
	{
		return x != 0 ? (bitsize<T>() - 1) - detail::clz(x) : -1;
	}

	template<typename T>
	inline int countr_zero(const T x)
	{
		return x != 0 ? detail::ctz(x) : bitsize<T>();
	}

	template<typename T>
	inline int countl_zero(const T x)
	{
		return x != 0 ? detail::clz(x) : bitsize<T>();
	}

	template<typename T>
	inline int popcount(const T x)
	{
		return detail::popcount(x);
	}

#endif

	template<typename ToT, typename FromT>
	union cast_union
	{
		const FromT* from;
		ToT* to;
	};

	template<typename ToT, typename FromT>
	crstl_nodiscard
	crstl_constexpr14 inline ToT bitcast(const FromT& x)
	{
		static_assert(sizeof(ToT) == sizeof(FromT), "Size must be equal");

#if CRSTL_CPPVERSION >= CRSTL_CPP20
		return __builtin_bit_cast(ToT, x);
#else
		cast_union<ToT, FromT> cu;
		cu.from = &x;
		return *cu.to;
#endif
	}

	template<typename T>
	crstl_nodiscard
	inline int countr_one(const T x)
	{
		return crstl::countr_zero(~x);
	}

	template<typename T>
	crstl_nodiscard
	inline int countl_one(const T x)
	{
		return crstl::countl_zero(~x);
	}

	template<typename T>
	crstl_nodiscard
	inline T rotl(const T x, int s)
	{
		crstl_constexpr const int digits = bitsize<T>();

		crstl_constexpr_if(digits == 8)
		{
			return (T)crstl_rotate_left8(static_cast<uint8_t>(x), static_cast<uint8_t>(s));
		}
		else crstl_constexpr_if(digits == 16)
		{
			return (T)crstl_rotate_left16(static_cast<uint16_t>(x), static_cast<uint8_t>(s));
		}
		else crstl_constexpr_if(digits == 32)
		{
			return (T)crstl_rotate_left32(static_cast<uint32_t>(x), static_cast<uint32_t>(s));
		}
		else
		{
			return (T)crstl_rotate_left64(static_cast<uint64_t>(x), static_cast<uint32_t>(s));
		}
	}

	template<typename T>
	crstl_nodiscard
	inline T rotr(const T x, int s)
	{
		crstl_constexpr const int digits = bitsize<T>();

		crstl_constexpr_if(digits == 8)
		{
			return (T)crstl_rotate_right8(static_cast<uint8_t>(x), static_cast<uint8_t>(s));
		}
		else crstl_constexpr_if(digits == 16)
		{
			return (T)crstl_rotate_right16(static_cast<uint16_t>(x), static_cast<uint8_t>(s));
		}
		else crstl_constexpr_if(digits == 32)
		{
			return (T)crstl_rotate_right32(static_cast<uint32_t>(x), static_cast<uint32_t>(s));
		}
		else
		{
			return (T)crstl_rotate_right64(static_cast<uint64_t>(x), static_cast<uint32_t>(s));
		}
	}

	template<typename T>
	crstl_nodiscard
	inline T byteswap(const T x)
	{
		crstl_constexpr const int digits = bitsize<T>();

		crstl_constexpr_if(digits == 8)
		{
			return x;
		}
		else crstl_constexpr_if(digits == 16)
		{
			return (T)crstl_byteswap16(static_cast<uint16_t>(x));
		}
		else crstl_constexpr_if(digits == 32)
		{
			return (T)crstl_byteswap32(static_cast<uint32_t>(x));
		}
		else
		{
			return (T)crstl_byteswap64(static_cast<uint64_t>(x));
		}
	}

	// Functionally equivalent to has_single_bit but it's much clearer what we want to know
	template<typename T>
	crstl_nodiscard
	crstl_constexpr inline bool is_pow2(const T x)
	{
		return x != 0 && (x & (x - 1)) == 0;
	}

	template<typename T>
	crstl_nodiscard
	crstl_constexpr inline bool has_single_bit(const T x)
	{
		return is_pow2(x);
	}

	template<typename T>
	crstl_nodiscard
	crstl_constexpr14 inline T bit_ceil(const T x) crstl_noexcept
	{
		if (x <= 1U)
		{
			return T(1);
		}

		const auto shift = crstl::bitsize<T>() - crstl::countl_zero(static_cast<T>(x - 1));
		return static_cast<T>(T(1) << shift);
	}

	template <typename T>
	crstl_nodiscard
	crstl_constexpr14 T bit_floor(const T x) crstl_noexcept
	{
		if (x == 0)
		{
			return T(0);
		}

		const auto shift = crstl::bitsize<T>() - crstl::countl_zero(x) - 1;
		return static_cast<T>(T(1) << shift);
	}

	template <typename T>
	crstl_nodiscard
	crstl_constexpr T bit_width(const T x) crstl_noexcept
	{
		return static_cast<T>(bitsize<T>() - crstl::countl_zero(x));
	}
};