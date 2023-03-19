#pragma once

#include "crstl/config.h"

#include "crstl/allocator.h"

#include "crstl/compressed_pair.h"

#include "crstl/string_common.h"

// string
//
// Replacement for std::string
//

namespace crstl
{
	template<typename T, typename Allocator = crstl::allocator<T>>
	class basic_string
	{
	public:

		typedef T                          value_type;
		typedef T&                         reference;
		typedef const T&                   const_reference;
		typedef T*                         pointer;
		typedef const T*                   const_pointer;
		typedef T*                         iterator;
		typedef const T*                   const_iterator;

		// View of heap-allocated string
		struct heap_view
		{
			T* data;
			size_t length;
			size_t capacity;
		};

		// Insert padding when sizeof(value_type) > 1
		template<int N> struct sso_padding { char padding[N]; };
		template<> struct sso_padding<0> {};

		struct sso_size : sso_padding<sizeof(value_type) - 1>
		{
			char value;
		};

		// View of string acting as small-string optimization
		struct sso_view
		{
			value_type data[sizeof(heap_view) / sizeof(value_type) - 1];
			sso_size remaining_length;
		};

		struct layout
		{
			union
			{
				heap_view m_heap;
				sso_view m_sso;
			};
		};

		static_assert(sizeof(heap_view) == sizeof(sso_view), "Size mismatch");

		static const size_t kCharSize = sizeof(T);

		static const size_t kSSOCapacity = sizeof(sso_view) / sizeof(value_type) - 1;

		// This is valid for little endian machines, where the remaining_length and capacity
		// share the same space in memory
		static const size_t kSSOMask = 0x80;

		static const size_t kHeapCapacityMask = ((size_t)-1) >> 1;

		static const crstl_constexpr size_t npos = (size_t)-1;

		//-------------
		// Constructors
		//-------------

		crstl_constexpr basic_string() crstl_noexcept
		{
			m_layout_allocator.m_first.m_sso.data[0] = 0;
			m_layout_allocator.m_first.m_sso.remaining_length.value = kSSOCapacity;
		}

		crstl_constexpr basic_string(const_pointer string, size_t length) crstl_noexcept
		{
			crstl_assert(string != nullptr);
			initialize_string(string, length);
		}

		template<int N>
		explicit crstl_constexpr basic_string(const T(&string_literal)[N]) crstl_noexcept
		{
			initialize_string(string_literal, N - 1);
		}

		template<int N>
		explicit crstl_constexpr basic_string(T(&char_array)[N]) crstl_noexcept
		{
			initialize_string(char_array, string_length(char_array, N));
		}

		template<typename T>
		crstl_constexpr basic_string(T string, crstl_is_char_ptr(T)) crstl_noexcept
		{
			initialize_string(string, string_length(string));
		}

		crstl_constexpr basic_string(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			initialize_string(begin, end - begin);
		}

		crstl_constexpr basic_string(const basic_string& string) crstl_noexcept
		{
			initialize_string(string.c_str(), string.size());
		}

		crstl_constexpr basic_string(const basic_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			crstl_assert(subpos + sublen <= string.length());
			initialize_string(string.c_str() + subpos, sublen);
		}

		~basic_string() crstl_noexcept
		{
			if (is_heap())
			{
				deallocate_heap();
			}
		}

		crstl_constexpr reference at(size_t i) crstl_noexcept
		{
			crstl_assert(i < length());
			return data()[i];
		}

		crstl_constexpr const_reference at(size_t i) const crstl_noexcept
		{
			crstl_assert(i < length());
			return data()[i];
		}

		//-------
		// append
		//-------

		// Append a const char* string with a provided length
		crstl_constexpr basic_string& append(const_pointer string, size_t length)
		{
			append_function(length, [string, length](T* begin, T* /*end*/)
			{
				memcpy(begin, string, length * kCharSize);
			});

			return *this;
		}

		template<int N>
		crstl_constexpr basic_string& append(const T(&string_literal)[N]) crstl_noexcept
		{
			append(string_literal, N - 1); return *this;
		}

		template<int N>
		crstl_constexpr basic_string& append(T(&char_array)[N]) crstl_noexcept
		{
			append(char_array, string_length(char_array, N - 1)); return *this;
		}

		template<typename T>
		crstl_constexpr basic_string& append(T string, crstl_is_char_ptr(T))
		{
			return append(string, string_length(string));
		}

		crstl_constexpr basic_string& append(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end > begin);
			append(begin, (size_t)(end - begin));
			return *this;
		}

