#pragma once

#include "crstl/config.h"

#include "crstl/allocator.h"

#include "crstl/compressed_pair.h"

#include "crstl/utility/string_common.h"

#include "crstl/utility/string_utf.h"

#include "crstl/utility/memory_ops.h"

// crstl::string
//
// Replacement for std::string
//
// - There are extra functions not present in the standard such as
//   - constructors and functions taking string literals and char arrays (avoids calling strlen)
//   - explicit constructors for creating a string from an integer or float
//   - append_convert: append converting from different characters representations
//   - assign_convert: assign converting from different characters representations
//   - append_sprintf: Use sprintf-like formatting to append string
//   - comparei: compare ignoring case
//   - erase_all: erase all occurrences of a given string. This is more efficient than calling find and erase in a loop because there are fewer copies
//   - replace_all: replaces all occurrences of needle with replace. Note that replace_all(char, char) doesn't work with unicode strings
//   - resize_uninitialized(length): resize string but don't initialize contents. Using c_str() right after is undefined behavior. Useful when populating
//   string from an external source

crstl_module_export namespace crstl
{
	namespace detail
	{
		// Insert padding when sizeof(CharT) > 1
		template<int N> struct sso_padding { char padding[N]; };
		template<> struct sso_padding<0> {};
	};

	template<typename CharT, typename Allocator = crstl::allocator>
	class basic_string
	{
	public:

		typedef CharT        value_type;
		typedef CharT&       reference;
		typedef const CharT& const_reference;
		typedef CharT*       pointer;
		typedef const CharT* const_pointer;
		typedef CharT*       iterator;
		typedef const CharT* const_iterator;
		typedef size_t       size_type;

		// View of heap-allocated string
		struct heap_view
		{
			CharT* data;
			size_t length;
			size_t capacity;
		};

		struct sso_size : detail::sso_padding<sizeof(CharT) - 1>
		{
			unsigned char value;
		};

		// View of string acting as small-string optimization
		struct sso_view
		{
			CharT data[sizeof(heap_view) / sizeof(CharT) - 1];
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

		static const crstl_constexpr size_t kCharSize = sizeof(CharT);

		static const crstl_constexpr size_t kSSOCapacity = sizeof(sso_view) / sizeof(CharT) - 1;

		// This is valid for little endian machines, where the remaining_length and capacity
		// share the same space in memory
		static const crstl_constexpr size_t kSSOMask = 0x80;

		static const crstl_constexpr size_t kHeapCapacityMask = ((size_t)-1) >> 1;

		static const crstl_constexpr size_t npos = (size_t)-1;

		//-------------
		// Constructors
		//-------------

		crstl_constexpr14 basic_string() crstl_noexcept
		{
			m_layout_allocator.m_first.m_sso.data[0] = 0;

			// Some compilers will think this is uninitialized because some branches may take it. We'll go with simple and initialize to 0
			// even though we knows it's not really needed, and avoid removing warnings, etc
			m_layout_allocator.m_first.m_heap.length = 0;
			m_layout_allocator.m_first.m_sso.remaining_length.value = kSSOCapacity;
		}

		crstl_constexpr14 basic_string(const_pointer string, size_t length) crstl_noexcept
		{
			crstl_assert(string != nullptr);
			initialize_string(string, length);
		}

		template<int N>
		crstl_constexpr14 basic_string(const CharT(&string_literal)[N]) crstl_noexcept
		{
			initialize_string(string_literal, string_length(string_literal, N - 1));
		}

		template<typename Q>
		crstl_constexpr14 basic_string(Q string, crstl_is_char_ptr(Q)) crstl_noexcept
		{
			initialize_string(string, string_length(string));
		}

		crstl_constexpr14 basic_string(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			initialize_string(begin, end - begin);
		}

		crstl_constexpr14 basic_string(const basic_string& string) crstl_noexcept
		{
			initialize_string(string.c_str(), string.size());
		}

		crstl_constexpr14 basic_string(basic_string&& string) crstl_noexcept
		{
			m_layout_allocator = string.m_layout_allocator;
			string.m_layout_allocator.first().m_heap.data = nullptr; // Don't try to deallocate
		}

		crstl_constexpr14 basic_string(ctor_concatenate, const basic_string& string1, const basic_string& string2) crstl_noexcept : basic_string()
		{
			reserve(string1.size() + string2.size());
			initialize_string(string1.c_str(), string1.size());
			append(string2);
		}

		template<int N>
		crstl_constexpr14 basic_string(ctor_concatenate, const basic_string& string, const CharT(&string_literal)[N]) crstl_noexcept : basic_string()
		{
			reserve(string.size() + N);
			initialize_string(string.c_str(), string.size());
			append(string_literal);
		}

		template<typename Q>
		crstl_constexpr14 basic_string(ctor_concatenate, const basic_string& string1, Q string2, crstl_is_char_ptr(Q)) crstl_noexcept : basic_string()
		{
			size_t string2_length = string_length(string2);
			reserve(string1.size() + string2_length);
			initialize_string(string1.c_str(), string1.size());
			append(string2, string2_length);
		}

		crstl_constexpr14 basic_string(ctor_concatenate, const CharT* string1, const basic_string& string2) crstl_noexcept : basic_string()
		{
			size_t string1_length = string_length(string1);
			reserve(string1_length + string2.size());
			initialize_string(string1, string1_length);
			append(string2);
		}

		crstl_constexpr14 basic_string(const basic_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			size_t string_length = string.length();

			crstl_assert(subpos <= string_length);

			if(sublen > string_length - subpos)
			{
				sublen = string_length - subpos;
			}

			initialize_string(string.c_str() + subpos, sublen);
		}

		crstl_constexpr14 basic_string(size_t n, CharT c) crstl_noexcept : basic_string()
		{
			append(n, c);
		}

		crstl_constexpr14 basic_string(ctor_no_initialize, size_t capacity, size_t length = 0) : basic_string()
		{
			crstl_assert(length <= capacity);

			if (capacity < kSSOCapacity)
			{
				m_layout_allocator.m_first.m_sso.remaining_length.value = (char)(kSSOCapacity - length);
				m_layout_allocator.m_first.m_sso.data[length] = 0;
			}
			else
			{
				m_layout_allocator.m_first.m_heap.data = allocate_heap(capacity);
				m_layout_allocator.m_first.m_heap.length = length;
				m_layout_allocator.m_first.m_heap.data[length] = 0;
			}
		}

		explicit crstl_constexpr14 basic_string(int value)                crstl_noexcept : basic_string() { append_sprintf("%d", value); }
		explicit crstl_constexpr14 basic_string(long value)               crstl_noexcept : basic_string() { append_sprintf("%ld", value); }
		explicit crstl_constexpr14 basic_string(long long value)          crstl_noexcept : basic_string() { append_sprintf("%lld", value); }
		explicit crstl_constexpr14 basic_string(unsigned value)           crstl_noexcept : basic_string() { append_sprintf("%u", value); }
		explicit crstl_constexpr14 basic_string(unsigned long value)      crstl_noexcept : basic_string() { append_sprintf("%lu", value); }
		explicit crstl_constexpr14 basic_string(unsigned long long value) crstl_noexcept : basic_string() { append_sprintf("%llu", value); }
		explicit crstl_constexpr14 basic_string(float value)              crstl_noexcept : basic_string() { append_sprintf("%f", value); }
		explicit crstl_constexpr14 basic_string(double value)             crstl_noexcept : basic_string() { append_sprintf("%f", value); }
		explicit crstl_constexpr14 basic_string(long double value)        crstl_noexcept : basic_string() { append_sprintf("%Lf", value); }

		~basic_string() crstl_noexcept
		{
			if (is_heap())
			{
				deallocate_heap();
			}
		}

		crstl_constexpr14 CharT& at(size_t i) crstl_noexcept
		{
			return crstl_assert(i < length()), data()[i];
		}

		crstl_constexpr const CharT& at(size_t i) const crstl_noexcept
		{
			return crstl_assert(i < length()), data()[i];
		}

		//-------
		// append
		//-------

		crstl_constexpr14 basic_string& append(const_pointer string, size_t length)
		{
			append_function(length, [string, length](CharT* begin, CharT* /*end*/)
			{
				memory_copy(begin, string, length * kCharSize);
			});

			return *this;
		}

		template<int N>
		crstl_constexpr14 basic_string& append(const CharT(&string_literal)[N]) crstl_noexcept
		{
			append(string_literal, string_length(string_literal, N - 1)); return *this;
		}

		template<typename Q>
		crstl_constexpr14 basic_string& append(Q string, crstl_is_char_ptr(Q))
		{
			return append(string, string_length(string));
		}

		crstl_constexpr14 basic_string& append(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			append(begin, (size_t)(end - begin));
			return *this;
		}

		crstl_constexpr14 basic_string& append(const basic_string& string) crstl_noexcept
		{
			append(string.data(), string.length()); return *this;
		}

		crstl_constexpr14 basic_string& append(size_t n, CharT c) crstl_noexcept
		{
			append_function(n, [c](CharT* begin, CharT* end)
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

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& append_convert(const OtherCharT* string, size_t length) crstl_noexcept
		{
			size_t current_length = basic_string::length();

			// This target capacity is an approximation as we cannot know what the final capacity is going to be.
			// This is because utf-8 is variable-length encoding. If we know the target capacity is above the sso
			// we won't even try to store it there
			size_t target_capacity = current_length + length;
			size_t current_capacity = basic_string::capacity();
			size_t dst_decoded_length = 0;
			size_t src_decoded_length = 0;
			bool success = false;
			bool reallocate = false;

			// Try to append into the sso buffer. Maybe we append a small part of it but conclude it didn't fit entirely (because e.g. UTF-8 is variable length)
			// We then proceed to reallocate, copy the decoded data and continue decoding with an estimation of what we think is the data we need

			if (target_capacity < kSSOCapacity)
			{
				CharT* data = m_layout_allocator.m_first.m_sso.data;
				utf_result::t result = decode_chunk(data + current_length, data + kSSOCapacity, string, string + length, dst_decoded_length, src_decoded_length);

				switch (result)
				{
					case utf_result::invalid:
						return *this;
					case utf_result::success:
						success = true;
						break;
					case utf_result::no_memory:
						target_capacity = kSSOCapacity + 1; // Force reallocation
						break;
				}

				current_length += dst_decoded_length; // Add however many characters we managed to decode
				m_layout_allocator.m_first.m_sso.data[current_length] = 0;
				m_layout_allocator.m_first.m_sso.remaining_length.value = (unsigned char)(kSSOCapacity - current_length);
			}

			// Assume from here on that we're going to work with the heap

			if (target_capacity > current_capacity)
			{
				reallocate = true;
			}

			while (!success)
			{
				// Reallocate and prepare to continue decoding
				if (reallocate)
				{
					target_capacity = reallocate_heap_larger(target_capacity);
				}

				// Decode as much as we can with our estimation of the space we need for the string
				size_t iter_src_decoded_length = 0;
				size_t iter_dst_decoded_length = 0;
				CharT* dst_start = m_layout_allocator.m_first.m_heap.data + current_length;
				CharT* dst_end = m_layout_allocator.m_first.m_heap.data + target_capacity;
				const OtherCharT* src_start = string + src_decoded_length;
				const OtherCharT* src_end = string + length;
				utf_result::t result = decode_chunk(dst_start, dst_end, src_start, src_end, iter_dst_decoded_length, iter_src_decoded_length);

				// If we detect an invalid decoding, return immediately
				if (result == utf_result::invalid)
				{
					return *this;
				}

				success = (result == utf_result::success);
				current_length += iter_dst_decoded_length;
				src_decoded_length += iter_src_decoded_length;

				m_layout_allocator.m_first.m_heap.length = current_length;
				m_layout_allocator.m_first.m_heap.data[current_length] = 0;

				reallocate = true;
			}

			return *this;
		}

		template<typename OtherCharT, int N>
		crstl_constexpr14 basic_string& append_convert(const OtherCharT(&string_literal)[N]) crstl_noexcept
		{
			append_convert(string_literal, string_length(string_literal, N - 1)); return *this;
		}

		template<typename OtherCharQ>
		crstl_constexpr14 basic_string& append_convert(OtherCharQ string, crstl_is_char_ptr(OtherCharQ)) crstl_noexcept
		{
			append_convert(string, string_length(string));
			return *this;
		}

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& append_convert(const OtherCharT* begin, const OtherCharT* end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			append_convert(begin, (size_t)(end - begin));
			return *this;
		}

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& append_convert(const basic_string<OtherCharT>& string) crstl_noexcept
		{
			append_convert(string.data(), string.length());
			return *this;
		}

		//---------------
		// append_sprintf
		//---------------
		
		// Append a const char* string with a provided length
		crstl_constexpr14 basic_string& append_sprintf(const_pointer format, ...) crstl_noexcept
		{
			size_t current_length = length();
			size_t remaining_length = capacity() - current_length;

			CharT* data = basic_string::data();

			va_list va_arguments {};
			va_start(va_arguments, format);

			// Try to copy, limiting the number of characters to what we have available
			int snprintf_return = vsnprintf(data + current_length, remaining_length + 1, format, va_arguments);

			va_end(va_arguments);

			// It is a formatting error to return a negative number
			crstl_assert(snprintf_return >= 0);

			size_t char_count = (size_t)snprintf_return;

			// If the number of characters we would have written is greater than what we had available, 
			// we need to heap reallocate to have enough space, then try again. We would actually like
			// this to continue where it left off but that's not possible with format specifiers
			if (char_count > remaining_length)
			{
				reallocate_heap_larger(current_length + char_count); // When we reallocate, we always take the null terminator into account
				va_start(va_arguments, format);

				// We need to pass in char_count + 1 as it will try to leave space for the null terminator and we'd end up with one less character
				snprintf_return = vsnprintf(m_layout_allocator.m_first.m_heap.data + current_length, char_count + 1u, format, va_arguments);

				va_end(va_arguments);
				crstl_assert(snprintf_return >= 0);

				char_count = (size_t)snprintf_return;
			}

			if (is_sso())
			{
				m_layout_allocator.m_first.m_sso.remaining_length.value = (unsigned char)(kSSOCapacity - char_count);
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

		crstl_constexpr14 basic_string& assign(const_pointer string, size_t length) crstl_noexcept
		{
			clear(); append(string, length); return *this;
		}

		template<int N>
		crstl_constexpr14 basic_string& assign(const CharT(&string_literal)[N]) crstl_noexcept
		{
			clear(); append(string_literal); return *this;
		}

		template<typename Q>
		crstl_constexpr14 basic_string& assign(Q string, crstl_is_char_ptr(Q)) crstl_noexcept
		{
			clear(); append(string); return *this;
		}

		crstl_constexpr14 basic_string& assign(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			clear(); append(begin, end); return *this;
		}

		crstl_constexpr14 basic_string& assign(const basic_string& string) crstl_noexcept
		{
			clear(); append(string); return *this;
		}

		crstl_constexpr14 basic_string& assign(const basic_string& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			clear(); append(string, subpos, sublen); return *this;
		}

		//---------------
		// assign_convert
		//---------------

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& assign_convert(const OtherCharT* string, size_t length) crstl_noexcept
		{
			clear(); append_convert(string, length); return *this;
		}

		template<typename OtherCharT, int N>
		crstl_constexpr14 basic_string& assign_convert(const OtherCharT(&string_literal)[N]) crstl_noexcept
		{
			clear(); append_convert(string_literal); return *this;
		}

		template<typename OtherCharQ>
		crstl_constexpr14 basic_string& assign_convert(OtherCharQ string, crstl_is_char_ptr(OtherCharQ)) crstl_noexcept
		{
			clear(); append_convert(string); return *this;
		}

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& assign_convert(const OtherCharT* begin, const OtherCharT* end) crstl_noexcept
		{
			crstl_assert(end >= begin);
			clear(); append_convert(begin, end); return *this;
		}

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& assign_convert(const basic_string<OtherCharT>& string) crstl_noexcept
		{
			clear(); append_convert(string); return *this;
		}

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& assign_convert(const basic_string<OtherCharT>& string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			clear(); append_convert(string, subpos, sublen); return *this;
		}

		template<typename OtherCharT>
		crstl_constexpr14 basic_string& assign_convert(size_t n, OtherCharT c) crstl_noexcept
		{
			clear(); append_convert(n, c); return *this;
		}

		crstl_constexpr14 CharT& back() crstl_noexcept
		{
			return crstl_assert(length() > 0), data()[length() - 1];
		}

		crstl_constexpr const CharT& back() const crstl_noexcept
		{
			return crstl_assert(length() > 0), data()[length() - 1];
		}

		crstl_constexpr14 iterator begin() crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
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
		crstl_constexpr14 void clear()
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

		crstl_constexpr int compare(const CharT* string) const crstl_noexcept
		{
			return crstl::string_compare(data(), length(), string, string_length(string));
		}

		crstl_constexpr int compare(const CharT* string, size_t string_length) const crstl_noexcept
		{
			return crstl::string_compare(data(), length(), string, string_length);
		}

		crstl_constexpr int compare(size_t pos, size_t length, const CharT* string) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_compare(data() + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int compare(size_t pos, size_t length, const CharT* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_compare(data() + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int compare(const basic_string& string) const crstl_noexcept
		{
			return crstl::string_compare(data(), length(), string.data(), string.length());
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_string& string) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_compare(data() + pos, clamp_length(pos, length), string.data(), string.length());
		}

		crstl_constexpr int compare(size_t pos, size_t length, const basic_string& string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_compare(data() + pos, clamp_length(pos, length), string.data() + subpos, string.clamp_length(subpos, sublen));
		}

		//---------
		// comparei
		//---------

		crstl_constexpr int comparei(const CharT* string) const crstl_noexcept
		{
			return crstl::string_comparei(data(), length(), string, string_length(string));
		}

		crstl_constexpr int comparei(const CharT* string, size_t string_length) const crstl_noexcept
		{
			return crstl::string_comparei(data(), length(), string, string_length);
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const CharT* string) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_comparei(data() + pos, clamp_length(pos, length), string, string_length(string));
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const CharT* string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_comparei(data() + pos, clamp_length(pos, length), string + subpos, crstl::string_clamp_length(string_length(string), subpos, sublen));
		}

		crstl_constexpr int comparei(const basic_string& string) const crstl_noexcept
		{
			return crstl::string_comparei(data(), length(), string.data(), string.length());
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const basic_string& string) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_comparei(data() + pos, clamp_length(pos, length), string.data(), string.length());
		}

		crstl_constexpr int comparei(size_t pos, size_t length, const basic_string& string, size_t subpos, size_t sublen = npos) const crstl_noexcept
		{
			return crstl_assert(pos < basic_string::length()), crstl::string_comparei(data() + pos, clamp_length(pos, length), string.data() + subpos, string.clamp_length(subpos, sublen));
		}

		crstl_constexpr14 pointer data() crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
		crstl_constexpr const_pointer data() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }

		crstl_nodiscard
		crstl_constexpr bool empty() const crstl_noexcept { return length() == 0; }

		crstl_constexpr14 iterator end() crstl_noexcept
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

		//------
		// erase
		//------

		crstl_constexpr14 basic_string& erase(size_t pos = 0, size_t length = npos) crstl_noexcept
		{
			crstl_assert(length > 0);

			size_t current_length = basic_string::length();
			CharT* data = basic_string::data();

			length = length < current_length - pos ? length : current_length - pos;

			pointer dst = data + pos;
			pointer src = data + pos + length;

			// Copy data from the position to the current length
			for (size_t i = 0; i < current_length - pos; ++i)
			{
				dst[i] = src[i];
			}

			// Adjust the length
			if (is_sso())
			{
				m_layout_allocator.m_first.m_sso.remaining_length.value += (unsigned char)length;
				m_layout_allocator.m_first.m_sso.data[kSSOCapacity - m_layout_allocator.m_first.m_sso.remaining_length.value] = 0;
			}
			else
			{
				m_layout_allocator.m_first.m_heap.length -= length;
				m_layout_allocator.m_first.m_heap.data[m_layout_allocator.m_first.m_heap.length] = 0;
			}

			return *this;
		}

		crstl_constexpr14 basic_string& erase(const_pointer begin) crstl_noexcept
		{
			crstl_assert(data() <= begin && begin <= data() + length());
			return erase((size_t)(begin - data()), 1);
		}

		crstl_constexpr14 basic_string& erase(const_pointer begin, const_pointer end) crstl_noexcept
		{
			crstl_assert(end >= begin && data() <= begin && begin <= data() + length());
			return erase((size_t)(begin - data()), (size_t)(end - begin));
		}

		//----------
		// erase_all
		//----------

		crstl_constexpr14 basic_string& erase_all(CharT c, size_t pos = 0)
		{
			size_t current_length = length();
			pointer current_data = data();
			crstl_assert(pos <= current_length);

			const_pointer data_end = crstl::erase_all(current_data, current_length, pos, c);

			if (data_end)
			{
				set_length_and_terminator((size_t)(data_end - current_data));
			}

			return *this;
		}

		crstl_constexpr14 basic_string& erase_all(const_pointer needle_string, size_t pos, size_t needle_length)
		{
			size_t current_length = length();
			pointer current_data = data();
			crstl_assert(pos <= current_length);

			const_pointer data_end = crstl::erase_all(current_data, current_length, pos, needle_string, needle_length);

			if (data_end)
			{
				set_length_and_terminator((size_t)(data_end - current_data));
			}

			return *this;
		}

		crstl_constexpr14 basic_string& erase_all(const_pointer needle_string, size_t pos = 0) crstl_noexcept
		{
			return erase_all(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr14 basic_string& erase_all(const basic_string& needle_string, size_t pos = 0) crstl_noexcept
		{
			return erase_all(needle_string.data(), pos, needle_string.length());
		}

		//-----
		// find
		//-----

		// Find a character
		crstl_constexpr14 size_t find(CharT c, size_t pos = 0) const crstl_noexcept
		{
			size_t current_length = length();
			const_pointer current_data = data();

			if (pos < current_length)
			{
				const_pointer ptr = crstl::string_find_char(current_data + pos, c, current_length - pos);
				return ptr ? (size_t)(ptr - current_data) : npos;
			}
			else
			{
				return npos;
			}
		}

		// Find a const char* string with an offset and a length
		crstl_constexpr14 size_t find(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			size_t current_length = length();
			const_pointer current_data = data();

			if (pos < current_length)
			{
				const_pointer found_string = crstl::string_find(current_data + pos, current_length - pos, needle_string, needle_length);
				return found_string ? (size_t)(found_string - current_data) : npos;
			}
			else
			{
				return npos;
			}
		}

		crstl_constexpr14 size_t find(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr14 size_t find(const basic_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find(needle_string.data(), pos, needle_string.length());
		}

		//--------------
		// find_first_of
		//--------------

		crstl_constexpr14 size_t find_first_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			size_t current_length = length();
			const_pointer current_data = data();

			if (pos < current_length)
			{
				const_pointer found_string = crstl::string_find_of(current_data + pos, current_length - pos, needle_string, needle_length);
				return found_string ? (size_t)(found_string - current_data) : npos;
			}
			else
			{
				return npos;
			}
		}

		crstl_constexpr14 size_t find_first_of(const_pointer needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr14 size_t find_first_of(const basic_string& needle_string, size_t pos = 0) const crstl_noexcept
		{
			return find_first_of(needle_string.c_str(), pos, needle_string.length());
		}

		crstl_constexpr14 size_t find_first_of(CharT c, size_t pos = 0) const crstl_noexcept
		{
			return find(c, pos);
		}

		//-------------
		// find_last_of
		//-------------

		crstl_constexpr14 size_t find_last_of(const_pointer needle_string, size_t pos, size_t needle_length) const crstl_noexcept
		{
			pos = pos < length() ? pos : length();
			const_pointer found_string = crstl::string_rfind_of(data() + pos, pos, needle_string, needle_length);
			return found_string ? (size_t)(found_string - data()) : npos;
		}

		crstl_constexpr14 size_t find_last_of(const_pointer needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string, pos, crstl::string_length(needle_string));
		}

		crstl_constexpr14 size_t find_last_of(const basic_string& needle_string, size_t pos = npos) const crstl_noexcept
		{
			return find_last_of(needle_string.c_str(), pos, needle_string.length());
		}

		crstl_constexpr14 size_t find_last_of(CharT c, size_t pos = npos) const crstl_noexcept
		{
			return rfind(c, pos);
		}

		crstl_constexpr14 CharT& front() crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }
		crstl_constexpr const CharT& front() const crstl_noexcept { return is_sso() ? m_layout_allocator.m_first.m_sso.data : m_layout_allocator.m_first.m_heap.data; }

		crstl_constexpr14 void force_length(size_t length) crstl_noexcept
		{
			crstl_assert(length < capacity());

			// Ensure there's at least a null terminator
			if(is_sso())
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

		crstl_constexpr size_t length() const crstl_noexcept { return is_sso() ? length_sso() : length_heap(); }

		crstl_constexpr size_t max_size() const crstl_noexcept { return kHeapCapacityMask; }

		crstl_constexpr14 void pop_back() crstl_noexcept
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

		crstl_constexpr14 reference push_back(CharT c) { append(1, c); return back(); }

		//--------
		// replace
		//--------

		crstl_constexpr14 basic_string& replace(size_t needle_pos, size_t needle_length, const_pointer replace_string, size_t replace_length)
		{
			crstl_assert(needle_pos + needle_length <= basic_string::length());
			CharT* data = replace_common(basic_string::length(), needle_pos, needle_length, replace_length);
			memory_copy(data + needle_pos, replace_string, replace_length * kCharSize);
			return *this;
		}

		crstl_constexpr14 basic_string& replace(size_t needle_pos, size_t needle_length, size_t n, CharT c)
		{
			crstl_assert(needle_pos + needle_length <= basic_string::length());
			CharT* data = replace_common(basic_string::length(), needle_pos, needle_length, n);
			crstl::fill_char(data + needle_pos, n, c);
			return *this;
		}

		crstl_constexpr14 basic_string& replace(size_t needle_pos, size_t needle_length, const_pointer replace_string)
		{
			return replace(needle_pos, needle_length, replace_string, string_length(replace_string));
		}

		crstl_constexpr14 basic_string& replace(size_t needle_pos, size_t needle_length, const basic_string& replace_string) crstl_noexcept
		{
			return replace(needle_pos, needle_length, replace_string.data(), replace_string.length());
		}

		crstl_constexpr14 basic_string& replace(size_t needle_pos, size_t needle_length, const basic_string& replace_string, size_t subpos, size_t sublen = npos) crstl_noexcept
		{
			sublen = sublen < replace_string.length() ? sublen : replace_string.length();
			return replace(needle_pos, needle_length, replace_string.data() + subpos, sublen);
		}

		crstl_constexpr14 basic_string& replace(const_pointer begin, const_pointer end, const_pointer replace_string) crstl_noexcept
		{
			crstl_assert(end >= begin);
			return replace((size_t)(begin - data()), (size_t)(end - begin), replace_string, crstl::string_length(replace_string));
		}

		crstl_constexpr14 basic_string& replace(const_pointer begin, const_pointer end, const_pointer replace_string, size_t replace_length) crstl_noexcept
		{
			crstl_assert(end >= begin);
			return replace((size_t)(begin - data()), (size_t)(end - begin), replace_string, replace_length);
		}

		crstl_constexpr14 basic_string& replace(const_pointer begin, const_pointer end, const basic_string& replace_string) crstl_noexcept
		{
			crstl_assert(end >= begin);
			return replace((size_t)(begin - data()), (size_t)(end - begin), replace_string.data(), (size_t)replace_string.length());
		}

		//------------
		// replace_all
		//------------

		crstl_constexpr14 basic_string& replace_all(CharT needle_c, CharT replace_c)
		{
			CharT* data = basic_string::data();

			for (size_t i = 0; i < length(); ++i)
			{
				if (data[i] == needle_c)
				{
					data[i] = replace_c;
				}
			}

			return *this;
		}

		//--------
		// reserve
		//--------

		crstl_constexpr14 void reserve(size_t capacity)
		{
			if (capacity > basic_string::capacity())
			{
				reallocate_heap_larger(capacity);
			}
		}

		//-------
		// resize
		//-------

		crstl_constexpr14 void resize(size_t length)
		{
			resize(length, 0);
		}

		crstl_constexpr14 void resize(size_t length, CharT c)
		{
			size_t current_length = basic_string::length();

			// If length is larger than current length, initialize new characters to 0
			if (current_length < length)
			{
				append_function(length, [c](CharT* begin, CharT* end)
				{
					while (begin != end)
					{
						*begin = c;
						++begin;
					}
				});
			}

			set_length_and_terminator(length);
		}

		crstl_constexpr14 void resize_uninitialized(size_t length)
		{
			if (length > basic_string::capacity())
			{
				reallocate_heap_larger(length);
			}

			// Force the length of the string without initializing
			set_length_and_terminator(length);
		}

		//-----
		// rfind
		//-----

		size_t rfind(CharT c, size_t pos = npos) const crstl_noexcept
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

		crstl_constexpr bool starts_with(CharT c) const crstl_noexcept { return find(c, 0) == 0; }

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
		// operators
		//----------

		crstl_constexpr14 basic_string& operator = (const basic_string& string) crstl_noexcept
		{
			clear();
			append(string);
			return *this;
		}

		crstl_constexpr14 basic_string& operator = (basic_string&& string) crstl_noexcept
		{
			m_layout_allocator = string.m_layout_allocator;
			string.m_layout_allocator.first().m_heap.data = nullptr; // Don't try to deallocate
			return *this;
		}

		crstl_constexpr14 CharT& operator [](size_t i) crstl_noexcept
		{
			return crstl_assert(i < length()), data()[i];
		}

		crstl_constexpr const CharT& operator [](size_t i) const crstl_noexcept
		{
			return crstl_assert(i < length()), data()[i];
		}

		crstl_constexpr14 basic_string& operator += (const basic_string& string) crstl_noexcept
		{
			append(string);
			return *this;
		}

		crstl_constexpr14 basic_string& operator += (const_pointer string) crstl_noexcept
		{
			append(string);
			return *this;
		}

		// operator +

		friend basic_string operator + (const basic_string& string1, const basic_string& string2)
		{
			return basic_string(ctor_concatenate(), string1, string2);
		}

		template<int N>
		friend basic_string operator + (const basic_string& string, const CharT(&string_literal)[N])
		{
			return basic_string(ctor_concatenate(), string, string_literal);
		}

		template<typename Q>
		friend crstl_return_is_char_ptr(Q, basic_string) operator + (const basic_string& string, Q const_char)
		{
			return basic_string(ctor_concatenate(), string, const_char);
		}
		
		template<int N>
		friend basic_string operator + (const CharT(&string_literal)[N], const basic_string& string)
		{
			return basic_string(ctor_concatenate(), string_literal, string);
		}
		
		template<typename Q>
		friend crstl_return_is_char_ptr(Q, basic_string) operator + (Q const_char, const basic_string& string)
		{
			return basic_string(ctor_concatenate(), const_char, string);
		}

		crstl_constexpr bool operator == (const_pointer string) const crstl_noexcept { return compare(string) == 0; }
		crstl_constexpr bool operator != (const_pointer string) const crstl_noexcept { return compare(string) != 0; }
		crstl_constexpr bool operator <  (const_pointer string) const crstl_noexcept { return compare(string) <  0; }
		crstl_constexpr bool operator <= (const_pointer string) const crstl_noexcept { return compare(string) <= 0; }
		crstl_constexpr bool operator >  (const_pointer string) const crstl_noexcept { return compare(string) >  0; }
		crstl_constexpr bool operator >= (const_pointer string) const crstl_noexcept { return compare(string) >= 0; }

		crstl_constexpr bool operator == (const basic_string& string) const crstl_noexcept { return compare(string) == 0; }
		crstl_constexpr bool operator != (const basic_string& string) const crstl_noexcept { return compare(string) != 0; }
		crstl_constexpr bool operator <  (const basic_string& string) const crstl_noexcept { return compare(string) <  0; }
		crstl_constexpr bool operator <= (const basic_string& string) const crstl_noexcept { return compare(string) <= 0; }
		crstl_constexpr bool operator >  (const basic_string& string) const crstl_noexcept { return compare(string) >  0; }
		crstl_constexpr bool operator >= (const basic_string& string) const crstl_noexcept { return compare(string) >= 0; }

	private:

		crstl_constexpr size_t compute_new_capacity(size_t old_capacity) const
		{
			return old_capacity + (old_capacity * 50) / 100;
		}

		crstl_constexpr14 size_t reallocate_heap_larger(size_t requested_capacity)
		{
			size_t current_capacity = capacity();
			size_t growth_capacity = compute_new_capacity(current_capacity);
			size_t new_capacity = requested_capacity > growth_capacity ? requested_capacity : growth_capacity;

			// This ensure we pass in a capacity that is larger than the SSO buffer (it wouldn't make sense to allocate otherwise)
			// and larger than the existing heap capacity too
			crstl_assert(new_capacity > capacity());

			CharT* temp = (CharT*)m_layout_allocator.second().allocate(new_capacity * kCharSize + kCharSize);
			size_t length = 0;

			// Copy existing data from current source
			if (is_sso())
			{
				length = length_sso();
				memory_copy(temp, m_layout_allocator.m_first.m_sso.data, length * kCharSize + kCharSize);
			}
			else
			{
				length = length_heap();
				memory_copy(temp, m_layout_allocator.m_first.m_heap.data, length * kCharSize + kCharSize);
				m_layout_allocator.second().deallocate(m_layout_allocator.m_first.m_heap.data, current_capacity * kCharSize + kCharSize);
			}

			m_layout_allocator.m_first.m_heap.data = temp;
			m_layout_allocator.m_first.m_heap.length = length;
			set_capacity_heap(new_capacity);

			return new_capacity;
		}

		// As we need to reallocate and the logic can be a bit convoluted, we'll
		// provide a function here that deals with all that and calls a function
		// with what we need to do
		template<typename Function>
		crstl_constexpr14 void append_function(size_t length, const Function& function)
		{
			size_t current_length = basic_string::length();
			size_t target_length = current_length + length;

			if (target_length < kSSOCapacity)
			{
				CharT* begin = m_layout_allocator.m_first.m_sso.data + current_length;
				function(begin, begin + length);
				m_layout_allocator.m_first.m_sso.remaining_length.value -= (unsigned char)length;
				m_layout_allocator.m_first.m_sso.data[target_length] = 0;
			}
			else
			{
				// If we're currently in sso mode we assume a capacity of 0
				size_t heap_capacity = is_sso() ? 0 : get_capacity_heap();

				// If target length is greater than existing heap capacity, reallocate and copy
				if (target_length > heap_capacity)
				{
					reallocate_heap_larger(target_length);
				}

				CharT* begin = m_layout_allocator.m_first.m_heap.data + current_length;
				function(begin, begin + length);
				m_layout_allocator.m_first.m_heap.length = target_length;
				m_layout_allocator.m_first.m_heap.data[target_length] = 0;
			}
		}

		crstl_constexpr14 void initialize_string(const_pointer string, size_t length)
		{
			if (length < kSSOCapacity)
			{
				memory_copy(m_layout_allocator.m_first.m_sso.data, string, length * kCharSize);
				m_layout_allocator.m_first.m_sso.remaining_length.value = (unsigned char)(kSSOCapacity - length);
				m_layout_allocator.m_first.m_sso.data[length] = 0;
			}
			else
			{
				m_layout_allocator.m_first.m_heap.data = allocate_heap(length);
				memory_copy(m_layout_allocator.m_first.m_heap.data, string, length * kCharSize);
				m_layout_allocator.m_first.m_heap.length = length;
				m_layout_allocator.m_first.m_heap.data[length] = 0;
			}
		}

		crstl_constexpr14 CharT* replace_common(size_t current_length, size_t needle_pos, size_t needle_length, size_t replace_length)
		{
			size_t current_capacity = basic_string::capacity();
			size_t replace_difference = (replace_length - needle_length);
			size_t target_length = current_length + replace_length - needle_length;

			if (target_length > current_capacity)
			{
				current_capacity = reallocate_heap_larger(target_length);
			}

			CharT* data = nullptr;
			size_t dst_offset = needle_pos + replace_length;
			size_t src_offset = needle_pos + needle_length;
			size_t chars_to_move = current_length - (needle_pos + needle_length) + 1;
			size_t bytes_to_move = chars_to_move * kCharSize;

			// Move the parts that would be stomped or leave gaps via memory_move, including the null terminator
			if (is_sso())
			{
				data = m_layout_allocator.m_first.m_sso.data;

				if (replace_difference != 0)
				{
					crstl_assert(dst_offset < kSSOCapacity);
					crstl_assume(dst_offset < kSSOCapacity);
					memory_move(&data[dst_offset], &data[src_offset], bytes_to_move);
				}

				set_length_sso(target_length);
				m_layout_allocator.m_first.m_sso.data[target_length] = 0;
			}
			else
			{
				data = m_layout_allocator.m_first.m_heap.data;

				if (replace_difference != 0)
				{
					memory_move(&data[dst_offset], &data[src_offset], bytes_to_move);
				}

				set_length_heap(target_length);
				m_layout_allocator.m_first.m_heap.data[target_length] = 0;
			}

			return data;
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
			m_layout_allocator.m_first.m_sso.remaining_length.value = (unsigned char)(kSSOCapacity - length);
		}

		void set_length_heap(size_t length)
		{
			m_layout_allocator.m_first.m_heap.length = length;
		}

		void set_length_and_terminator(size_t length)
		{
			if (is_sso())
			{
				crstl_assert(length < kSSOCapacity);
				crstl_assume(length < kSSOCapacity);
				set_length_sso(length);
				m_layout_allocator.m_first.m_sso.data[length] = 0;
			}
			else
			{
				set_length_heap(length);
				m_layout_allocator.m_first.m_heap.data[length] = 0;
			}
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
		CharT* allocate_heap(size_t capacity)
		{
			capacity = compute_new_capacity(capacity);
			CharT* temp = (CharT*)m_layout_allocator.second().allocate(capacity * kCharSize + kCharSize);
			set_capacity_heap(capacity);
			return temp;
		}

		void deallocate_heap()
		{
			m_layout_allocator.second().deallocate(m_layout_allocator.m_first.m_heap.data, get_capacity_heap() * kCharSize + kCharSize);
			set_capacity_heap(0);
			m_layout_allocator.m_first.m_heap.data = nullptr;
		}

		crstl::compressed_pair<layout, Allocator> m_layout_allocator;
	};

	typedef basic_string<char, crstl::allocator> string;
	typedef basic_string<wchar_t, crstl::allocator> wstring;

	typedef basic_string<char8_t, crstl::allocator> u8string;
	typedef basic_string<char16_t, crstl::allocator> u16string;
	typedef basic_string<char32_t, crstl::allocator> u32string;
};