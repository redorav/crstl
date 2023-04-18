#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/config_fwd.h"

#include "crstl/move_forward.h"

#include "crstl/type_builtins.h"

crstl_module_export namespace crstl
{
	template<typename T, size_t NumElements>
	struct fixed_deque_iterator
	{
		typedef fixed_deque_iterator<T, NumElements> this_type;
		typedef uint32_t                             length_type;
		typedef ptrdiff_t                            difference_type;

		static const size_t kLastElement = NumElements - 1;

		fixed_deque_iterator(T* data, length_type current, length_type begin, length_type end) : m_data(data), m_current(current), m_begin(begin), m_end(end) {}

		T* operator -> () const { return &m_data[m_current]; }
		T& operator * () const { return m_data[m_current]; }

		this_type& operator ++ () { increment(); return *this; }
		this_type operator ++ (int) { this_type temp(*this); increment(); return temp; }

		this_type& operator -- () { decrement(); return *this; }
		this_type operator -- (int) { this_type temp(*this); decrement(); return temp; }

		this_type& operator += (difference_type n) { m_current += n; m_current %= NumElements; }
		this_type& operator -= (difference_type n) { m_current -= n; m_current %= NumElements; }
		
		this_type operator + (difference_type n) const { return this_type(m_data, (m_current + n) % NumElements, m_begin, m_end); }
		this_type operator - (difference_type n) const { return this_type(m_data, (m_current - n) % NumElements, m_begin, m_end); }

		bool operator == (const this_type& other) { return m_current == other.m_current; }
		bool operator != (const this_type& other) { return m_current != other.m_current; }

		void increment()
		{
			m_current = m_current + 1 > kLastElement ? 0 : m_current + 1;
		}

		void decrement()
		{
			m_current = m_current - 1 > kLastElement ? kLastElement : m_current - 1;
		}

	private:

		T* m_data;
		length_type m_current;
		length_type m_begin;
		length_type m_end;
	};

	template<typename T, size_t NumElements>
	class fixed_deque
	{
	public:

		typedef fixed_deque<T, NumElements>                this_type;
		typedef T&                                         reference;
		typedef const T&                                   const_reference;
		typedef T*                                         pointer;
		typedef const T*                                   const_pointer;
		typedef fixed_deque_iterator<T, NumElements>       iterator;
		typedef const fixed_deque_iterator<T, NumElements> const_iterator;
		typedef uint32_t                                   length_type;

		static const size_t kLastElement = NumElements - 1;
		
		crstl_constexpr fixed_deque() crstl_noexcept : m_length(0), m_begin(0), m_end(0) {}
		
		crstl_constexpr fixed_deque(size_t initialLength) crstl_noexcept : m_length(0), m_begin(m_data), m_end(m_data)
		{
			crstl_assert(initialLength < NumElements);
			for (size_t i = 0; i < initialLength; ++i)
			{
				push_back();
			}
		}

		crstl_constexpr fixed_deque(const fixed_deque& other) crstl_noexcept
		{
			*this = other;
		}

		~fixed_deque() crstl_noexcept
		{
			clear();
		}

		this_type& operator = (const this_type& other) crstl_noexcept
		{
			clear();

			if (other.m_end >= other.m_begin)
			{
				crstl_constexpr_if(crstl_is_trivially_copyable(T))
				{
					memcpy(&m_data[other.m_begin], &other.m_data[other.m_begin], other.m_length * sizeof(T));
				}
				else
				{
					for (size_t i = other.m_begin; i < other.m_length; ++i)
					{
						::new((void*)&m_data[i]) T(other.m_data[i]);
					}
				}
			}
			else
			{
				crstl_constexpr_if(crstl_is_trivially_copyable(T))
				{
					memcpy(&m_data[0], &other.m_data[0], other.m_end * sizeof(T));
					memcpy(&m_data[other.m_begin], &other.m_data[other.m_begin], (NumElements - other.m_begin) * sizeof(T));
				}
				else
				{
					for (size_t i = 0; i < other.m_end; ++i)
					{
						::new((void*)&m_data[i]) T(other.m_data[i]);
					}

					for (size_t i = other.m_begin; i < NumElements; ++i)
					{
						::new((void*)&m_data[i]) T(other.m_data[i]);
					}
				}
			}

			m_begin  = other.m_begin;
			m_end    = other.m_end;
			m_length = other.m_length;

			return *this;
		}

		crstl_constexpr reference at(size_t i) { return *this[i]; }
		crstl_constexpr const_reference at(size_t i) const { return *this[i]; }
		
		crstl_constexpr reference back() { crstl_assert(m_length > 0); return m_data[m_end - 1 < 0 ? kLastElement : m_end - 1]; }
		crstl_constexpr const_reference back() const { crstl_assert(m_length > 0); return m_data[m_end - 1 < 0 ? kLastElement : m_end - 1]; }

		crstl_constexpr iterator begin() { return iterator(m_data, m_begin, m_begin, m_end); }
		crstl_constexpr const_iterator begin() const { return iterator(m_data, m_begin, m_begin, m_end); }
		crstl_constexpr const_iterator cbegin() const { return iterator(m_data, m_begin, m_begin, m_end); }

		crstl_constexpr size_t capacity() const { return NumElements; }
		
		crstl_constexpr void clear()
		{
			if (m_begin <= m_end)
			{
				for (size_t i = m_begin; i != m_end; ++i)
				{
					m_data[i].~T();
				}
			}
			else
			{
				for (size_t i = 0; i != m_end; ++i)
				{
					m_data[i].~T();
				}
		
				for (size_t i = m_begin; i != NumElements; ++i)
				{
					m_data[i].~T();
				}
			}
		
			m_length = 0;
		}
		
	#if defined(CRSTL_VARIADIC_TEMPLATES)

		template<typename... Args>
		reference emplace_back(Args&&... args)
		{
			crstl_assert(m_length < NumElements);
			size_t current_end = m_end;
			::new((void*)&m_data[current_end]) T(crstl::forward<Args>(args)...);
			m_end = m_end + 1 > kLastElement ? 0 : m_end + 1;
			m_length++;
			return m_data[current_end];
		}

		template<typename... Args>
		reference emplace_front(Args&&... args)
		{
			crstl_assert(m_length < NumElements);
			m_begin = m_begin - 1 > kLastElement ? kLastElement : m_begin - 1;
			::new((void*)&m_data[m_begin]) T(crstl::forward<Args>(args)...);
			m_length++;
			return m_data[m_begin];
		}

	#endif
		
		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_constexpr iterator end() { return iterator(m_data, m_end, m_begin, m_end); }
		crstl_constexpr const_iterator end() const { return iterator(m_data, m_end, m_begin, m_end); }
		crstl_constexpr const_iterator cend() const { return iterator(m_data, m_end, m_begin, m_end); }
		
		crstl_constexpr reference front() { return m_data[m_begin]; }
		crstl_constexpr const_reference front() const { return m_data[m_begin]; }

		crstl_constexpr size_t max_size() const { return NumElements; }
		
		void pop_back()
		{
			crstl_assert(m_length > 0);
			m_end = m_end - 1 > kLastElement ? kLastElement : m_end - 1;
			m_data[m_end].~T();
			m_length--;
		}

		void pop_front()
		{
			crstl_assert(m_length > 0);
			m_data[m_begin].~T();
			m_begin = m_begin + 1 > kLastElement ? 0 : m_begin + 1;
			m_length--;
		}
		
		//----------
		// push_back
		//----------
		
		reference push_back()
		{
			crstl_assert(m_length < NumElements);
			length_type current_end = m_end;
			::new((void*)&m_data[current_end]) T();
			m_end = m_end + 1 > kLastElement ? 0 : m_end + 1;
			m_length++;
			return m_data[current_end];
		}
		
		reference push_back_uninitialized()
		{
			crstl_assert(m_length < NumElements);
			length_type current_end = m_end;
			m_end = m_end + 1 > kLastElement ? 0 : m_end + 1;
			m_length++;
			return m_data[current_end];
		}
		
		void push_back(const T& v)
		{
			crstl_assert(m_length < NumElements);
			::new((void*)&m_data[m_end]) T(v);
			m_end = m_end + 1 > kLastElement ? 0 : m_end + 1;
			m_length++;
		}
		
		void push_back(T&& v)
		{
			crstl_assert(m_length < NumElements);
			::new((void*)&m_data[m_end]) T(crstl::move(v));
			m_end = m_end + 1 > kLastElement ? 0 : m_end + 1;
			m_length++;
		}

		//-----------
		// push_front
		//-----------
		
		reference push_front()
		{
			crstl_assert(m_length < NumElements);
			m_begin = m_begin - 1 > kLastElement ? kLastElement : m_begin - 1;
			::new((void*)&m_data[m_begin]) T();
			m_length++;
			return m_data[m_begin];
		}
		
		reference push_front_uninitialized()
		{
			crstl_assert(m_length < NumElements);
			m_begin = m_begin - 1 > kLastElement ? kLastElement : m_begin - 1;
			m_length++;
			return m_data[m_begin];
		}
		
		void push_front(const T& v)
		{
			crstl_assert(m_length < NumElements);
			m_begin = m_begin - 1 > kLastElement ? kLastElement : m_begin - 1;
			::new((void*)&m_data[m_begin]) T(v);
			m_length++;
		}
		
		void push_front(T&& v)
		{
			crstl_assert(m_length < NumElements);
			m_begin = m_begin - 1 > kLastElement ? kLastElement : m_begin - 1;
			::new((void*)&m_data[m_begin]) T(crstl::move(v));
			m_length++;
		}

		//-------
		// resize
		//-------

		void resize(size_t length)
		{
			resize_function(length, [](T* data_ptr)
			{
				::new((void*)data_ptr) T();
			});
		}

		void resize(size_t length, const T& value)
		{
			resize_function(length, [&value](T* data_ptr)
			{
				::new((void*)data_ptr) T(value);
			});
		}

		size_t size() const { return m_length; }

		reference operator [] (size_t i)
		{
			crstl_assert(i < m_length);
			return m_data[m_begin + i < NumElements ? m_begin + i : m_begin + i - NumElements];
		}
		
		const_reference operator [] (size_t i) const
		{
			crstl_assert(i < m_length);
			return m_data[m_begin + i < NumElements ? m_begin + i : m_begin + i - NumElements];
		}

		//---------------------
		// Comparison Operators
		//---------------------

		crstl_constexpr bool operator == (const this_type& other) crstl_noexcept
		{
			if (m_length == other.m_length)
			{
				if (other.m_end >= other.m_begin)
				{
					for (size_t i = other.m_begin; i < other.m_length; ++i)
					{
						if (!(m_data[i] == other.m_data[i])) { return false; }
					}
				}
				else
				{
					for (size_t i = 0; i < other.m_end; ++i)
					{
						if (!(m_data[i] == other.m_data[i])) { return false; }
					}

					for (size_t i = other.m_begin; i < NumElements; ++i)
					{
						if (!(m_data[i] == other.m_data[i])) { return false; }
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		crstl_constexpr bool operator != (const this_type& other) crstl_noexcept
		{
			return !(*this == other);
		}

	private:

		template<typename Function>
		void resize_function(size_t length, const Function& createFunction)
		{
			crstl_assert(length <= NumElements);

			if ((size_t)m_length < length)
			{
				size_t end = 0;

				if (m_end <= m_begin)
				{
					end = m_end + length;

					for (size_t i = m_end; i != end; ++i)
					{
						createFunction(&m_data[i]);
					}
				}
				else
				{
					size_t remaining_length = NumElements - m_end;

					for (size_t i = m_end; i != m_end + remaining_length; ++i)
					{
						createFunction(&m_data[i]);
					}

					end = length - remaining_length;

					for (size_t i = 0; i != end; ++i)
					{
						createFunction(&m_data[i]);
					}
				}

				m_end = (length_type)end;
			}
			else if ((size_t)m_length > length)
			{
				size_t delete_length = m_length - length;

				size_t begin = 0;

				if (m_end >= m_begin)
				{
					begin = m_end - delete_length;

					for (size_t i = begin; i != m_end; ++i)
					{
						m_data[i].~T();
					}
				}
				else
				{
					for (size_t i = 0; i != m_end; ++i)
					{
						m_data[i].~T();
					}

					begin = NumElements - (delete_length - m_end);

					for (size_t i = begin; i != NumElements; ++i)
					{
						m_data[i].~T();
					}
				}

				m_end = (length_type)begin;
			}

			m_length = (length_type)length;
		}

		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { T m_data[NumElements ? NumElements : 1]; };
		};
		crstl_warning_anonymous_struct_union_end

		length_type m_length;

		length_type m_begin;

		length_type m_end;
	};
};