		crstl_constexpr basic_string& append(const basic_string& string) crstl_noexcept
		{
			append(string.data(), string.length()); return *this;
		}

		crstl_constexpr basic_string& append(size_t n, value_type c) crstl_noexcept
		{
			append_function(n, [c](T* begin, T* end)
			{
				while (begin != end)
				{
					*begin = c;
					++begin;
				}
			});

			return *this;
		}

		//---------------
		// append_convert
		//---------------

		//template<typename OtherCharT>
		//crstl_constexpr basic_string& append_convert(const OtherCharT* string, size_t length) crstl_noexcept
		//{
		//	size_t current_length = basic_string::length();
		//	size_t target_length = current_length + length;
		//
		//	const OtherCharT* stringEnd = string + length;
		//	value_type* dataStart = m_data + m_length;
		//
		//	size_t sizeBytes = 0;
		//	bool success = decode_chunk(dataStart, dataStart + (kCharacterCapacity - m_length), string, stringEnd, sizeBytes);
		//	crstl_assert(success);
		//
		//	m_length += (uint32_t)sizeBytes;
		//	m_data[m_length] = '\0';
		//
		//	return *this;
		//}

		//---------------
		// append_sprintf
		//---------------
		
		// Append a const char* string with a provided length
		crstl_constexpr basic_string& append_sprintf(const_pointer format, ...) crstl_noexcept
		{
			va_list va_arguments;
			va_start(va_arguments, format);

			size_t current_length = length();
			size_t remaining_length = capacity() - current_length;

			T* data = basic_string::data();

			// Try to copy, limiting the number of characters to what we have available
			int char_count = vsnprintf(data + current_length, remaining_length, format, va_arguments);

			// It is a formatting error to return a negative number
			crstl_assert(char_count >= 0);

			// If the number of characters we would have written is greater than what we had available, 
			// we need to heap reallocate to have enough space, then try again. We would actually like
			// this to continue where it left off but that's not possible with format specifiers
			if ((size_t)char_count > remaining_length)
			{
				reallocate_heap_larger(current_length + char_count);
				char_count = vsnprintf(m_layout_allocator.m_first.m_heap.data + current_length, char_count, format, va_arguments);
				crstl_assert(char_count >= 0);
			}

			if (is_sso())
			{
				m_layout_allocator.m_first.m_sso.remaining_length.value = (char)(kSSOCapacity - char_count);
			}
			else
			{
				m_layout_allocator.m_first.m_heap.length = char_count;
			}

			return *this;
		}

		//-------
		// assign
		//-------

		crstl_constexpr basic_string& assign(const_pointer string, size_t length) crstl_noexcept
		{
			clear(); append(string, length); return *this;
		}

		template<int N>
		crstl_constexpr basic_string& assign(const T(&string_literal)[N]) crstl_noexcept
		{
			clear(); assign(string_literal, N - 1); return *this;
		}

		template<int N>
		crstl_constexpr basic_string& assign(T(&char_array)[N]) crstl_noexcept
		{
			clear(); assign(char_array); return *this;
		}

		template<typename T>
		crstl_constexpr basic_string& assign(T string, crstl_is_char_ptr(T)) crstl_noexcept
		{
			clear(); append(string); return *this;
		}

		crstl_constexpr basic_string& assign(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			clear(); append(begin, end); return *this;
		}

		crstl_constexpr basic_string& assign(const basic_string& string) crstl_noexcept
		{
			clear(); append(string); return *this;
		}

		crstl_constexpr basic_string& assign(const basic_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			clear(); append(string, subpos, sublen); return *this;
		}

		crstl_constexpr basic_string& assign(size_t n, value_type c) crstl_noexcept
		{
			clear(); append(n, c); return *this;
		}

		crstl_constexpr reference back() crstl_noexcept
		{
			crstl_assert(length() > 0);
			return data()[length() - 1];
		}

		crstl_constexpr const_reference back() const crstl_noexcept
		{
			crstl_assert(length() > 0);
			return data()[length() - 1];
		}

