#pragma once

// C++ Version

#if defined(_MSVC_LANG)

	#define CRSTL_CPPVERSION _MSVC_LANG

#elif defined(__cplusplus)

	#define CRSTL_CPPVERSION __cplusplus

#endif

// Compiler

// Clang can define the GNU flags but not the other way around, so we enforce this ordering
#if defined(__clang__)

	#define CRSTL_COMPILER_CLANG

#elif defined(__GNUG__) || defined(__GNUC__)

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
// https://stackoverflow.com/questions/152016/detecting-cpu-architecture-compile-time

#if defined(__aarch64__) || defined(_M_ARM64)

	#define CRSTL_ARCH_ARM64
	#define CRSTL_ARCH_ARM

#elif defined(__arm__) || defined(_M_ARM)

	#define CRSTL_ARCH_ARM32
	#define CRSTL_ARCH_ARM

#elif defined(__x86_64__) || defined(__amd64__) || defined(_M_X64)

	#define CRSTL_ARCH_X86_64
	#define CRSTL_ARCH_X86

#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)

	#define CRSTL_ARCH_X86_32
	#define CRSTL_ARCH_X86

#elif defined(__powerpc__)

	#define CRSTL_ARCH_POWERPC

#elif defined(__riscv)

	#define CRSTL_ARCH_RISCV

#endif

// Operating System

#if defined(_WIN32)

	#define CRSTL_OS_WINDOWS

#elif defined(ANDROID) || defined(__ANDROID__)

	#define CRSTL_OS_ANDROID

#elif defined(__linux__)

	#define CRSTL_OS_LINUX

#elif defined(__APPLE__)

	#define CRSTL_OS_OSX

#endif

#if defined(CRSTL_COMPILER_MSVC)

	#define crstl_finline __forceinline

#else

	#define crstl_finline inline __attribute__((always_inline))

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
	#if CRSTL_CPPVERSION >= CRSTL_CPP14

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
	#define CRSTL_INLINE_VARIABLES

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

// We make the macro a little convoluted to make sure it can run in constexpr scenarios
// For more information see https://akrzemi1.wordpress.com/2017/05/18/asserts-in-constexpr-functions/
#define crstl_assert(x) ((x) ? void(0) : [] { int* p = (int*)(int)(0xDEADC0DE); *p = 0; }())
#define crstl_assert_msg(x, msg) crstl_assert(x)

#else

#define crstl_assert(x) void(0)
#define crstl_assert_msg(x, msg) void(0)

#endif

#if defined(CRSTL_COMPILER_MSVC)

	#define crstl_assume(condition) __assume(condition)

#elif defined(CRSTL_COMPILER_CLANG)

	#define crstl_assume(condition) __builtin_assume(condition)

#elif defined(CRSTL_COMPILER_GCC)

	#define crstl_assume(condition) do { if (!(condition)) __builtin_unreachable(); } while (0)

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

#if defined(CRSTL_COMPILER_MSVC)

	#define crstl_forceinline __forceinline

#else

	#define crstl_forceinline inline __attribute__((always_inline))

#endif
