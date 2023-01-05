#pragma once

#include "stdint.h"

#if defined(_WIN32)
#include "atomic_win32.h"
#endif

namespace crstl
{
	// atomic_store: store value to variable pointed by target, and return its previous value

	inline int8_t atomic_store(int8_t volatile* target, int8_t value) noexcept
	{
		return crstl_atomic_store8(target, value);
	}

	inline int16_t atomic_store(int16_t volatile* target, int16_t value) noexcept
	{
		return crstl_atomic_store16(target, value);
	}

	inline int32_t atomic_store(int32_t volatile* target, int32_t value) noexcept
	{
		return crstl_atomic_store32(target, value);
	}

	inline int64_t atomic_store(int64_t volatile* target, int64_t value) noexcept
	{
		return crstl_atomic_store64(target, value);
	}

	// atomic_add: add value to variable pointed by target, and return the previous value of target

	inline int8_t atomic_add(int8_t volatile* target, int8_t value) noexcept
	{
		return crstl_atomic_add8(target, value);
	}

	inline int16_t atomic_add(int16_t volatile* target, int16_t value) noexcept
	{
		return crstl_atomic_add16(target, value);
	}

	inline int32_t atomic_add(int32_t volatile* target, int32_t value) noexcept
	{
		return crstl_atomic_add32(target, value);
	}

	inline int64_t atomic_add(int64_t volatile* target, int64_t value) noexcept
	{
		return crstl_atomic_add64(target, value);
	}

	// atomic_and: AND value with variable pointed by target, and return the previous value of target

	inline int8_t atomic_and(int8_t volatile* target, int8_t value) noexcept
	{
		return crstl_atomic_and8(target, value);
	}

	inline int16_t atomic_and(int16_t volatile* target, int16_t value) noexcept
	{
		return crstl_atomic_and16(target, value);
	}

	inline int32_t atomic_and(int32_t volatile* target, int32_t value) noexcept
	{
		return crstl_atomic_and32(target, value);
	}

	inline int64_t atomic_and(int64_t volatile* target, int64_t value) noexcept
	{
		return crstl_atomic_and64(target, value);
	}

	// atomic_or: OR value with variable pointed by target, and return the previous value of target

	inline int8_t atomic_or(int8_t volatile* target, int8_t value) noexcept
	{
		return crstl_atomic_or8(target, value);
	}

	inline int16_t atomic_or(int16_t volatile* target, int16_t value) noexcept
	{
		return crstl_atomic_or16(target, value);
	}

	inline int32_t atomic_or(int32_t volatile* target, int32_t value) noexcept
	{
		return crstl_atomic_or32(target, value);
	}

	inline int64_t atomic_or(int64_t volatile* target, int64_t value) noexcept
	{
		return crstl_atomic_or64(target, value);
	}

	// atomic_xor: XOR value with variable pointed by target, and return the previous value of target

	inline int8_t atomic_xor(int8_t volatile* target, int8_t value) noexcept
	{
		return crstl_atomic_xor8(target, value);
	}

	inline int16_t atomic_xor(int16_t volatile* target, int16_t value) noexcept
	{
		return crstl_atomic_xor16(target, value);
	}

	inline int32_t atomic_xor(int32_t volatile* target, int32_t value) noexcept
	{
		return crstl_atomic_xor32(target, value);
	}

	inline int64_t atomic_xor(int64_t volatile* target, int64_t value) noexcept
	{
		return crstl_atomic_xor64(target, value);
	}

	template<size_t N> struct atomic_type {};

	template<> struct atomic_type<1> { using type = int8_t; };
	template<> struct atomic_type<2> { using type = int16_t; };
	template<> struct atomic_type<4> { using type = int32_t; };
	template<> struct atomic_type<8> { using type = int64_t; };

	template<typename storage_type>
	class atomic
	{
	public:

		using operation_type = typename atomic_type<sizeof(storage_type)>::type;

		atomic() : m_value(0) {}

		atomic(const storage_type& value)
		{
			atomic_store((operation_type*)&m_value, value);
		}

		atomic& operator += (const storage_type& value)
		{
			atomic_add((operation_type*)&m_value, value); return *this;
		}

		atomic& operator -= (const storage_type& value)
		{
			atomic_add((operation_type*)&m_value, -(operation_type)value); return *this;
		}

		atomic& operator &= (const storage_type& value)
		{
			atomic_and((operation_type*)&m_value, value); return *this;
		}

		atomic& operator |= (const storage_type& value)
		{
			atomic_or((operation_type*)&m_value, value); return *this;
		}

		atomic& operator ^= (const storage_type& value)
		{
			atomic_xor((operation_type*)& m_value, value); return *this;
		}

		// Pre-increment
		storage_type operator ++ ()
		{
			return atomic_add((operation_type*)&m_value, 1) + 1;
		}

		// Post-increment
		storage_type operator ++ (int)
		{
			return atomic_add((operation_type*)&m_value, 1);
		}

		// Pre-decrement
		storage_type operator -- ()
		{
			return atomic_add((operation_type*)&m_value, -1) - 1;
		}

		// Post-derement
		storage_type operator -- (int)
		{
			return atomic_add((operation_type*)&m_value, -1);
		}

		operator storage_type() const
		{
			return m_value;
		}

	protected:

		storage_type m_value;
	};
};