		crstl_constexpr iterator begin() crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
		crstl_constexpr const_iterator begin() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }

		crstl_constexpr const_pointer c_str() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }

		crstl_constexpr size_t capacity() const
		{
			return is_sso() ? kSSOCapacity : get_capacity_heap();
		}

		crstl_constexpr const_iterator cbegin() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
		crstl_constexpr const_iterator cend() const crstl_noexcept
		{
			return is_sso() ? 
				m_layout_allocator.m_first.m_sso.data + (kSSOCapacity - m_layout_allocator.m_first.m_sso.remaining_length.value) : 
				m_layout_allocator.m_first.m_heap.data + m_layout_allocator.m_first.m_heap.length;
		}

		// Clearing just sets the string as empty, but does not perform a deallocation
		// It also doesn't change between SSO and heap
		crstl_constexpr void clear()
		{
			if (is_sso())
			{
				m_layout_allocator.m_first.m_sso.data[0] = 0;
				m_layout_allocator.m_first.m_sso.remaining_length.value = kSSOCapacity;
			}
			else
			{
				m_layout_allocator.m_first.m_heap.data[0] = 0;
				m_layout_allocator.m_first.m_heap.length = 0;
			}
		}

		//--------
		// compare
		//--------

		crstl_constexpr int compare(const T* string) const crstl_noexcept
		{
			return crstl::string_compare(data(), length(), string, string_length(string));
		}

		crstl_constexpr int compare(size_t pos, size_t length, const T* string) const crstl_noexcept
		{
			crstl_assert(pos < basic_string::length());
			return crstl::string_compare(data() + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int compare(size_t pos, size_t length, const T* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			crstl_assert(pos < basic_string::length());
			return crstl::string_compare(data() + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int compare(const basic_string& string) const crstl_noexcept
		{
			return crstl::string_compare(data(), length(), string.data(), string.length());
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_string& string) const crstl_noexcept
		{
			crstl_assert(pos < basic_string::length());
			return crstl::string_compare(data() + pos, clamp_length(pos, length), string.data(), string.length());
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_string& string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			crstl_assert(pos < basic_string::length());
			return crstl::string_compare(data() + pos, clamp_length(pos, length), string.data() + subpos, string.clamp_length(subpos, sublen));
		}

		crstl_constexpr pointer data() crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
		crstl_constexpr const_pointer data() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }

		crstl_nodiscard
		crstl_constexpr bool empty() const crstl_noexcept { return length() == 0; }

		crstl_constexpr iterator end() crstl_noexcept
		{
			return is_sso() ?
				m_layout_allocator.m_first.m_sso.data + (kSSOCapacity - m_layout_allocator.m_first.m_sso.remaining_length.value) :
				m_layout_allocator.m_first.m_heap.data + m_layout_allocator.m_first.m_heap.length;
		}

		crstl_constexpr const_iterator end() const crstl_noexcept
		{
			return is_sso() ?
				m_layout_allocator.m_first.m_sso.data + (kSSOCapacity - m_layout_allocator.m_first.m_sso.remaining_length.value) :
				m_layout_allocator.m_first.m_heap.data + m_layout_allocator.m_first.m_heap.length;
		}

		//-----
		// find
		//-----

		// Find a character
		crstl_constexpr size_t find(value_type c, size_t pos = 0) const crstl_noexcept
		{
			crstl_assert(pos <= length());
			const_pointer ptr = (const_pointer)crstl::string_find_char(data() + pos, c, length() - pos);
			return ptr ? (size_t)(ptr - data()) : npos;
		}

		// Find a const char* string with an offset and a length
		crstl_constexpr size_t find(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			crstl_assert(pos <= length());
			const_pointer found_string = crstl::string_find(data() + pos, length() - pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - data()) : npos;
		}

		crstl_constexpr size_t find(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr size_t find(const basic_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string.data(), pos, needle_string.length());
		}

		//--------------
		// find_first_of
		//--------------

		crstl_constexpr size_t find_first_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			crstl_assert(pos < length());
			const_pointer found_string = crstl::string_find_of(data() + pos, length() - pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - data()) : npos;
		}

		crstl_constexpr size_t find_first_of(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr size_t find_first_of(const basic_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string.c_str(), pos, needle_string.length());
		}

		crstl_constexpr size_t find_first_of(value_type c, size_t pos = 0) const crstl_noexcept
		{
			return find(c, pos);
		}

		//-------------
		// find_last_of
		//-------------

		crstl_constexpr size_t find_last_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			pos = pos < length() ? pos : length();
			const_pointer found_string = crstl::string_rfind_of(data() + pos, pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - data()) : npos;
		}

		crstl_constexpr size_t find_last_of(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr size_t find_last_of(const basic_string& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string.c_str(), pos, needle_string.length());
		}

		crstl_constexpr size_t find_last_of(value_type c, size_t pos = npos) const crstl_noexcept
		{
			return rfind(c, pos);
		}

		crstl_constexpr reference front() crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
		crstl_constexpr const_reference front() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }

		crstl_constexpr size_t length() const crstl_noexcept { return is_sso() ? length_sso() : length_heap(); }

		crstl_constexpr size_t max_size() const crstl_noexcept { return kHeapCapacityMask; }

		crstl_constexpr void pop_back() crstl_noexcept
		{
			crstl_assert(length() > 0);

			if (is_sso())
			{
				m_layout_allocator.m_first.m_sso.remaining_length.value++;
			}
			else
			{
				m_layout_allocator.m_first.m_heap.length--;
			}
		}

		crstl_constexpr reference push_back(value_type c) { append(1, c); return back(); }

		//--------
		// replace
		//--------

		//-------
		// resize
		//-------

		crstl_constexpr void resize(size_t length)
		{
			resize(length, 0);
		}

		crstl_constexpr void resize(size_t length, T c)
		{
			size_t current_length = basic_string::length();

			// If length is larger than current length, initialize new characters to 0
			if (current_length < length)
			{
				append_function(length, [c](T* begin, T* end)
				{
					while (begin != end)
					{
						*begin = c;
						++begin;
					}
				});
			}
			
			if (is_sso())
			{
				set_length_sso(length);
				m_layout_allocator.m_first.m_sso.data[length] = 0;
			}
			else
			{
				set_length_heap(length);
				m_layout_allocator.m_first.m_heap.data[length] = 0;
			}
		}

		//-----
		// rfind
		//-----

		size_t rfind(value_type c, size_t pos = npos) const crstl_noexcept
		{
			size_t length = basic_string::length();
			pointer data = basic_string::data();
			pos = pos < length ? pos : length;
			const_pointer ptr = (const_pointer)string_rfind_char(data + pos, c, pos);
			return ptr ? (size_t)(ptr - data) : npos;
		}

		// Find a const char* string with an offset and a length
		size_t rfind(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			size_t length = basic_string::length();
			pointer data = basic_string::data();
			crstl_assert(pos <= length);
			pos = pos < length ? pos : length;
			const_pointer found_string = crstl::string_rfind(data + pos, length - pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - data) : npos;
		}

		size_t rfind(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return rfind(needle_string, pos, string_length(needle_string));
		}

		size_t rfind(const basic_string& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return rfind(needle_string.m_data, pos, needle_string.m_length);
		}

		crstl_constexpr size_t size() const crstl_noexcept { return is_sso() ? length_sso() : length_heap(); }

		//------------
		// starts_with
		//------------

		crstl_constexpr bool starts_with(value_type c) const crstl_noexcept { return find(c, 0) == 0; }

		crstl_constexpr size_t starts_with(const_pointer needle_string, size_t needle_length) const crstl_noexcept
		{
			return find(needle_string, 0, needle_length) == 0;
		}

		crstl_constexpr size_t starts_with(const_pointer needle_string) const crstl_noexcept
		{
			return find(needle_string, 0, string_length(needle_string)) == 0;
		}

		crstl_constexpr size_t starts_with(const basic_string& needle_string) const crstl_noexcept
		{
			return find(needle_string.m_data, 0, needle_string.m_length) == 0;
		}

		crstl_constexpr basic_string substr(size_t pos, size_t length = npos) const crstl_noexcept
		{
			return basic_string(*this, pos, length);
		}

		//----------
		// Operators
		//----------

		crstl_constexpr reference operator [](size_t i) crstl_noexcept
		{
			crstl_assert(i < length());
			return data()[i];
		}

		crstl_constexpr const_reference operator [](size_t i) const crstl_noexcept
		{
			crstl_assert(i < length());
			return data()[i];
		}

		crstl_constexpr basic_string& operator += (const basic_string& string) crstl_noexcept
		{
			append(string);
			return *this;
		}

		crstl_constexpr basic_string& operator += (const_pointer string) crstl_noexcept
		{
			append(string);
			return *this;
		}

		crstl_constexpr bool operator == (const_pointer string) const crstl_noexcept { return compare(string) == 0; }
		crstl_constexpr bool operator != (const_pointer string) const crstl_noexcept { return compare(string) != 0; }
		crstl_constexpr bool operator == (const basic_string& string) const crstl_noexcept { return compare(string) == 0; }
		crstl_constexpr bool operator != (const basic_string& string) const crstl_noexcept { return compare(string) != 0; }

	private:

		size_t compute_new_capacity(size_t capacity)
		{
			return capacity;
		}

		void reallocate_heap_larger(size_t new_capacity)
		{
			// This ensure we pass in a capacity that is larger than the SSO buffer (it wouldn't make sense to allocate otherwise)
			// and larger than the existing heap capacity too
			crstl_assert(new_capacity > capacity());

			T* temp = m_layout_allocator.second().allocate(new_capacity + 1);

			// Copy existing data from current source
			if (is_sso())
			{
				memcpy(temp, m_layout_allocator.m_first.m_sso.data, (length_sso() + 1) * kCharSize);
			}
			else
			{
				memcpy(temp, m_layout_allocator.m_first.m_heap.data, (length_heap() + 1) * kCharSize);
				m_layout_allocator.second().deallocate(m_layout_allocator.m_first.m_heap.data, get_capacity_heap() + 1);
			}

			m_layout_allocator.m_first.m_heap.data = temp;
			set_capacity_heap(new_capacity);
		}

		// As we need to reallocate and the logic can be a bit convoluted, we'll
		// provide a function here that deals with all that and calls a function
		// with what we need to do
		template<typename Function>
		void append_function(size_t length, const Function& function)
		{
			size_t current_length = basic_string::length();
			size_t target_length = current_length + length;

			if (target_length < kSSOCapacity)
			{
				T* begin = m_layout_allocator.m_first.m_sso.data + current_length;
				function(begin, begin + length);
				m_layout_allocator.m_first.m_sso.remaining_length.value -= (char)length;
				m_layout_allocator.m_first.m_sso.data[target_length] = 0;
			}
			else
			{
				// If we're currently in sso mode we assume a capacity 
				size_t heap_capacity = is_sso() ? 0 : get_capacity_heap();

				// If target length is greater than existing heap capacity, reallocate and copy
				if (target_length > heap_capacity)
				{
					reallocate_heap_larger(target_length);
				}

				T* begin = m_layout_allocator.m_first.m_heap.data + current_length;
				function(begin, begin + length);
				m_layout_allocator.m_first.m_heap.length = target_length;
				m_layout_allocator.m_first.m_heap.data[target_length] = 0;
			}
		}

		void initialize_string(const_pointer string, size_t length)
		{
			if (length < kSSOCapacity)
			{
				memcpy(m_layout_allocator.m_first.m_sso.data, string, length * kCharSize);
				m_layout_allocator.m_first.m_sso.remaining_length.value = (char)(kSSOCapacity - length);
				m_layout_allocator.m_first.m_sso.data[length] = 0;
			}
			else
			{
				m_layout_allocator.m_first.m_heap.data = allocate_heap(length);
				memcpy(m_layout_allocator.m_first.m_heap.data, string, length * kCharSize);
				m_layout_allocator.m_first.m_heap.length = length;
				m_layout_allocator.m_first.m_heap.data[length] = 0;
			}
		}

		// Given a position and a length, return the length that fits the string
		crstl_constexpr size_t clamp_length(size_t pos, size_t length) const crstl_noexcept
		{
			return crstl::string_clamp_length(basic_string::length(), pos, length);
		}

		bool is_sso() const
		{
			return (m_layout_allocator.m_first.m_sso.remaining_length.value & kSSOMask) == 0;
		}

		bool is_heap() const
		{
			return  (m_layout_allocator.m_first.m_sso.remaining_length.value & kSSOMask) != 0;
		}

		size_t length_sso() const
		{
			return kSSOCapacity - m_layout_allocator.m_first.m_sso.remaining_length.value;
		}

		size_t length_heap() const
		{
			return m_layout_allocator.m_first.m_heap.length;
		}

		void set_length_sso(size_t length)
		{
			m_layout_allocator.m_first.m_sso.remaining_length.value = (char)(kSSOCapacity - length);
		}

		void set_length_heap(size_t length)
		{
			m_layout_allocator.m_first.m_heap.length = length;
		}

		size_t get_capacity_heap() const
		{
			return m_layout_allocator.m_first.m_heap.capacity & kHeapCapacityMask;
		}

		void set_capacity_heap(size_t capacity)
		{
			m_layout_allocator.m_first.m_heap.capacity = capacity | ~kHeapCapacityMask;
		}

		// Assume allocate and deallocate always add a +1 for the null terminator
		T* allocate_heap(size_t capacity)
		{
			T* temp = m_layout_allocator.second().allocate(capacity + 1);
			set_capacity_heap(capacity);
			return temp;
		}

		void deallocate_heap()
		{
			m_layout_allocator.second().deallocate(m_layout_allocator.m_first.m_heap.data, get_capacity_heap() + 1);
			set_capacity_heap(0);
			m_layout_allocator.m_first.m_heap.data = nullptr;
		}

		crstl::compressed_pair<layout, Allocator> m_layout_allocator;
	};

	typedef basic_string<char, crstl::allocator<char>> string;
	typedef basic_string<wchar_t, crstl::allocator<wchar_t>> wstring;
};