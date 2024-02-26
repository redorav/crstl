#pragma once

#include "crstl/config.h"

#include "crstl/config_fwd.h"

#include "crstl/move_forward.h"

#include "crstl/crstldef.h"

#include "crstl/type_builtins.h"

#include "crstl/utility/fixed_common.h"

#include "crstl/utility/memory_ops.h"

#if defined(CRSTL_MODULE_DECLARATION)
import <initializer_list>;
#elif defined(CRSTL_FEATURE_INITIALIZER_LISTS)
#include <initializer_list>
#endif

// crstl::fixed_vector
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
		typedef typename fixed_length_select_type<NumElements>::type length_type;

		enum
		{
			kMaxStack = 4096,
		};

		// We need to define all standard operators because we've made the m_data member a union. The reason for it is to avoid
		// invoking the default constructor of T. We don't want to change it to some raw type like char as it becomes hard to
		// debug later on and would rely on a natvis to properly visualize it

		crstl_constexpr fixed_vector() crstl_noexcept : m_length(0) {}

		crstl_constexpr14 fixed_vector(size_t initialLength) crstl_noexcept : m_length(0)
		{
			crstl_assert(initialLength < NumElements);
			default_initialize_or_memset_zero(m_data, initialLength);
			m_length = (length_type)initialLength;
		}

		crstl_constexpr14 fixed_vector(size_t initialLength, const T& value) crstl_noexcept : m_length(0)
		{
			crstl_assert(initialLength < NumElements);
			set_initialize_or_memset(m_data, value, initialLength);
			m_length = (length_type)initialLength;
		}

		crstl_constexpr14 fixed_vector(const this_type& other) crstl_noexcept { *this = other; }

		crstl_constexpr14 fixed_vector(this_type&& other) crstl_noexcept { *this = other; }

		crstl_constexpr14 fixed_vector(T* iter1, T* iter2) crstl_noexcept
		{
			crstl_assert(iter1 != nullptr && iter2 != nullptr);
			crstl_assert(iter2 >= iter1);

			size_t iter_length = (size_t)(iter2 - iter1);
			crstl_assert(iter_length < NumElements);

			copy_initialize_or_memcpy(m_data, iter1, iter_length);
			m_length = (length_type)iter_length;
		}

#if defined(CRSTL_FEATURE_INITIALIZER_LISTS)

		crstl_constexpr14 fixed_vector(std::initializer_list<T> ilist) crstl_noexcept : m_length(0)
		{
			crstl_assert((size_t)(ilist.end() - ilist.begin()) <= NumElements);

			for (const T *ptr = ilist.begin(), *end = ilist.end(); ptr != end; ++ptr)
			{
				push_back(*ptr);
			}
		}

#endif

		~fixed_vector() crstl_noexcept
		{
			clear();
		}

		this_type& operator = (const this_type& other) crstl_noexcept
		{
			copy_initialize_or_memcpy<T>(m_data, other.m_data, other.m_length);
			m_length = other.m_length;
			return *this;
		}

		this_type& operator = (this_type&& other) crstl_noexcept
		{
			m_length = other.m_length;

			for (size_t i = 0; i < other.m_length; ++i)
			{
				crstl_placement_new((void*)&m_data[i]) T(crstl::move(other.m_data[i]));
			}

			other.m_length = 0;
			return *this;
		}

		crstl_constexpr14 T& at(size_t i) { return crstl_assert(i < m_length), m_data[i]; }
		crstl_constexpr const T& at(size_t i) const { return crstl_assert(i < m_length), m_data[i]; }

		crstl_constexpr14 T& back() { return crstl_assert(m_length > 0), m_data[m_length - 1]; }
		crstl_constexpr const T& back() const { return crstl_assert(m_length > 0), m_data[m_length - 1]; }

		crstl_constexpr14 iterator begin() { return &m_data[0]; }
		crstl_constexpr const_iterator begin() const { return &m_data[0]; }
		crstl_constexpr const_iterator cbegin() const { return &m_data[0]; }

		crstl_constexpr size_t capacity() const { return NumElements; }

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
			crstl_assert(m_length < NumElements);
			crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Args>(args)...);
			m_length++;
			return back();
		}
