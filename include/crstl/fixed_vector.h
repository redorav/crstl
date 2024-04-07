#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/vector_base.h"
#include "crstl/move_forward.h"
#include "crstl/type_builtins.h"
#include "crstl/utility/placement_new.h"
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
	class fixed_vector_storage
	{
	public:

		typedef typename fixed_length_select_type<NumElements>::type length_type;

		crstl_constexpr fixed_vector_storage() crstl_noexcept : m_length(0) {}

	protected:

		size_t get_capacity() const { return NumElements; }

		void reallocate_if_length_equals_capacity()
		{
			crstl_assert(m_length < NumElements);
		}

		void reallocate_if_length_greater_than_capacity(size_t length)
		{
			crstl_unused(length);
			crstl_assert(length <= NumElements);
		}

		// The union is here to avoid calling the default constructor of T on construction of fixed_vector.
		~fixed_vector_storage() {}

		// The alternative is to have a uint8_t buffer that we reinterpret through the interface, which is
		// hard to debug and would certainly need custom visualizers. This alternative works with any C++11
		// conforming compiler
		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { T m_data[NumElements]; };
		};
		crstl_warning_anonymous_struct_union_end

		length_type m_length;
	};

	template<typename T, size_t NumElements>
	class fixed_vector : public vector_base<T, fixed_vector_storage<T, NumElements>>
	{
	public:

		typedef vector_base<T, fixed_vector_storage<T, NumElements>> base_type;
		typedef fixed_vector<T, NumElements>                         this_type;

		typedef typename base_type::length_type     length_type;
		typedef typename base_type::reference       reference;
		typedef typename base_type::const_reference const_reference;
		typedef typename base_type::iterator        iterator;
		typedef typename base_type::const_iterator  const_iterator;
		typedef typename base_type::pointer         pointer;
		typedef typename base_type::const_pointer   const_pointer;
		
		using base_type::m_length;
		using base_type::m_data;

		using base_type::clear;
		using base_type::push_back;

		enum
		{
			kMaxStack = 4096,
		};

		// We need to define all standard operators because we've made the m_data member a union. The reason for it is to avoid
		// invoking the default constructor of T. We don't want to change it to some raw type like char as it becomes hard to
		// debug later on and would rely on a natvis to properly visualize it

		crstl_constexpr fixed_vector() crstl_noexcept : base_type() {}

		crstl_constexpr14 fixed_vector(size_t initialLength) crstl_noexcept : base_type()
		{
			crstl_assert(initialLength < NumElements);
			default_initialize_or_memset_zero(m_data, initialLength);
			m_length = (length_type)initialLength;
		}

		crstl_constexpr14 fixed_vector(size_t initialLength, const T& value) crstl_noexcept : base_type()
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

		crstl_constexpr14 fixed_vector(std::initializer_list<T> ilist) crstl_noexcept : base_type()
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
			if (this != &other)
			{
				m_length = other.m_length;

				for (size_t i = 0; i < other.m_length; ++i)
				{
					crstl_placement_new((void*)&m_data[i]) T(crstl_move(other.m_data[i]));
				}

				other.m_length = 0;
			}
			
			return *this;
		}

		crstl_constexpr size_t max_size() const { return NumElements; }

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
				size_t chunks     = (sizeof(this_type) + kMaxStack - 1) / kMaxStack;
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

		operator span<T>() const;
	};

	template<typename T, size_t NumElements>
	fixed_vector<T, NumElements>::operator span<T>() const
	{
		return span<T>((T*)m_data, (size_t)m_length);
	}
};