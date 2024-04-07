#pragma once

#include "crstl/config.h"

#include "crstl/utility/placement_new.h"

#include "crstl/move_forward.h"

#include "crstl/utility/memory_ops.h"

namespace crstl
{
	template<typename T, typename VectorStorage>
	class vector_base : public VectorStorage
	{
	public:

		typedef VectorStorage base_type;
		typedef vector_base   this_type;
		typedef T*            iterator;
		typedef const T*      const_iterator;
		typedef T*            pointer;
		typedef const T*      const_pointer;
		typedef T&            reference;
		typedef const T&      const_reference;

		typedef typename base_type::length_type length_type;

		using base_type::m_length;
		using base_type::m_data;

		using base_type::get_capacity;
		using base_type::reallocate_if_length_equals_capacity;
		using base_type::reallocate_if_length_greater_than_capacity;

		crstl_constexpr14 T& at(size_t i) { return crstl_assert(i < m_length), m_data[i]; }
		crstl_constexpr const T& at(size_t i) const { return crstl_assert(i < m_length), m_data[i]; }

		crstl_constexpr14 T& back() { return crstl_assert(m_length > 0), m_data[m_length - 1]; }
		crstl_constexpr const T& back() const { return crstl_assert(m_length > 0), m_data[m_length - 1]; }

		crstl_constexpr14 iterator begin() { return &m_data[0]; }
		crstl_constexpr const_iterator begin() const { return &m_data[0]; }
		crstl_constexpr const_iterator cbegin() const { return &m_data[0]; }

		crstl_constexpr size_t capacity() const { return get_capacity(); }

		crstl_constexpr14 void clear()
		{
			destruct_or_ignore(m_data, m_length);
			m_length = 0;
		}

		crstl_constexpr14 pointer data() { return &m_data[0]; }
		crstl_constexpr const_pointer data() const { return &m_data[0]; }

#if defined(CRSTL_VARIADIC_TEMPLATES)
		template<typename... Args>
		crstl_constexpr14 T& emplace_back(Args&&... args)
		{
			reallocate_if_length_equals_capacity();

			crstl_assert(m_length < get_capacity());
			crstl_placement_new((void*)&m_data[m_length]) T(crstl_forward(Args, args)...);
			m_length++;
			return back();
		}
#endif

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_constexpr14 iterator end() { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator end() const { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator cend() const { return &m_data[0] + m_length; }

		crstl_constexpr14 T& front() { return m_data[0]; }
		crstl_constexpr const T& front() const { return m_data[0]; }

		crstl_constexpr14 void pop_back()
		{
			crstl_assert(m_length > 0);
			destruct_or_ignore(back());
			m_length--;
		}

		//----------
		// push_back
		//----------

		crstl_constexpr14 T& push_back()
		{
			reallocate_if_length_equals_capacity();
			default_initialize_or_memset_zero(m_data[m_length]);
			m_length++;
			return back();
		}

		crstl_constexpr14 void push_back(const T& v)
		{
			reallocate_if_length_equals_capacity();
			set_initialize_or_memset(m_data[m_length], v);
			m_length++;
		}

		crstl_constexpr14 void push_back(T&& v)
		{
			reallocate_if_length_equals_capacity();
			crstl_placement_new((void*)&m_data[m_length]) T(crstl_move(v));
			m_length++;
		}

		crstl_constexpr14 T& push_back_uninitialized()
		{
			reallocate_if_length_equals_capacity();
			m_length++;
			return back();
		}

		crstl_constexpr14 void reserve(size_t capacity)
		{
			reallocate_if_length_greater_than_capacity(capacity);
		}

		//-------
		// resize
		//-------

		crstl_constexpr14 void resize(size_t length)
		{
			if ((size_t)m_length < length)
			{
				reallocate_if_length_greater_than_capacity(length);

				default_initialize_or_memset_zero(m_data, length);
			}
			else if ((size_t)m_length > length)
			{
				destruct_or_ignore(&m_data[length], m_length - length);
			}

			m_length = (length_type)length;
		}

		crstl_constexpr14 void resize(size_t length, const T& value)
		{
			if ((size_t)m_length < length)
			{
				reallocate_if_length_greater_than_capacity(length);

				set_initialize_or_memset(&m_data[m_length], value, length - m_length);
			}
			else if ((size_t)m_length > length)
			{
				destruct_or_ignore(&m_data[length], m_length - length);
			}

			m_length = (length_type)length;
		}

		crstl_constexpr14 void resize_uninitialized(size_t length)
		{
			if ((size_t)m_length < length)
			{
				reallocate_if_length_greater_than_capacity(length);
			}
			else if ((size_t)m_length > length)
			{
				destruct_or_ignore(&m_data[length], m_length - length);
			}

			m_length = (length_type)length;
		}

		crstl_constexpr size_t size() const { return m_length; }

		crstl_constexpr14 T& operator [] (size_t i) { return crstl_assert(i < m_length), m_data[i]; }

		crstl_constexpr const T& operator [] (size_t i) const { return crstl_assert(i < m_length), m_data[i]; }

		//---------------------
		// Comparison Operators
		//---------------------

		crstl_constexpr14 bool operator == (const this_type& other) const crstl_noexcept
		{
			if (m_length == other.m_length)
			{
				for (size_t i = 0; i < m_length; ++i)
				{
					if (!(m_data[i] == other.m_data[i])) { return false; }
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		crstl_constexpr14 bool operator != (const this_type& other) crstl_noexcept
		{
			return !(*this == other);
		}
	};
};