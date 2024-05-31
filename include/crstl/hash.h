#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

namespace crstl
{
	template<typename T> struct hash;

	// Pointers tend to be correlated by being aligned so they'll typically end up in the same buckets and cause many collisions.
	// This is a relatively fast way to mix them up a bit without being too expensive. Other approaches such as removing the bottom
	// bits are very cheap but cause many collisions, and better hashes like Wang or 
	// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
	template <typename T> struct hash<T*>
	{
		size_t operator()(T* ptr) const
		{
			uintptr_t uptr = (uintptr_t)ptr;

			crstl_constexpr_if(sizeof(T*) == 8)
			{
				uptr = (uptr ^ (uptr >> 30)) * 0xbf58476d1ce4e5b9;
				uptr = (uptr ^ (uptr >> 27)) * 0x94d049bb133111eb;
				uptr = uptr ^ (uptr >> 31);
			}
			else
			{
				uptr = ((uptr >> 16) ^ uptr) * 0x45d9f3b;
				uptr = ((uptr >> 16) ^ uptr) * 0x45d9f3b;
				uptr = (uptr >> 16) ^ uptr;
			}

			return static_cast<size_t>(uptr);
		}
	};

	template<> struct hash<bool> { size_t operator()(bool value) const { return static_cast<size_t>(value); } };

	template<> struct hash<char> { size_t operator()(char value) const { return static_cast<size_t>(value); } };

	template<> struct hash<unsigned char> { size_t operator()(unsigned char value) const { return static_cast<size_t>(value); } };

	template<> struct hash<short> { size_t operator()(short value) const { return static_cast<size_t>(value); } };

	template<> struct hash<unsigned short> { size_t operator()(unsigned short value) const { return static_cast<size_t>(value); } };

	template<> struct hash<int> { size_t operator()(int value) const { return static_cast<size_t>(value); } };

	template<> struct hash<unsigned int> { size_t operator()(unsigned int value) const { return static_cast<size_t>(value); } };

	template<> struct hash<long> { size_t operator()(long value) const { return static_cast<size_t>(value); } };

	template<> struct hash<unsigned long> { size_t operator()(unsigned long value) const { return static_cast<size_t>(value); } };

	template<> struct hash<long long> { size_t operator()(long long value) const { return static_cast<size_t>(value); } };

	template<> struct hash<unsigned long long> { size_t operator()(unsigned long long value) const { return static_cast<size_t>(value); } };

	template<> struct hash<float>
	{
		size_t operator()(float value)
		{
			union
			{
				float f;
				uint32_t u;
			} u;

			u.f = value;

			return static_cast<size_t>(u.u);
		}
	};

	template<> struct hash<double>
	{
		size_t operator()(double value)
		{
			union
			{
				double d;
				size_t u;
			} u;

			u.d = value;

			return static_cast<size_t>(u.u);
		}
	};
};