#pragma once

#include "crstl/config.h"

//----------------
// Type Properties
//----------------

// Whether type is empty, i.e. has no members. Mainly used for the
// empty base class optimization
#define crstl_is_empty(T) __is_empty(T)

// Whether type is a union
#define crstl_is_union(T) __is_union(T)

// Whether type is an enum
#define crstl_is_enum(T) __is_enum(T)

// Whether type has a pure virtual function
#define crstl_is_abstract(T) __is_abstract(T)

#define crstl_is_base_of(Base, Derived) __is_base_of(Base, Derived)

#define crstl_has_unique_object_representations(T) __has_unique_object_representations(T)

// Whether type is trivially constructible and destructible
#if (defined(CRSTL_MSVC) && (CRSTL_MSVC_VERSION < CRSTL_MSVC_2015)) || defined(CRSTL_GCC)

	#define crstl_is_trivially_constructible(T) __has_trivial_constructor(T)
	#define crstl_is_trivially_destructible(T) __has_trivial_destructor(T)

#else

	#define crstl_is_trivially_constructible(T) __is_trivially_constructible(T)
	#define crstl_is_trivially_destructible(T) __is_trivially_destructible(T)

#endif

// Whether type is trivially copyable, i.e. whether we can use mempcy directly
// to copy an object or stream it in
#if defined(CRSTL_MSVC) && (CRSTL_MSVC_VERSION < CRSTL_MSVC_2015)

	#define crstl_is_trivially_copyable(T) __has_trivial_copy(T)
	#define crstl_is_final(T) false

#else

	#define crstl_is_trivially_copyable(T) __is_trivially_copyable(T)
	#define crstl_is_final(T) __is_final(T)

#endif

// Whether type is signed
#define crstl_is_signed(T) (T(-1) < T(0))

// Whether type is unsigned
#define crstl_is_unsigned(T) (T(-1) > T(0))