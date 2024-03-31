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

// Whether type is a class
#define crstl_is_class(T) __is_class(T)

// Whether type is an enum
#define crstl_is_enum(T) __is_enum(T)

// Whether type has a pure virtual function
#define crstl_is_abstract(T) __is_abstract(T)

// Whether type declares or inherits at least one virtual function
#define crstl_is_polymorphic(T) __is_polymorphic(T)

// Whether Base is a base class of Derived
#define crstl_is_base_of(Base, Derived) __is_base_of(Base, Derived)

// Whether this object can be safely hashed, as it has no padding that could alter the result
#define crstl_has_unique_object_representations(T) __has_unique_object_representations(T)

// Whether class From can be implicitly converted to To
#if defined(CRSTL_COMPILER_MSVC)

	#define crstl_is_convertible(From, To) __is_convertible_to(From, To)

#else

	#define crstl_is_convertible(From, To) __is_convertible(From, To)

#endif

// Whether type is trivially constructible and destructible
#if crstl_has_builtin(__is_trivially_destructible) || (defined(CRSTL_COMPILER_MSVC) && CRSTL_MSVC_VERSION >= CRSTL_MSVC_2015)

	#define crstl_is_trivially_constructible(T) __is_trivially_constructible(T)
	#define crstl_is_trivially_destructible(T) __is_trivially_destructible(T)

#else

	#define crstl_is_trivially_constructible(T) __has_trivial_constructor(T)
	#define crstl_is_trivially_destructible(T) __has_trivial_destructor(T)

#endif

// Whether type is trivially copyable, i.e. whether we can use mempcy directly
// to copy an object or stream it in
#if defined(CRSTL_COMPILER_MSVC) && (CRSTL_MSVC_VERSION < CRSTL_MSVC_2015)

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