#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

namespace crstl
{
	template<typename T> struct hash;

	template <typename T> struct hash<T*> { size_t operator()(T* p) const { return static_cast<size_t>(uintptr_t(p)); } };

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