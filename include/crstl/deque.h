#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/allocator.h"

#include "crstl/compressed_pair.h"

#include "crstl/config_fwd.h"

// Only turn this on for development
//#define CRSTL_DEQUE_EXHAUSTIVE_VALIDATION

// deque
//
// Replacement for std::deque
//
// In the same spirit as the standard, it works via a series of fixed sized chunks, thereby having 
//
// Non-standard functions
//
// - push_[back/front](): push [back/front] an empty default object
// - push_[back/front]_uninitialized(): push [back/front] an uninitialized object
//
// - resize_[back/front](): resizes and puts all new elements at the [back/front] of the deque
//

crstl_module_export namespace crstl
{
	template<typename T, size_t ChunkSize>
	struct deque_chunk
	{
		~deque_chunk() {}

		crstl_warning_anonymous_struct_union_begin
		union
		{
			struct { T m_data[ChunkSize ? ChunkSize : 1]; };
		};
		crstl_warning_anonymous_struct_union_end
	};

	template<typename T, size_t ChunkSize>
	struct deque_iterator
	{
		typedef deque_iterator<T, ChunkSize>  this_type;
		typedef deque_chunk<T, ChunkSize>     chunk_type;
		typedef ptrdiff_t                     difference_type;
		typedef size_t                        length_type;

		deque_iterator(chunk_type** chunk_array, size_t chunk_index, size_t local_index)
		{
			m_chunk_array = chunk_array;
			m_chunk_index = chunk_index;
			m_local_index = local_index;
		}

		T* operator -> () const { return &m_chunk_array[m_chunk_index]->m_data[m_local_index]; }
		T& operator * () const { return m_chunk_array[m_chunk_index]->m_data[m_local_index]; }

		this_type& operator ++ () { increment(); return *this; }
		this_type operator ++ (int) { this_type temp(*this); increment(); return temp; }

		this_type& operator -- () { decrement(); return *this; }
		this_type operator -- (int) { this_type temp(*this); decrement(); return temp; }

		//this_type& operator += (difference_type n) { m_current += n; m_current %= NumElements; }
		//this_type& operator -= (difference_type n) { m_current -= n; m_current %= NumElements; }
		//
		//this_type operator + (difference_type n) const { return this_type(m_data, (m_current + n) % NumElements, m_begin, m_end); }
		//this_type operator - (difference_type n) const { return this_type(m_data, (m_current - n) % NumElements, m_begin, m_end); }

		bool operator == (const this_type& other) { return m_local_index == other.m_local_index && m_chunk_index == other.m_chunk_index; }
		bool operator != (const this_type& other) { return m_local_index != other.m_local_index || m_chunk_index != other.m_chunk_index; }

	private:

		void increment()
		{
			m_local_index++;
			
			if (m_local_index >= ChunkSize)
			{
				m_local_index = 0;
				m_chunk_index++;
			}
		}

		void decrement()
		{
			m_local_index--;

			if (m_local_index >= ChunkSize)
			{
				m_local_index = 0;
				m_chunk_index--;
			}
		}

		size_t m_chunk_index;

		size_t m_local_index;

		chunk_type** m_chunk_array;
	};

	template<typename T, typename Allocator = crstl::allocator, size_t ChunkSize = 16>
	class deque
	{
	public:

		typedef deque<T, Allocator, ChunkSize>     this_type;
		typedef T&                                 reference;
		typedef const T&                           const_reference;
		typedef T*                                 pointer;
		typedef const T*                           const_pointer;
		typedef deque_iterator<T, ChunkSize>       iterator;
		typedef const deque_iterator<T, ChunkSize> const_iterator;
		typedef deque_chunk<T, ChunkSize>          chunk_type;
		typedef size_t                             chunk_length_type;
		typedef uint32_t                           local_length_type;
		
		static const size_t kLastChunkElement = ChunkSize - 1;

		deque() crstl_noexcept : m_chunk_array(nullptr), m_length(0), m_chunk_begin(0), m_chunk_end(0), m_local_begin(0), m_local_end(0)
		{
			m_capacity_allocator.m_first = 0;
		}

		deque(size_t initial_size, const T& value) crstl_noexcept
		{
			size_t chunk_count = get_required_chunks(initial_size);

			m_chunk_array = (chunk_type**)m_capacity_allocator.second().allocate(chunk_count * sizeof(chunk_type*));

			#if defined(CRSTL_DEQUE_EXHAUSTIVE_VALIDATION)
				memset(m_chunk_array, 0, chunk_count * sizeof(chunk_type*));
			#endif

			// Can this be done in a single allocation? How to keep track of memory blocks?
			for (size_t i = 0; i < chunk_count; ++i)
			{
				m_chunk_array[i] = (chunk_type*)m_capacity_allocator.second().allocate(sizeof(chunk_type));
			}

			m_chunk_begin = 0;
			m_local_begin = 0;

			iterate(0, initial_size, [&](T& data)
			{
				::new((void*)&data) T(value);
			});

			m_capacity_allocator.m_first = chunk_count * ChunkSize;
			m_length = initial_size;

			m_chunk_end = (chunk_length_type)(initial_size / ChunkSize);
			m_local_end = (local_length_type)(initial_size - m_chunk_end * ChunkSize);
		}

		~deque()
		{
			size_t current_chunk_count = m_capacity_allocator.m_first / ChunkSize;

			for (size_t i = 0; i < current_chunk_count; ++i)
			{
				m_capacity_allocator.second().deallocate(m_chunk_array[i], sizeof(chunk_type));
			}

			m_capacity_allocator.second().deallocate(m_chunk_array, current_chunk_count * sizeof(chunk_type*));
		}

		crstl_constexpr reference at(size_t i)
		{
			crstl_assert(i < m_length);
			size_t global_begin = m_chunk_begin * ChunkSize + m_local_begin + i;
			return m_chunk_array[global_begin / ChunkSize]->m_data[global_begin - (global_begin / ChunkSize) * ChunkSize];
		}

		crstl_constexpr const_reference at(size_t i) const
		{
			crstl_assert(i < m_length);
			size_t global_begin = m_chunk_begin * ChunkSize + m_local_begin + i;
			return m_chunk_array[global_begin / ChunkSize]->m_data[global_begin - (global_begin / ChunkSize) * ChunkSize];
		}

		crstl_constexpr reference back()
		{
			crstl_assert(m_length > 0);
			return m_chunk_array[m_local_end - 1 > kLastChunkElement ? m_chunk_end - 1 : m_chunk_end]->m_data[m_local_end - 1 > kLastChunkElement ? kLastChunkElement : m_local_end - 1];
		}

		crstl_constexpr const_reference back() const
		{
			crstl_assert(m_length > 0);
			return m_chunk_array[m_local_end - 1 > kLastChunkElement ? m_chunk_end - 1 : m_chunk_end]->m_data[m_local_end - 1 > kLastChunkElement ? kLastChunkElement : m_local_end - 1];
		}

		crstl_constexpr iterator begin() { return iterator(m_chunk_array, m_chunk_begin, m_local_begin); }
		crstl_constexpr const_iterator begin() const { return iterator(m_chunk_array, m_chunk_begin, m_local_begin); }
		crstl_constexpr const_iterator cbegin() const { return iterator(m_chunk_array, m_chunk_begin, m_local_begin); }

		crstl_constexpr size_t capacity() const { return m_capacity_allocator.m_first; }

		crstl_constexpr void clear()
		{
			crstl_constexpr_if(!crstl_is_trivially_destructible(T))
			{
				for (size_t i = 0; i < m_length; ++i)
				{
					size_t chunk_index = i / ChunkSize;
					size_t value_index = i - chunk_index * ChunkSize;
					m_chunk_array[chunk_index]->m_data[value_index].~T();
				}
			}

			m_chunk_begin = m_chunk_end = 0;
			m_local_begin = m_local_end = 0;
			m_length = 0;
		}

#if defined(CRSTL_VARIADIC_TEMPLATES)

		template<typename... Args>
		crstl_constexpr reference emplace_back(Args&&... args)
		{
			request_capacity_back(1);
			reference data = m_chunk_array[m_chunk_end]->m_data[m_local_end];
			::new((void*)&data) T(crstl::forward<Args>(args)...);
			increment_back();
			return data;
		}

		template<typename... Args>
		crstl_constexpr reference emplace_front(Args&&... args)
		{
			request_capacity_front(1);
			increment_front();
			reference data = m_chunk_array[m_chunk_begin]->m_data[m_local_begin]; 
			::new((void*)&data) T(crstl::forward<Args>(args)...);
			return data;
		}

#endif

		crstl_nodiscard
		crstl_constexpr bool empty() const { return m_length == 0; }

		crstl_constexpr iterator end() { return iterator(m_chunk_array, m_chunk_end, m_local_end); }
		crstl_constexpr const_iterator end() const { return iterator(m_chunk_array, m_chunk_end, m_local_end); }
		crstl_constexpr const_iterator cend() const { return iterator(m_chunk_array, m_chunk_end, m_local_end); }
		
		crstl_constexpr reference front() { crstl_assert(m_length > 0); return m_chunk_array[m_chunk_begin]->m_data[m_local_begin]; }
		crstl_constexpr const_reference front() const { crstl_assert(m_length > 0); return m_chunk_array[m_chunk_begin]->m_data[m_local_begin]; }

		crstl_constexpr void pop_back()
		{
			crstl_assert(m_length > 0);
			decrement_back();
			m_chunk_array[m_chunk_end]->m_data[m_local_end].~T();
		}

		crstl_constexpr void pop_front()
		{
			crstl_assert(m_length > 0);
			m_chunk_array[m_chunk_begin]->m_data[m_local_begin].~T();
			decrement_front();
		}

		//----------
		// push_back
		//----------

		crstl_constexpr reference push_back()
		{
			request_capacity_back(1);
			reference data = m_chunk_array[m_chunk_end]->m_data[m_local_end];
			::new((void*)&data) T();
			increment_back();
			return data;
		}

		crstl_constexpr reference push_back(const T& v)
		{
			request_capacity_back(1);
			reference data = m_chunk_array[m_chunk_end]->m_data[m_local_end];
			::new((void*)&data) T(v);
			increment_back();
			return data;
		}

		crstl_constexpr reference push_back(T&& v)
		{
			request_capacity_back(1);
			reference data = m_chunk_array[m_chunk_end]->m_data[m_local_end];
			::new((void*)&data) T(crstl::move(v));
			increment_back();
			return data;
		}

		crstl_constexpr reference push_back_uninitialized()
		{
			request_capacity_back(1);
			reference data = m_chunk_array[m_chunk_end]->m_data[m_local_end];
			increment_back();
			return data;
		}

		//-----------
		// push_front
		//-----------

		crstl_constexpr reference push_front()
		{
			request_capacity_front(1);
			increment_front();
			reference data = m_chunk_array[m_chunk_begin]->m_data[m_local_begin];
			::new((void*)&data) T();
			return data;
		}

		crstl_constexpr reference push_front(const T& v)
		{
			request_capacity_front(1);
			increment_front();
			reference data = m_chunk_array[m_chunk_begin]->m_data[m_local_begin];
			::new((void*)&data) T(v);
			return data;
		}

		crstl_constexpr reference push_front(T&& v)
		{
			request_capacity_front(1);
			increment_front();
			reference data = m_chunk_array[m_chunk_begin]->m_data[m_local_begin];
			::new((void*)&data) T(crstl::move(v));
			return data;
		}

		crstl_constexpr reference push_front_uninitialized()
		{
			request_capacity_front(1);
			increment_front();
			reference data = m_chunk_array[m_chunk_begin]->m_data[m_local_begin];
			return data;
		}

		//-------
		// resize
		//-------

		// Assume resize has the same behavior as resize_back
		crstl_constexpr void resize(size_t length)
		{
			resize_back(length);
		}

		crstl_constexpr void resize_back(size_t length)
		{
			if (length > (size_t)m_length)
			{
				request_capacity_back(length - m_capacity_allocator.m_first);
				
				iterate(m_length, length, [&](T& data)
				{
					::new((void*)&data) T();
				});

				size_t global_end = m_chunk_begin * ChunkSize + m_local_begin + length;
				m_chunk_end = global_end / ChunkSize;
				m_local_end = (local_length_type)(global_end - (global_end / ChunkSize) * ChunkSize);
				m_length = (local_length_type)length;
			}
			else if (length < (size_t)m_length)
			{
				crstl_constexpr_if(!crstl_is_trivially_destructible(T))
				{
					iterate(length, m_length, [&](T& data)
					{
						data.~T();
					});
				}

				size_t global_end = m_chunk_begin * ChunkSize + m_local_begin + length;
				m_chunk_end = global_end / ChunkSize;
				m_local_end = (local_length_type)(global_end - (global_end / ChunkSize) * ChunkSize);
				m_length = (local_length_type)length;
			}
		}

		// This is difficult, we need to make a reservation backwards
		// and then loop through the uninitialized elements to initialize
		// or the existing elements to destroy them
		crstl_constexpr void resize_front(size_t length)
		{
			if (length > (size_t)m_length)
			{
				ptrdiff_t capacity_increment = length - m_capacity_allocator.m_first;

				request_capacity_front(capacity_increment);

				iterate(-capacity_increment, 0, [&](T& data)
				{
					::new((void*)&data) T();
				});

				size_t global_begin = m_chunk_begin * ChunkSize + m_local_begin - capacity_increment;
				m_chunk_begin = global_begin / ChunkSize;
				m_local_begin = (local_length_type)(global_begin - (global_begin / ChunkSize) * ChunkSize);
				m_length = (local_length_type)length;
			}
			else if (length < (size_t)m_length)
			{
				crstl_constexpr_if(!crstl_is_trivially_destructible(T))
				{
					ptrdiff_t length_decrement = m_length - length;

					iterate(0, length_decrement, [&](T& data)
					{
						data.~T();
					});

					size_t global_begin = m_chunk_begin * ChunkSize + m_local_begin - length_decrement;
					m_chunk_begin = global_begin / ChunkSize;
					m_local_begin = (local_length_type)(global_begin - (global_begin / ChunkSize) * ChunkSize);
					m_length = (local_length_type)length;
				}
			}
		}

		// This essentially needs to call resize back and front with half the elements
		// void resize_balanced(size_t length)

		crstl_constexpr size_t size() const { return m_length; }

		crstl_constexpr reference operator [] (size_t i)
		{
			crstl_assert(i < m_length);
			size_t global_begin = m_chunk_begin * ChunkSize + m_local_begin + i;
			return m_chunk_array[global_begin / ChunkSize]->m_data[global_begin - (global_begin / ChunkSize) * ChunkSize];
		}

		crstl_constexpr const_reference operator [] (size_t i) const
		{
			crstl_assert(i < m_length);
			size_t global_begin = m_chunk_begin * ChunkSize + m_local_begin + i;
			return m_chunk_array[global_begin / ChunkSize]->m_data[global_begin - (global_begin / ChunkSize) * ChunkSize];
		}

	private:

		template<typename Function>
		crstl_constexpr void iterate(ptrdiff_t begin, ptrdiff_t end, Function function)
		{
			ptrdiff_t global_offset = m_chunk_begin * ChunkSize + m_local_begin;

			for (ptrdiff_t i = begin; i < end; ++i)
			{
				ptrdiff_t global_begin = global_offset + i;
				crstl_assert(global_begin >= 0);
				function(m_chunk_array[global_begin / ChunkSize]->m_data[global_begin - (global_begin / ChunkSize) * ChunkSize]);
			}
		}

		crstl_constexpr size_t get_required_chunks(size_t length)
		{
			return (length + ChunkSize - 1) / ChunkSize;
		}

		crstl_constexpr size_t compute_new_capacity(size_t old_capacity)
		{
			return old_capacity + (old_capacity * 50) / 100;
		}

		// Requests incremental capacity in a given direction, which could mean rebalancing the entries to make space
		// for the given elements, or reallocating as necessary
		crstl_constexpr void request_capacity_back(size_t requested_capacity_increment)
		{
			size_t remaining_capacity_back = m_capacity_allocator.m_first - (m_chunk_end * ChunkSize + m_local_end);

			// If we have capacity in the remaining chunks
			if (requested_capacity_increment <= remaining_capacity_back)
			{
				return;
			}

			size_t current_chunk_capacity = m_capacity_allocator.m_first / ChunkSize;
			size_t remaining_chunks_front = m_chunk_begin;
			size_t refit_elements = requested_capacity_increment - remaining_capacity_back;
			size_t refit_chunks = (refit_elements + ChunkSize - 1) / ChunkSize;

			// If we have the number of chunks available at the front, copy them over to the back
			if (refit_chunks <= remaining_chunks_front)
			{
				// Copy chunk pointers to temporary memory. We shouldn't normally need this many
				chunk_type** temp = (chunk_type**)crstl_alloca(refit_chunks * sizeof(chunk_type*));
				chunk_type** chunk_ptr_src_tmp = m_chunk_array + m_chunk_begin - refit_chunks;
				for (size_t i = 0; i < refit_chunks; ++i)
				{
					temp[i] = chunk_ptr_src_tmp[i];
				}

				// This should be guaranteed not to overflow by construction
				size_t current_chunk_count = m_chunk_end - m_chunk_begin + 1;
				crstl_assert(current_chunk_count > 0);

				// Move existing used chunks to make space
				chunk_type** chunk_ptr_src = m_chunk_array + m_chunk_begin;
				chunk_type** chunk_ptr_dst = m_chunk_array + m_chunk_begin - refit_chunks;
				for (size_t i = 0; i < current_chunk_count; ++i)
				{
					chunk_ptr_dst[i] = chunk_ptr_src[i];
				}

				// Copy the new chunk pointers
				chunk_type** chunk_ptr_dst_tmp = m_chunk_array + m_chunk_end - refit_chunks;
				for (size_t i = 0; i < refit_chunks; ++i)
				{
					chunk_ptr_dst_tmp[i] = temp[i];
				}

				m_chunk_begin -= refit_chunks;
				m_chunk_end -= refit_chunks;
			}
			// If there is no capacity, we need actual reallocation
			else
			{
				// 1. Compute required capacity and chunk count
				size_t current_capacity = m_capacity_allocator.m_first;
				size_t growth_capacity = compute_new_capacity(current_capacity);
				size_t requested_capacity = current_capacity + requested_capacity_increment;
				size_t new_capacity = requested_capacity > growth_capacity ? requested_capacity : growth_capacity;

				size_t growth_chunk_count = (new_capacity + ChunkSize - 1) / ChunkSize;

				// 2. Allocate chunk pointers
				chunk_type** temp_array = (chunk_type**)m_capacity_allocator.second().allocate(growth_chunk_count * sizeof(chunk_type*));
				
				#if defined(CRSTL_DEQUE_EXHAUSTIVE_VALIDATION)
					memset(temp_array, 0, growth_chunk_count * sizeof(chunk_type*));
				#endif

				// 3. Copy existing chunk pointers
				for (size_t i = m_chunk_begin; i < m_chunk_end; ++i)
				{
					temp_array[i] = m_chunk_array[i];
				}

				// 4. Deallocate old array and point to new one
				m_capacity_allocator.second().deallocate(m_chunk_array, current_chunk_capacity * sizeof(chunk_type*));
				m_chunk_array = temp_array;

				// 5. Allocate new chunks
				for (size_t i = m_chunk_end; i < growth_chunk_count; ++i)
				{
					m_chunk_array[i] = (chunk_type*)m_capacity_allocator.second().allocate(sizeof(chunk_type));
				}

				m_capacity_allocator.m_first = growth_chunk_count * ChunkSize;
			}

			#if defined(CRSTL_DEQUE_EXHAUSTIVE_VALIDATION)
				for (size_t i = 0; i < current_chunk_capacity; ++i)
				{
					crstl_assert(m_chunk_array[i] != nullptr);
					for (size_t j = i + 1; j < current_chunk_capacity; ++j)
					{
						crstl_assert(m_chunk_array[i] != m_chunk_array[j]);
					}
				}
			#endif
		}

		crstl_constexpr void request_capacity_front(size_t requested_capacity_increment)
		{
			size_t remaining_capacity_front = m_chunk_begin * ChunkSize + m_local_begin;

			// If we have capacity in the remaining chunks
			if (requested_capacity_increment <= remaining_capacity_front)
			{
				return;
			}

			size_t current_chunk_capacity = m_capacity_allocator.m_first / ChunkSize;
			size_t refit_elements = requested_capacity_increment - remaining_capacity_front;
			size_t refit_chunks = (refit_elements + ChunkSize - 1) / ChunkSize;

			size_t remaining_chunks_back = m_chunk_end < current_chunk_capacity ? current_chunk_capacity - (m_chunk_end + 1) : 0;
			
			// If we have the number of chunks available at the front, copy them over to the back
			if (refit_chunks <= remaining_chunks_back)
			{
				// Copy chunk pointers to temporary memory. We shouldn't normally need this many
				chunk_type** temp = (chunk_type**)crstl_alloca(refit_chunks * sizeof(chunk_type*));
				chunk_type** chunk_ptr_src_tmp = m_chunk_array + m_chunk_end + 1;
				for (size_t i = 0; i < refit_chunks; ++i)
				{
					temp[i] = chunk_ptr_src_tmp[i];
				}

				// Move existing used chunks to make space
				chunk_type** chunk_ptr_src = m_chunk_array;
				chunk_type** chunk_ptr_dst = m_chunk_array + refit_chunks;
				for (size_t i = m_chunk_end; i != size_t(-1); --i)
				{
					chunk_ptr_dst[i] = chunk_ptr_src[i];
				}

				// Copy the new chunk pointers
				chunk_type** chunk_ptr_dst_tmp = m_chunk_array + m_chunk_begin;
				for (size_t i = 0; i < refit_chunks; ++i)
				{
					chunk_ptr_dst_tmp[i] = temp[i];
				}

				m_chunk_begin += refit_chunks;
				m_chunk_end += refit_chunks;
			}
			// If there is no capacity, we need actual reallocation
			else
			{
				// 1. Compute required capacity and chunk count
				size_t current_capacity = m_capacity_allocator.m_first;
				size_t growth_capacity = compute_new_capacity(current_capacity);
				size_t requested_capacity = current_capacity + requested_capacity_increment;
				size_t new_capacity = requested_capacity > growth_capacity ? requested_capacity : growth_capacity;

				size_t growth_chunk_count = (new_capacity + ChunkSize - 1) / ChunkSize;

				// 2. Allocate chunk pointers
				chunk_type** temp_array = (chunk_type**)m_capacity_allocator.second().allocate(growth_chunk_count * sizeof(chunk_type*));

				#if defined(CRSTL_DEQUE_EXHAUSTIVE_VALIDATION)
					memset(temp_array, 0, growth_chunk_count * sizeof(chunk_type*));
				#endif

				// 3. Copy existing chunk pointers

				size_t new_chunk_count = growth_chunk_count - current_chunk_capacity;

				chunk_type** chunk_ptr_dst = temp_array + new_chunk_count;
				for (size_t i = 0; i < current_chunk_capacity; ++i)
				{
					chunk_ptr_dst[i] = m_chunk_array[i];
				}

				// 4. Deallocate old array and point to new one
				m_capacity_allocator.second().deallocate(m_chunk_array, current_chunk_capacity * sizeof(chunk_type*));

				// 5. Allocate new chunks
				for (size_t i = 0; i < new_chunk_count; ++i)
				{
					temp_array[i] = (chunk_type*)m_capacity_allocator.second().allocate(sizeof(chunk_type));
				}

				m_chunk_array = temp_array;
				m_capacity_allocator.m_first = growth_chunk_count * ChunkSize;

				m_chunk_begin += new_chunk_count;
				m_chunk_end += new_chunk_count;
			}

			#if defined(CRSTL_DEQUE_EXHAUSTIVE_VALIDATION)
				for (size_t i = 0; i < current_chunk_capacity; ++i)
				{
					crstl_assert(m_chunk_array[i] != nullptr);
					for (size_t j = i + 1; j < current_chunk_capacity; ++j)
					{
						crstl_assert(m_chunk_array[i] != m_chunk_array[j]);
					}
				}
			#endif
		}

		void increment_back()
		{
			m_length++;
			m_local_end++;
			if (m_local_end >= ChunkSize)
			{
				m_local_end = 0;
				m_chunk_end++;
			}
		}

		void decrement_back()
		{
			m_length--;
			m_local_end--;
			if (m_local_end >= ChunkSize)
			{
				m_local_end = ChunkSize - 1;
				m_chunk_end--;
			}
		}

		void increment_front()
		{
			m_length++;
			m_local_begin--;
			if (m_local_begin >= ChunkSize)
			{
				m_local_begin = ChunkSize - 1;
				m_chunk_begin--;
			}
		}

		void decrement_front()
		{
			m_length--;
			m_local_begin++;
			if (m_local_begin >= ChunkSize)
			{
				m_local_begin = 0;
				m_chunk_begin++;
			}
		}

		chunk_type** m_chunk_array;

		size_t m_length;

		chunk_length_type m_chunk_begin;

		chunk_length_type m_chunk_end;

		local_length_type m_local_begin;

		local_length_type m_local_end;

		compressed_pair<size_t, Allocator> m_capacity_allocator;
	};
};