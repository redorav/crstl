#pragma once

#include "crstl/config.h"

#include "crstl/crstldef.h"

#include "crstl/bit.h"

#include "crstl/utility/memory_ops.h"

// crstl::bitset
//
// Replacement for std::bitset
//

crstl_module_export namespace crstl
{
	template<size_t NumBits, typename WordType>
	class bitset
	{
	public:

		typedef WordType word_type;

		static const word_type kNumBits = NumBits;
		static const word_type kBitsPerWord = sizeof(word_type) * 8;
		static const word_type kBitsPerWordMask = kBitsPerWord - 1; // Used as modulo for words
		static const word_type kBitsPerWordShift =                  // How many bits to shift for this word type
			(kBitsPerWord == 8) ? 3 :
			(kBitsPerWord == 16) ? 4 :
			(kBitsPerWord == 32) ? 5 :
			(kBitsPerWord == 64) ? 6 : 7;
		static const word_type kMaxWordValue = word_type(~word_type(0));                           // Maximum value of this word type
		static const word_type kNumWords = (NumBits + kBitsPerWord - 1) / kBitsPerWord;            // How many words we need to store the number of bits
		static const word_type kLastWordBits = kNumBits - (kNumWords - 1) * kBitsPerWord;          // How many bits in the last word
		static const word_type kLastWordBitMask = kMaxWordValue >> (kBitsPerWord - kLastWordBits); // Mask for those "leftover" bits in the last word

		// Hold reference to a bit, so that we can do the [ ] operator
		struct bit_reference
		{
			word_type& bit_word;
			size_t bit_index;

			bit_reference(word_type& bit_word, size_t bit_index) : bit_word(bit_word), bit_index(bit_index) {}

			bit_reference(const bit_reference& r)
				: bit_word(r.bit_word)
				, bit_index(r.bit_index)
			{}

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

		bitset()
		{
			memory_set(m_data, 0, sizeof(m_data));
		}

		bitset(word_type v)
		{
			memory_set(m_data, 0, sizeof(m_data));

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
			for (size_t w = 0; w < kNumWords; ++w)
			{
				if (m_data[w] != 0)
				{
					return true;
				}
			}

			return false;
		}

		size_t count() const
		{
			size_t n = 0;

			for (size_t w = 0; w < kNumWords; ++w)
			{
				n += crstl::popcount(m_data[w]);
			}

			return n;
		}

		bitset& flip(size_t i)
		{
			crstl_assert(i < NumBits);

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
			memory_set(m_data, 0, sizeof(m_data));
		}

		bitset& set()
		{
			memory_set(m_data, 0xff, sizeof(m_data));
			
			// Set bits in last word to 0 if necessary
			crstl_constexpr_if(kNumBits & kBitsPerWordMask)
			{
				m_data[kNumWords - 1] &= kLastWordBitMask;
			}

			return *this;
		}

		bitset& set(size_t i, bool value = true)
		{
			crstl_assert(i < NumBits);

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
			crstl_assert(i < NumBits);
			return (m_data[i >> kBitsPerWordShift] & ((word_type)1 << (i & kBitsPerWordMask))) != 0;
		}

		bit_reference operator[](size_t i)
		{
			crstl_assert(i < NumBits);
			return bit_reference(m_data[i >> kBitsPerWordShift], (i & kBitsPerWordMask));
		}

		bool operator[](size_t i) const
		{
			crstl_assert(i < NumBits);
			return (m_data[i >> kBitsPerWordShift] & ((word_type)1 << i)) != 0;
		}

		bool operator == (const bitset& b) const
		{
			for (size_t w = 0; w < kNumWords; ++w)
			{
				if (m_data[w] != b.m_data[w])
				{
					return false;
				}
			}

			return true;
		}

		bool operator != (const bitset& b) const
		{
			for (size_t w = 0; w < kNumWords; ++w)
			{
				if (m_data[w] != b.m_data[w])
				{
					return true;
				}
			}

			return false;
		}

	private:

		word_type m_data[kNumWords];
	};
};