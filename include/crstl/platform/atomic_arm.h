#pragma once

// Storage

#define crstl_atomic_store8(target, value) __sync_lock_test_and_set((char*)(target), (value))

#define crstl_atomic_store16(target, value) __sync_lock_test_and_set((short*)(target), (value))

#define crstl_atomic_store32(target, value) __sync_lock_test_and_set((long*)(target), (value))

#define crstl_atomic_store64(target, value) __sync_lock_test_and_set((long long*)(target), (value))

// Addition

#define crstl_atomic_add8(target, value) __sync_fetch_and_add((char*)(target), (value))

#define crstl_atomic_add16(target, value) __sync_fetch_and_add((short*)(target), (value))

#define crstl_atomic_add32(target, value) __sync_fetch_and_add((long*)(target), (value))

#define crstl_atomic_add64(target, value) __sync_fetch_and_add((long long*)(target), (value))

// And

#define crstl_atomic_and8(target, value) __sync_fetch_and_and((char*)(target), (value))

#define crstl_atomic_and16(target, value) __sync_fetch_and_and((short*)(target), (value))

#define crstl_atomic_and32(target, value) __sync_fetch_and_and((long*)(target), (value))

#define crstl_atomic_and64(target, value) __sync_fetch_and_and((long long*)(target), (value))

// Or

#define crstl_atomic_or8(target, value) __sync_fetch_and_or((char*)(target), (value))

#define crstl_atomic_or16(target, value) __sync_fetch_and_or((short*)(target), (value))

#define crstl_atomic_or32(target, value) __sync_fetch_and_or((long*)(target), (value))

#define crstl_atomic_or64(target, value) __sync_fetch_and_or((long long*)(target), (value))

// Xor

#define crstl_atomic_xor8(target, value) __sync_fetch_and_xor((char*)(target), (value))

#define crstl_atomic_xor16(target, value) __sync_fetch_and_xor((short*)(target), (value))

#define crstl_atomic_xor32(target, value) __sync_fetch_and_xor((long*)(target), (value))

#define crstl_atomic_xor64(target, value) __sync_fetch_and_xor((long long*)(target), (value))