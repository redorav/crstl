#pragma once

#include "config.h"

#include "type_utils.h"

#include <stdint.h>

#include <string.h>

namespace crstl
{
	template<typename T, uint64_t NumElements>
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

		enum
		{
			kMaxStack = 4096,
		};

		// We need to define all standard operators because we've made the m_data member a union. The reason for it is to avoid
		// invoking the default constructor of T. We don't want to change it to some raw type like char as it becomes hard to
		// debug later on and would rely on a natvis to properly visualize it

		fixed_vector() crstl_noexcept : m_currentSize(0) {}
		~fixed_vector() crstl_noexcept {}
		fixed_vector(const this_type& v) { *this = v; }
		fixed_vector(this_type&& v) crstl_noexcept { *this = v; }

		this_type& operator = (const this_type& v) crstl_noexcept
		{
			memcpy(m_data, v.m_data, sizeof(m_data));
			return *this;
		}

		this_type& operator = (this_type&& v) crstl_noexcept
		{
			crstl::swap(m_currentSize, v.m_currentSize);
			crstl::swap(m_data, v.m_data);
			return *this;
		}

		reference at(size_t i) { crstl_assert(i < m_currentSize); return m_data[i]; }
		const_reference at(size_t i) const { crstl_assert(i < m_currentSize); return m_data[i]; }

		reference back() { crstl_assert(m_currentSize > 0); return m_data[m_currentSize - 1]; }
		const_reference back() const { crstl_assert(m_currentSize > 0); return m_data[m_currentSize - 1]; }

		iterator begin() { return &m_data[0]; }
		const_iterator begin() const { return &m_data[0]; }
		const_iterator cbegin() const { return &m_data[0]; }

		size_t capacity() const { return NumElements; }

		void clear()
		{
			for (size_t i = 0; i < m_currentSize; ++i)
			{
				m_data[i].~T();
			}

			m_currentSize = 0;
		}

		pointer data() { return &m_data[0]; }
		const_pointer data() const { return &m_data[0]; }

		template<class... Args>
		reference emplace_back(Args&&... args)
		{
			crstl_assert(m_currentSize < NumElements);
			::new((void*)&m_data[m_currentSize]) T(crstl::forward<Args>(args)...);
			m_currentSize++;
			return back();
		}

		bool empty() const { return m_currentSize == 0; }

		iterator end() { return &m_data[0] + m_currentSize; }
		const_iterator end() const { return &m_data[0] + m_currentSize; }
		const_iterator cend() const { return &m_data[0] + m_currentSize; }

		reference front() { m_data[0]; }
		const_reference front() const { m_data[0]; }

		void pop_back()
		{
			crstl_assert(m_currentSize > 0);
			back().~T();
			m_currentSize--;
		}

		reference push_back()
		{
			crstl_assert(m_currentSize < NumElements);
			::new((void*)&m_data[m_currentSize]) T();
			m_currentSize++;
			return m_data[m_currentSize - 1];
		}

		reference push_back_uninitialized()
		{
			m_currentSize++;
			return back();
		}

		void push_back(const T& v)
		{
			::new((void*)&m_data[m_currentSize]) T(crstl::move(v));
			m_currentSize++;
		}

		void push_back(T&& v)
		{
			::new((void*)&m_data[m_currentSize]) T(crstl::move(v));
			m_currentSize++;
		}

		void resize(size_t s)
		{
			if (s > (size_t)m_currentSize)
			{
				for (uint64_t i = 0; i < s; ++i)
				{
					::new((void*)&m_data[i]) T();
				}
			}
			else if (s < (size_t)m_currentSize)
			{
				for (uint64_t i = s; s < m_currentSize; ++i)
				{
					m_data[i].~T();
				}
			}

			m_currentSize = (uint32_t)s;
		}

		size_t size() const { return m_currentSize; }

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
				uint64_t chunks   = (sizeof(this_type) + kMaxStack - 1) / kMaxStack;
				uint8_t* vData    = reinterpret_cast<uint8_t*>(&v);
				uint8_t* thisData = reinterpret_cast<uint8_t*>(this);

				for (uint32_t c = 0; c < chunks - 1; ++c)
				{
					uint8_t chunkData[kMaxStack];
					uint64_t chunkOffset = c * kMaxStack;
					memcpy(chunkData, vData + chunkOffset, kMaxStack);
					memcpy(vData + chunkOffset, thisData + chunkOffset, kMaxStack);
					memcpy(thisData + chunkOffset, chunkData, kMaxStack);
				}

				static const uint64_t kChunkCount = (sizeof(this_type) + kMaxStack - 1) / kMaxStack;
				static const uint64_t kChunkRemaining = sizeof(this_type) - (kChunkCount - 1) * kMaxStack;

				// Copy last chunk
				uint8_t chunkData[kMaxStack];
				uint64_t chunkOffset = (chunks - 1) * kMaxStack;
				memcpy(chunkData, vData + chunkOffset, kChunkRemaining);
				memcpy(vData + chunkOffset, thisData + chunkOffset, kChunkRemaining);
				memcpy(thisData + chunkOffset, chunkData, kChunkRemaining);
			}
		}

		reference operator [] (size_t i) { crstl_assert(i < m_currentSize); return m_data[i]; }

		const_reference operator [] (size_t i) const { crstl_assert(i < m_currentSize); return m_data[i]; }

	private:

		// The union is here to avoid calling the default constructor of T on construction of fixed_vector.
		// The alternative is to have a uint8_t buffer that we reinterpret through the interface, which is
		// hard to debug and would certainly need custom visualizers. This alternative works with any C++11
		// conforming compiler
		union
		{
			T m_data[NumElements ? NumElements : 1];
		};

		uint32_t m_currentSize;
	};
};