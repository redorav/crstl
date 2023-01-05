#pragma once

namespace crstl
{
	// We declare these to avoid including windows.h
	extern "C"
	{
		// Storage

		char _InterlockedExchange8(char volatile* target, char value);

		short _InterlockedExchange16(short volatile* target, short value);

		long _InterlockedExchange(long volatile* target, long value);

		long long _InterlockedExchange64(long long volatile* target, long long value);

		// Addition

		char _InterlockedExchangeAdd8(char volatile* target, char value);

		short _InterlockedExchangeAdd16(short volatile* target, short value);

		long _InterlockedExchangeAdd(long volatile* target, long value);

		long long _InterlockedExchangeAdd64(long long volatile* target, long long value);

		// And

		char _InterlockedAnd8(char volatile* target, char mask);

		short _InterlockedAnd16(short volatile* target, short mask);

		long _InterlockedAnd(long volatile* target, long mask);

		long long _InterlockedAnd64(long long volatile* target, long long mask);

		// Or

		char _InterlockedOr8(char volatile* target, char mask);

		short _InterlockedOr16(short volatile* target, short mask);

		long _InterlockedOr(long volatile* target, long mask);

		long long _InterlockedOr64(long long volatile* target, long long mask);

		// Xor

		char _InterlockedXor8(char volatile* target, char mask);

		short _InterlockedXor16(short volatile* target, short mask);

		long _InterlockedXor(long volatile* target, long mask);

		long long _InterlockedXor64(long long volatile* target, long long mask);

		// Compare and Exchange

		char _InterlockedCompareExchange8(char volatile* target, char exchange, char comparand);

		short _InterlockedCompareExchange16(short volatile* target, short exchange, short comparand);

		long _InterlockedCompareExchange(long volatile* target, long exchange, long comparand);

		long long _InterlockedCompareExchange64(long long volatile* target, long long exchange, long long comparand);
	}
};

// Storage

#define crstl_atomic_store8(target, value) _InterlockedExchange8((char*)(target), (value))

#define crstl_atomic_store16(target, value) _InterlockedExchange16((short*)(target), (value))

#define crstl_atomic_store32(target, value) _InterlockedExchange((long*)(target), (value))

#define crstl_atomic_store64(target, value) _InterlockedExchange64((long long*)(target), (value))

// Addition

#define crstl_atomic_add8(target, value) _InterlockedExchangeAdd8((char*)(target), (value))

#define crstl_atomic_add16(target, value) _InterlockedExchangeAdd16((short*)(target), (value))

#define crstl_atomic_add32(target, value) _InterlockedExchangeAdd((long*)(target), (value))

#define crstl_atomic_add64(target, value) _InterlockedExchangeAdd64((long long*)(target), (value))

// And

#define crstl_atomic_and8(target, value) _InterlockedAnd8((char*)(target), (value))

#define crstl_atomic_and16(target, value) _InterlockedAnd16((short*)(target), (value))

#define crstl_atomic_and32(target, value) _InterlockedAnd((long*)(target), (value))

#define crstl_atomic_and64(target, value) _InterlockedAnd64((long long*)(target), (value))

// Or

#define crstl_atomic_or8(target, value) _InterlockedOr8((char*)(target), (value))

#define crstl_atomic_or16(target, value) _InterlockedOr16((short*)(target), (value))

#define crstl_atomic_or32(target, value) _InterlockedOr((long*)(target), (value))

#define crstl_atomic_or64(target, value) _InterlockedOr64((long long*)(target), (value))

// Xor

#define crstl_atomic_xor8(target, value) _InterlockedXor8((char*)(target), (value))

#define crstl_atomic_xor16(target, value) _InterlockedXor16((short*)(target), (value))

#define crstl_atomic_xor32(target, value) _InterlockedXor((long*)(target), (value))

#define crstl_atomic_xor64(target, value) _InterlockedXor64((long long*)(target), (value))