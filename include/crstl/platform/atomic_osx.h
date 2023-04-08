#pragma once

//#include <libkern/OSAtomic.h>

extern "C"
{
    char OSAddAtomic8(int amount, volatile char* address);
};

// Storage

#define crstl_atomic_store8(target, value) 0//OSAtomicCompareAndSwap32(value, target)

#define crstl_atomic_store16(target, value) 0//OSAtomicCompareAndSwap32(value, target)

#define crstl_atomic_store32(target, value) 0//OSAtomicCompareAndSwap32(value, (crstl::int32_t*)target)

#define crstl_atomic_store64(target, value) 0//OSAtomicCompareAndSwap64(value, target)

// Addition

#define crstl_atomic_add8(target, value) OSAddAtomic8(value, (volatile char*)target)

#define crstl_atomic_add16(target, value) 0//OSAtomicAdd32Barrier(value, target)

#define crstl_atomic_add32(target, value) 0//__c11_atomic_fetch_add(target, value, __ATOMIC_SEQ_CST)

#define crstl_atomic_add64(target, value) 0//OSAtomicAdd64Barrier(value, target)

// And

#define crstl_atomic_and8(target, value) 0//OSAtomicAnd32Barrier(value, target)

#define crstl_atomic_and16(target, value) 0//OSAtomicAnd32Barrier(value, target)

#define crstl_atomic_and32(target, value) 0//OSAtomicAnd32Barrier(value, target)

#define crstl_atomic_and64(target, value) 0//OSAtomicAnd64Barriers(value, target)

// Or

#define crstl_atomic_or8(target, value) 0//OSAtomicOr32Barrier(value, target)

#define crstl_atomic_or16(target, value) 0//OSAtomicOr32Barrier(value, target)

#define crstl_atomic_or32(target, value) 0//OSAtomicOr32Barrier(value, target)

#define crstl_atomic_or64(target, value) 0//OSAtomicOr64Barrier(value, target)

// Xor

#define crstl_atomic_xor8(target, value) 0//OSAtomicXor32Barrier(value, target)

#define crstl_atomic_xor16(target, value) 0//OSAtomicXor32Barrier(value, target)

#define crstl_atomic_xor32(target, value) 0//OSAtomicXor32Barrier(value, target)

#define crstl_atomic_xor64(target, value) 0//OSAtomicXor64Barrier(value, target)
