#pragma once

#include "config.h"

#include "stdint.h"

namespace crstl
{
	// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	template<typename T>
	uint64_t count_bits(T v)
	{
		v = v - ((v >> 1) & (T)~(T)0 / 3);
		v = (v & (T)~(T)0 / 15 * 3) + ((v >> 2) & (T)~(T)0 / 15 * 3);
		v = (v + (v >> 4)) & (T)~(T)0 / 255 * 15;
		T c = (T)(v * ((T)~(T)0 / 255)) >> (sizeof(T) - 1) * 8 /*CHAR_BIT*/;
		return c;
	}

	template<size_t NumBits, typename WordType>
	class bitset
	{
	public:

		typedef WordType word_type;

		enum : word_type
		{
			kNumBits = NumBits,
			kBitsPerWord = sizeof(word_type) * 8,
			kBitsPerWordMask = kBitsPerWord - 1, // Used as modulo for words
			kBitsPerWordShift =                  // How many bits to shift for this word type
			(kBitsPerWord == 8) ? 3 :
			(kBitsPerWord == 16) ? 4 :
			(kBitsPerWord == 32) ? 5 :
			(kBitsPerWord == 64) ? 6 : 7,
			kMaxWordValue = word_type(~word_type(0)),                          // Maximum value of this word type
			kNumWords = (NumBits + kBitsPerWord - 1) / kBitsPerWord,          // How many words we need to store the number of bits
			kLastWordBits = kNumBits - (kNumWords - 1) * kBitsPerWord,         // How many bits in the last word
			kLastWordBitMask = kMaxWordValue >> (kBitsPerWord - kLastWordBits) // Mask for those "leftover" bits in the last word
		};

		// Hold reference to a bit, so that we can do the [ ] operator
		struct bit_reference
		{
			word_type& bit_word;
			size_t bit_index;

			bit_reference(word_type& bit_word, size_t bit_index) : bit_word(bit_word), bit_index(bit_index) {}

			bit_reference& operator = (bool value)
			{
				if (value)
				{
					bit_word |= ((word_type)1 << bit_index);
				}
				else
				{
					bit_word &= ~((word_type)1 << bit_index);
				}

				return *this;
			}

			bit_reference& operator = (const bit_reference& r)
			{
				// If the bit in the input reference is set, set the bit in the current reference
				// Else, unset the bit
				if (r.bit_word & ((word_type)1 << r.bit_index))
				{
					bit_word |= ((word_type)1 << bit_index);
				}
				else
				{
					bit_word &= ~((word_type)1 << bit_index);
				}

				return *this;
			}

			operator bool () const
			{
				return (bit_word & ((word_type)1 << bit_index)) != 0;
			}
		};

		bitset() {}

		bitset(word_type v)
		{
			m_data[0] = v;

			// Set bits in last word to 0 if necessary
			crstl_constexpr_if(kNumBits & kBitsPerWordMask)
			{
				m_data[0] &= kLastWordBitMask;
			}
		}

		bool all() const
		{
			return count() == size();
		}

		bool any() const
		{
			for (size_t i = 0; i < kNumWords; ++i)
			{
				if (m_data[i] != 0)
				{
					return true;
				}
			}

			return false;
		}

		size_t count() const
		{
			size_t n = 0;

			for (uint64_t i = 0; i < kNumWords; ++i)
			{
				#if defined(__GNUG__) || defined(__clang__)
                n += __builtin_popcountll(m_data[i]);
				#else
				n += count_bits(m_data[i]);
				#endif
			}

			return n;
		}

		bitset& flip(size_t i)
		{
			if (m_data[i >> kBitsPerWordShift] & ((word_type)1 << (i & kBitsPerWordMask)))
			{
				m_data[i >> kBitsPerWordShift] &= ~((word_type)1 << (i & kBitsPerWordMask));
			}
			else
			{
				m_data[i >> kBitsPerWordShift] |= ((word_type)1 << (i & kBitsPerWordMask));
			}

			return *this;
		}

		bool none() const
		{
			return !any();
		}

		void reset()
		{
			memset(m_data, 0, sizeof(m_data));
		}

		bitset& set()
		{
			memset(m_data, 0xff, sizeof(m_data));
			
			// Set bits in last word to 0 if necessary
			crstl_constexpr_if(kNumBits & kBitsPerWordMask)
			{
				m_data[kNumWords - 1] &= kLastWordBitMask;
			}

			return *this;
		}

		bitset& set(size_t i, bool value = true)
		{
			if (value)
			{
				m_data[i >> kBitsPerWordShift] |= ((word_type)1 << (i & kBitsPerWordMask));
			}
			else
			{
				m_data[i >> kBitsPerWordShift] &= ~((word_type)1 << (i & kBitsPerWordMask));
			}
			
			return *this;
		}

		size_t size() const
		{
			return kNumBits;
		}

		bool test(size_t i) const
		{
			return (m_data[i >> kBitsPerWordShift] & ((word_type)1 << (i & kBitsPerWordMask))) != 0;
		}

		bit_reference operator[](size_t i)
		{
			return bit_reference(m_data[i >> kBitsPerWordShift], (i & kBitsPerWordMask));
		}

		bool operator[](size_t i) const
		{
			return (m_data[i >> kBitsPerWordShift] & ((word_type)1 << i)) != 0;
		}

		bool operator == (const bitset& b) const
		{
			for (size_t i = 0; i < kNumWords; ++i)
			{
				if (m_data[i] != b.m_data[i])
				{
					return false;
				}
			}

			return true;
		}

		bool operator != (const bitset& b) const
		{
			for (size_t i = 0; i < kNumWords; ++i)
			{
				if (m_data[i] != b.m_data[i])
				{
					return true;
				}
			}

			return false;
		}

	private:

		word_type m_data[kNumWords] = {};
	};
};