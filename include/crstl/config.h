#pragma once

#if defined(_MSVC_LANG)

	#define crstl_cppversion _MSVC_LANG

#elif defined(__cplusplus)

	#define crstl_cppversion __cplusplus

#endif

#if defined(__clang__)

	#define CRSTL_CLANG

#elif defined(__GNUG__)

	#define CRSTL_GCC

#elif defined(_MSC_VER)

	#define CRSTL_MSVC

#endif

#if defined(__clang__) || defined(__GNUG__)

	#define crstl_finline inline __attribute__((always_inline))

#elif defined(_MSC_VER)

	#define crstl_finline __forceinline

#endif

#if defined(__cpp_if_constexpr)

	#define crstl_constexpr constexpr

#else

	#define crstl_constexpr

#endif

#if defined(_MSC_VER) && _MSC_VER < 1900

	#define crstl_noexcept

#else

	#define crstl_noexcept noexcept

#endif

#if crstl_cppversion >= 201702L
	#define crstl_nodiscard [[nodiscard]]
#else
	#define crstl_nodiscard
#endif

#if defined(__cpp_if_constexpr)

	#define crstl_constexpr_if(x) if constexpr(x)

#else

	#if defined(_MSC_VER)
		
		// warning C4127: conditional expression is constant
		// Disable because we always use these in a template context
		// Builds that don't support constexpr optimize them away
		#define crstl_constexpr_if(x) \
		__pragma(warning(push)) \
		__pragma(warning(disable : 4127)) \
		if(x) \
		__pragma(warning(pop))

	#else

		#define crstl_constexpr_if(x) if(x)

	#endif

#endif

#if crstl_cppversion >= 201402L
	#define CRSTL_VARIADIC_TEMPLATES
#endif

#define CRSTL_ENABLE_ASSERT

#if defined(CRSTL_ENABLE_ASSERT)

inline void crstl_assert_impl(bool condition)
{
	if (!condition)
	{
		int* p = (int*)(int)(0xDEADBEEF);
		*p = 0;
	}
}

#define crstl_assert(x) crstl_assert_impl((x))

#else

#define crstl_assert(x)

#endif

#if defined(_M_ARM) || defined(__arm__) || defined(_M_ARM64) || defined(__aarch64__)

	#define CRSTL_ARM

#elif defined(_WIN32)

	#define CRSTL_X86_64

#endif

#if defined(_WIN32)

	#define CRSTL_WINDOWS

#elif defined(__linux__)

	#define CRSTL_LINUX

#endif