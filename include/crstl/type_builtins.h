#pragma once

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

// Whether type is trivially copyable, i.e. whether we can use mempcy directly
// to copy an object or stream it in
#define crstl_is_trivially_copyable(T) __is_trivially_copyable(T)

// Whether type is signed
#define crstl_is_signed(T) (T(-1) < T(0))

// Whether type is unsigned
#define crstl_is_unsigned(T) (T(-1) > T(0))