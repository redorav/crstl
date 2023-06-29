#pragma once

// C++ Version

#if defined(_MSVC_LANG)

	#define CRSTL_CPPVERSION _MSVC_LANG

#elif defined(__cplusplus)

	#define CRSTL_CPPVERSION __cplusplus

#endif

// Compiler

#if defined(__clang__)

	#define CRSTL_COMPILER_CLANG

#endif

#if defined(__GNUG__) || defined(__GNUC__)

	#define CRSTL_COMPILER_GCC

#endif

#if defined(_MSC_VER)

	#define CRSTL_COMPILER_MSVC
	#define CRSTL_MSVC_VERSION _MSC_VER
	#define CRSTL_MSVC_FULL_VERSION _MSC_FULL_VER

	#define CRSTL_MSVC_2015 1900

#endif

#if defined(__INTEL_COMPILER)

	#define CRSTL_COMPILER_INTEL

#endif

// Processor Architecture

#if defined(_M_ARM) || defined(__arm__) || defined(_M_ARM64) || defined(__aarch64__)

	#define CRSTL_ARM

#elif defined(_WIN32)

	#define CRSTL_X86_64

#endif

// Operating System

#if defined(_WIN32)

	#define CRSTL_WINDOWS

#elif defined(__linux__)

	#define CRSTL_LINUX

#elif defined(__APPLE__)

	#define CRSTL_OSX

#endif

#if defined(CRSTL_COMPILER_CLANG) || defined(CRSTL_COMPILER_GCC)

	#define crstl_finline inline __attribute__((always_inline))

#elif defined(CRSTL_COMPILER_MSVC)

	#define crstl_finline __forceinline

#endif

#define CRSTL_CPP11 201103L
#define CRSTL_CPP14 201402L
#define CRSTL_CPP17 201703L
#define CRSTL_CPP20 202002L

#if defined(__cpp_constexpr)

	#define crstl_constexpr constexpr
	
	// In C++11, functions marked as constexpr are also considered const. Because of that, a function marked constexpr
	// and a function marked const are considered to be the same thing. It's an unfortunate change of behavior that
	// causes compilation errors so we need to disambiguate. We should only mark non-const functions with this define
	#if CRSTL_CPPVERSION > CRSTL_CPP11

		#define crstl_constexpr14 constexpr

	#else

		#define crstl_constexpr14

	#endif

#else

	#define crstl_constexpr
	#define crstl_constexpr14

#endif

// __cpp_noexcept_function_type not very reliable
// noexcept first appeared in VS2015
#if defined(CRSTL_COMPILER_MSVC) && (CRSTL_MSVC_VERSION < CRSTL_MSVC_2015)

	#define crstl_noexcept

#else

	#define crstl_noexcept noexcept

#endif

#if CRSTL_CPPVERSION >= CRSTL_CPP20 || defined(__cpp_lib_char8_t)

#define CRSTL_CHAR8_TYPE

#endif

#if CRSTL_CPPVERSION >= CRSTL_CPP17

	#define crstl_nodiscard [[nodiscard]]

#else

	#define crstl_nodiscard

#endif

#if defined(__cpp_if_constexpr)

	#define crstl_constexpr_if(x) if constexpr(x)

#else

	#if defined(CRSTL_COMPILER_MSVC)
		
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

#if defined(CRSTL_COMPILER_MSVC)

	#define crstl_warning_anonymous_struct_union_begin \
	__pragma(warning(push)) \
	__pragma(warning(disable : 4201))

	#define crstl_warning_anonymous_struct_union_end __pragma(warning(pop))

#else

	#define crstl_warning_anonymous_struct_union_begin
	#define crstl_warning_anonymous_struct_union_end

#endif

#if CRSTL_CPPVERSION >= CRSTL_CPP11

	#define CRSTL_VARIADIC_TEMPLATES
	#define crstl_constructor_delete = delete
	#define crstl_constructor_default = default

#else

	#define crstl_constructor_delete
	#define crstl_constructor_default {}

#endif

#if defined(__cpp_initializer_lists)
	
	#define CRSTL_INITIALIZER_LISTS

#endif

#define CRSTL_ENABLE_ASSERT

#if defined(CRSTL_ENABLE_ASSERT)

inline void crstl_assert_impl(bool condition)
{
	if (!condition)
	{
		int* p = (int*)(int)(0xDEADC0DE);
		*p = 0;
	}
}

#define crstl_assert(x) crstl_assert_impl((x))

#else

#define crstl_assert(x)

#endif

#if defined(CRSTL_MODULE_DECLARATION)

	#define crstl_module_export export
	
#else

	#define crstl_module_export

#endif

#if defined(CRSTL_COMPILER_CLANG) || defined(CRSTL_COMPILER_GCC)

	#define crstl_likely(x) __builtin_expect(!!(x), 1)
	#define crstl_unlikely(x) __builtin_expect(!!(x), 0)

#else

	#define crstl_likely(x) (x)
	#define crstl_unlikely(x) (x)	

#endif

#define crstl_restrict __restrict

#define crstl_unused(x) (void)x