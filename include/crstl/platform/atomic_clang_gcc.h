#pragma once

// Storage

#define crstl_atomic_store8(target, value) __atomic_exchange_n((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_store16(target, value) __atomic_exchange_n((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_store32(target, value) __atomic_exchange_n((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_store64(target, value) __atomic_exchange_n((target), (value), __ATOMIC_SEQ_CST)

// Addition

#define crstl_atomic_add8(target, value) __atomic_fetch_add((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_add16(target, value) __atomic_fetch_add((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_add32(target, value) __atomic_fetch_add((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_add64(target, value) __atomic_fetch_add((target), (value), __ATOMIC_SEQ_CST)

// Subtraction

#define crstl_atomic_sub8(target, value) __atomic_fetch_sub((target), (value))

#define crstl_atomic_sub16(target, value) __atomic_fetch_sub((target), (value))

#define crstl_atomic_sub32(target, value) __atomic_fetch_sub((target), (value))

#define crstl_atomic_sub64(target, value) __atomic_fetch_sub((target), (value))

// And

#define crstl_atomic_and8(target, value) __atomic_fetch_and((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_and16(target, value) __atomic_fetch_and((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_and32(target, value) __atomic_fetch_and((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_and64(target, value) __atomic_fetch_and((target), (value), __ATOMIC_SEQ_CST)

// Or

#define crstl_atomic_or8(target, value) __atomic_fetch_or((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_or16(target, value) __atomic_fetch_or((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_or32(target, value) __atomic_fetch_or((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_or64(target, value) __atomic_fetch_or((target), (value), __ATOMIC_SEQ_CST)

// Xor

#define crstl_atomic_xor8(target, value) __atomic_fetch_xor((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_xor16(target, value) __atomic_fetch_xor((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_xor32(target, value) __atomic_fetch_xor((target), (value), __ATOMIC_SEQ_CST)

#define crstl_atomic_xor64(target, value) __atomic_fetch_xor((target), (value), __ATOMIC_SEQ_CST)
