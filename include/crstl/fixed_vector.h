#pragma once

#include "config.h"

#include "config_fwd.h"

#include "type_utils.h"

#include "crstldef.h"

#include <initializer_list>

// fixed_vector
//
// Fixed replacement for std::vector
//
// fixed_vector doesn't allocate memory, instead manages an internal array
// 
// - The number of elements is specified at compile time
// - resize() is only used to initialize or destroy objects. It is
//   an error to call it with a value > NumElements
// - 

crstl_module_export namespace crstl
{
	// We forward include all the uses of span, so we don't need to always include it
	template<typename T>
	class span;

	template<typename T, size_t NumElements>
	class fixed_vector
	{
	public:

		typedef fixed_vector<T, NumElements> this_type;
		typedef T&                           reference;
		typedef const T&                     const_reference;
		typedef T*                           pointer;
		typedef const T*                     const_pointer;
		typedef T*                           iterator;
		typedef const T*                     const_iterator;
		typedef uint32_t                     length_type;

		enum
		{
			kMaxStack = 4096,
		};

		// We need to define all standard operators because we've made the m_data member a union. The reason for it is to avoid
		// invoking the default constructor of T. We don't want to change it to some raw type like char as it becomes hard to
		// debug later on and would rely on a natvis to properly visualize it

		crstl_constexpr fixed_vector() crstl_noexcept : m_length(0) {}

		crstl_constexpr fixed_vector(size_t initialLength) crstl_noexcept : m_length(0)
		{
			crstl_assert(initialLength < NumElements);
			for (size_t i = 0; i < initialLength; ++i)
			{
				push_back();
			}
		}

		crstl_constexpr fixed_vector(const this_type& other) crstl_noexcept { *this = other; }

		crstl_constexpr fixed_vector(this_type&& other) crstl_noexcept { *this = other; }

		template<typename Iterator>
		crstl_constexpr fixed_vector(Iterator iter1, Iterator iter2) crstl_noexcept
		{
			crstl_assert(iter1 != nullptr && iter2 != nullptr);
			crstl_assert(iter2 >= iter1);

			size_t iter_length = iter2 - iter1;
			crstl_assert(iter_length < NumElements);

			for (size_t i = 0; i < iter_length; ++i)
			{
				::new((void*)&m_data[i]) T(iter1[i]);
			}

			m_length = (length_type)iter_length;
		}

		crstl_constexpr fixed_vector(std::initializer_list<T> ilist) crstl_noexcept : m_length(0)
		{
			crstl_assert((ilist.end() - ilist.begin()) <= NumElements);

			for (const T *ptr = ilist.begin(), *end = ilist.end(); ptr != end; ++ptr)
			{
				push_back(*ptr);
			}
		}

		~fixed_vector() crstl_noexcept
		{
			clear();
		}

		this_type& operator = (const this_type& other) crstl_noexcept
		{
			for (size_t i = 0; i < other.m_length; ++i)
			{
				::new((void*)&m_data[i]) T(other.m_data[i]);
			}

			m_length = other.m_length;
			return *this;
		}

		this_type& operator = (this_type&& other) crstl_noexcept
		{
			crstl::swap(m_length, other.m_length);
			crstl::swap(m_data, other.m_data);
			return *this;
		}

		reference at(size_t i) { crstl_assert(i < m_length); return m_data[i]; }
		const_reference at(size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		reference back() { crstl_assert(m_length > 0); return m_data[m_length - 1]; }
		const_reference back() const { crstl_assert(m_length > 0); return m_data[m_length - 1]; }

		iterator begin() { return &m_data[0]; }
		const_iterator begin() const { return &m_data[0]; }
		const_iterator cbegin() const { return &m_data[0]; }

		size_t capacity() const { return NumElements; }

		void clear()
		{
			for (size_t i = 0; i < m_length; ++i)
			{
				m_data[i].~T();
			}

			m_length = 0;
		}

		pointer data() { return &m_data[0]; }
		const_pointer data() const { return &m_data[0]; }

#if defined(CRSTL_VARIADIC_TEMPLATES)
		template<typename... Args>
		reference emplace_back(Args&&... args)
		{
			crstl_assert(m_length < NumElements);
			::new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> reference emplace_back(Arg0&& arg0)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> reference emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		reference emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			crstl_assert(m_length < NumElements); ::new((void*)&m_data[m_length]) 
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8), crstl::forward<Arg9>(arg9));
			m_length++; return back();
		}

#endif

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		iterator end() { return &m_data[0] + m_length; }
		const_iterator end() const { return &m_data[0] + m_length; }
		const_iterator cend() const { return &m_data[0] + m_length; }

		reference front() { m_data[0]; }
		const_reference front() const { m_data[0]; }

		void pop_back()
		{
			crstl_assert(m_length > 0);
			back().~T();
			m_length--;
		}

		//----------
		// push_back
		//----------

		reference push_back()
		{
			crstl_assert(m_length < NumElements);
			::new((void*)&m_data[m_length]) T();
			m_length++;
			return back();
		}

		reference push_back_uninitialized()
		{
			m_length++;
			return back();
		}

		void push_back(const T& v)
		{
			::new((void*)&m_data[m_length]) T(v);
			m_length++;
		}

		void push_back(T&& v)
		{
			::new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		//-------
		// resize
		//-------

		void resize(size_t length)
		{
			if ((size_t)m_length < length)
			{
				for (size_t i = m_length; i < length; ++i)
				{
					::new((void*)&m_data[i]) T();
				}
			}
			else if ((size_t)m_length > length)
			{
				for (size_t i = length; i < m_length; ++i)
				{
					m_data[i].~T();
				}
			}

			m_length = (length_type)length;
		}

		void resize(size_t length, const T& value)
		{
			if ((size_t)m_length < length)
			{
				for (size_t i = m_length; i < length; ++i)
				{
					::new((void*)&m_data[i]) T(value);
				}
			}
			else if ((size_t)m_length > length)
			{
				for (size_t i = length; i < m_length; ++i)
				{
					m_data[i].~T();
				}
			}

			m_length = (length_type)length;
		}

		size_t size() const { return m_length; }

		void swap(this_type& v)
		{
			// For small vectors, make a temporary copy directly on the stack, then copy it back
			crstl_constexpr_if(sizeof(this_type) <= kMaxStack)
			{
				this_type temp = v;
				v = *this;
				*this = temp;
			}
			else // Otherwise copy it in chunks, all on the stack
			{
				size_t chunks   = (sizeof(this_type) + kMaxStack - 1) / kMaxStack;
				uint8_t* vData    = reinterpret_cast<uint8_t*>(&v);
				uint8_t* thisData = reinterpret_cast<uint8_t*>(this);

				for (uint32_t c = 0; c < chunks - 1; ++c)
				{
					uint8_t chunkData[kMaxStack];
					size_t chunkOffset = c * kMaxStack;
					memcpy(chunkData, vData + chunkOffset, kMaxStack);
					memcpy(vData + chunkOffset, thisData + chunkOffset, kMaxStack);
					memcpy(thisData + chunkOffset, chunkData, kMaxStack);
				}

				static const size_t kChunkCount = (sizeof(this_type) + kMaxStack - 1) / kMaxStack;
				static const size_t kChunkRemaining = sizeof(this_type) - (kChunkCount - 1) * kMaxStack;

				// Copy last chunk
				uint8_t chunkData[kMaxStack];
				size_t chunkOffset = (chunks - 1) * kMaxStack;
				memcpy(chunkData, vData + chunkOffset, kChunkRemaining);
				memcpy(vData + chunkOffset, thisData + chunkOffset, kChunkRemaining);
				memcpy(thisData + chunkOffset, chunkData, kChunkRemaining);
			}
		}

		reference operator [] (size_t i) { crstl_assert(i < m_length); return m_data[i]; }

		const_reference operator [] (size_t i) const { crstl_assert(i < m_length); return m_data[i]; }

		operator span<T>() const;

	private:

		// The union is here to avoid calling the default constructor of T on construction of fixed_vector.
		// The alternative is to have a uint8_t buffer that we reinterpret through the interface, which is
		// hard to debug and would certainly need custom visualizers. This alternative works with any C++11
		// conforming compiler
		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { T m_data[NumElements ? NumElements : 1]; };
		};
		crstl_warning_anonymous_struct_union_end

		length_type m_length;
	};

	template<typename T, size_t NumElements>
	fixed_vector<T, NumElements>::operator span<T>() const
	{
		return span<T>((T*)m_data, (size_t)m_length);
	}
};