#else
		template<typename Arg0> T& emplace_back(Arg0&& arg0)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1> T& emplace_back(Arg0&& arg0, Arg1&& arg1)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1)); m_length++; return back();
		}
		
		template<typename Arg0, typename Arg1, typename Arg2> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2)); m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length]) T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5> T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8));
			m_length++; return back();
		}

		template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
		T& emplace_back(Arg0&& arg0, Arg1&& arg1, Arg2&& arg2, Arg3&& arg3, Arg4&& arg4, Arg5&& arg5, Arg6&& arg6, Arg7&& arg7, Arg8&& arg8, Arg9&& arg9)
		{
			crstl_assert(m_length < NumElements); crstl_placement_new((void*)&m_data[m_length])
				T(crstl::forward<Arg0>(arg0), crstl::forward<Arg1>(arg1), crstl::forward<Arg2>(arg2), crstl::forward<Arg3>(arg3), crstl::forward<Arg4>(arg4), 
				crstl::forward<Arg5>(arg5), crstl::forward<Arg6>(arg6), crstl::forward<Arg7>(arg7), crstl::forward<Arg8>(arg8), crstl::forward<Arg9>(arg9));
			m_length++; return back();
		}

#endif

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_constexpr14 iterator end() { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator end() const { return &m_data[0] + m_length; }
		crstl_constexpr const_iterator cend() const { return &m_data[0] + m_length; }

		crstl_constexpr14 T& front() { return m_data[0]; }
		crstl_constexpr const T& front() const { return m_data[0]; }

		crstl_constexpr size_t max_size() const { return NumElements; }

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
			crstl_assert(m_length < NumElements);
			crstl_placement_new((void*)&m_data[m_length]) T();
			m_length++;
			return back();
		}

		crstl_constexpr14 void push_back(const T& v)
		{
			crstl_assert(m_length < NumElements);
			crstl_placement_new((void*)&m_data[m_length]) T(v);
			m_length++;
		}

		crstl_constexpr14 void push_back(T&& v)
		{
			crstl_assert(m_length < NumElements);
			crstl_placement_new((void*)&m_data[m_length]) T(crstl::move(v));
			m_length++;
		}

		crstl_constexpr14 T& push_back_uninitialized()
		{
			crstl_assert(m_length < NumElements);
			m_length++;
			return back();
		}

		//-------
		// resize
		//-------

		crstl_constexpr14 void resize(size_t length)
		{
			crstl_assert(length <= NumElements);

			if ((size_t)m_length < length)
			{
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
			crstl_assert(length <= NumElements);

			if ((size_t)m_length < length)
			{
				set_initialize_or_memset(&m_data[m_length], value, length - m_length);
			}
			else if ((size_t)m_length > length)
			{
				destruct_or_ignore(&m_data[length], m_length - length);
			}

			m_length = (length_type)length;
		}

		crstl_constexpr size_t size() const { return m_length; }

		crstl_constexpr14 void swap(this_type& v)
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
					memory_copy(chunkData, vData + chunkOffset, kMaxStack);
					memory_copy(vData + chunkOffset, thisData + chunkOffset, kMaxStack);
					memory_copy(thisData + chunkOffset, chunkData, kMaxStack);
				}

				const size_t kChunkCount = (sizeof(this_type) + kMaxStack - 1) / kMaxStack;
				const size_t kChunkRemaining = sizeof(this_type) - (kChunkCount - 1) * kMaxStack;

				// Copy last chunk
				uint8_t chunkData[kMaxStack];
				size_t chunkOffset = (chunks - 1) * kMaxStack;
				memory_copy(chunkData, vData + chunkOffset, kChunkRemaining);
				memory_copy(vData + chunkOffset, thisData + chunkOffset, kChunkRemaining);
				memory_copy(thisData + chunkOffset, chunkData, kChunkRemaining);
			}
		}

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