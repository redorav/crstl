#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

namespace crstl
{
	template<typename T> struct hash;

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

	template<> struct hash<float> { size_t operator()(float value) { size_t bits = *(size_t*)&value; return static_cast<size_t>(bits); } };

	template<> struct hash<double> { size_t operator()(double value) { size_t bits = *(size_t*)&value; return static_cast<size_t>(bits); } };
};