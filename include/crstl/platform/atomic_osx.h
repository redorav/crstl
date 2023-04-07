#pragma once

// Storage

#define crstl_atomic_store8(target, value) OSAtomicCompareAndSwap32(value, target)

#define crstl_atomic_store16(target, value) OSAtomicCompareAndSwap32(value, target)

#define crstl_atomic_store32(target, value) OSAtomicCompareAndSwap32(value, target)

#define crstl_atomic_store64(target, value) OSAtomicCompareAndSwap64(value, target)

// Addition

#define crstl_atomic_add8(target, value) OSAtomicAdd32Barrier(value, target)

#define crstl_atomic_add16(target, value) OSAtomicAdd32Barrier(value, target)

#define crstl_atomic_add32(target, value) OSAtomicAdd32Barrier(value, target)

#define crstl_atomic_add64(target, value) OSAtomicAdd64Barrier(value, target)

// And

#define crstl_atomic_and8(target, value) OSAtomicAnd32Barrier(value, target)

#define crstl_atomic_and16(target, value) OSAtomicAnd32Barrier(value, target)

#define crstl_atomic_and32(target, value) OSAtomicAnd32Barrier(value, target)

#define crstl_atomic_and64(target, value) OSAtomicAnd64Barriers(value, target)

// Or

#define crstl_atomic_or8(target, value) OSAtomicOr32Barrier(value, target)

#define crstl_atomic_or16(target, value) OSAtomicOr32Barrier(value, target)

#define crstl_atomic_or32(target, value) OSAtomicOr32Barrier(value, target)

#define crstl_atomic_or64(target, value) OSAtomicOr64Barrier(value, target)

// Xor

#define crstl_atomic_xor8(target, value) OSAtomicXor32Barrier(value, target)

#define crstl_atomic_xor16(target, value) OSAtomicXor32Barrier(value, target)

#define crstl_atomic_xor32(target, value) OSAtomicXor32Barrier(value, target)

#define crstl_atomic_xor64(target, value) OSAtomicXor64Barrier(value, target)
