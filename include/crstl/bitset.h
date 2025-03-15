#pragma once

#include "crstl/config.h"
#include "crstl/crstldef.h"
#include "crstl/bit.h"
#include "crstl/forward_declarations.h"
#include "crstl/utility/memory_ops.h"
#include "crstl/utility/constructor_utils.h"

// crstl::bitset
//
// Replacement for std::bitset
//

#define CRSTL_WORD_COUNT(BitCount, WordType) ((BitCount + 8 * sizeof(WordType)) / (8 * sizeof(WordType)))

crstl_module_export namespace crstl
{
	template<size_t WordCount, typename WordType>
	class bitset_base
	{
	public:

		typedef WordType                                                    word_type;

	protected:

		word_type m_data[WordCount];
	};

	template<size_t BitCount, typename WordType>
	class bitset : public bitset_base<CRSTL_WORD_COUNT(BitCount, WordType), WordType>
	{
	public:

		typedef bitset_base<CRSTL_WORD_COUNT(BitCount, WordType), WordType> base_type;
		typedef WordType                                                    word_type;

		using base_type::m_data;

		static const word_type kBitCount = BitCount;
		static const word_type kBitsPerWord = sizeof(word_type) * 8;
		static const word_type kBitsPerWordMask = kBitsPerWord - 1; // Used as modulo for words
		static const word_type kBitsPerWordShift =                  // How many bits to shift for this word type
			(kBitsPerWord == 8) ? 3 :
			(kBitsPerWord == 16) ? 4 :
			(kBitsPerWord == 32) ? 5 :
			(kBitsPerWord == 64) ? 6 : 7;
		static const word_type kMaxWordValue = word_type(~word_type(0));                           // Maximum value of this word type
		static const word_type kWordCount = (BitCount + kBitsPerWord - 1) / kBitsPerWord;            // How many words we need to store the number of bits
		static const word_type kLastWordBits = kBitCount - (kWordCount - 1) * kBitsPerWord;          // How many bits in the last word
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

		bitset(ctor_no_initialize_e) {}

		bitset(word_type v)
		{
			memory_set(m_data, 0, sizeof(m_data));

			m_data[0] = v;

			// Set bits in last word to 0 if necessary
			crstl_constexpr_if(kBitCount & kBitsPerWordMask)
			{
				m_data[0] &= kLastWordBitMask;
			}
		}

		bool all() const
		{
			return (count() == size());
		}

		bool any() const
		{
			for (size_t w = 0; w < kWordCount; ++w)
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

			for (size_t w = 0; w < kWordCount; ++w)
			{
				n += crstl::popcount(m_data[w]);
			}

			return n;
		}

		bitset& flip(size_t i)
		{
			crstl_assert(i < BitCount);

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
			crstl_constexpr_if(kBitCount & kBitsPerWordMask)
			{
				m_data[kWordCount - 1] &= kLastWordBitMask;
			}

			return *this;
		}

		bitset& set(size_t i, bool value = true)
		{
			crstl_assert(i < BitCount);

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
			return kBitCount;
		}

		bool test(size_t i) const
		{
			crstl_assert(i < BitCount);
			return (m_data[i >> kBitsPerWordShift] & ((word_type)1 << (i & kBitsPerWordMask))) != 0;
		}

		//------------------
		// Bitwise Operators
		//------------------

		bitset operator & (const bitset& other)
		{
			bitset result(ctor_no_initialize);

			for (size_t i = 0; i < kWordCount; ++i)
			{
				result.m_data[i] = m_data[i] & other.m_data[i];
			}

			return result;
		}

		bitset operator | (const bitset& other)
		{
			bitset result(ctor_no_initialize);

			for (size_t i = 0; i < kWordCount; ++i)
			{
				result.m_data[i] = m_data[i] | other.m_data[i];
			}

			return result;
		}

		bitset operator ^ (const bitset& other)
		{
			bitset result(ctor_no_initialize);

			for (size_t i = 0; i < kWordCount; ++i)
			{
				result.m_data[i] = m_data[i] ^ other.m_data[i];
			}

			return result;
		}

		bitset operator ~() const
		{
			bitset result(ctor_no_initialize);

			for (size_t i = 0; i < kWordCount; ++i)
			{
				result.m_data[i] = ~m_data[i];
			}

			return result;
		}

		bitset& operator &= (const bitset& other)
		{
			for (size_t i = 0; i < kWordCount; ++i)
			{
				m_data[i] &= other.m_data[i];
			}

			return *this;
		}

		bitset& operator |= (const bitset& other)
		{
			for (size_t i = 0; i < kWordCount; ++i)
			{
				m_data[i] |= other.m_data[i];
			}

			return *this;
		}

		bitset& operator ^= (const bitset& other)
		{
			for (size_t i = 0; i < kWordCount; ++i)
			{
				m_data[i] ^= other.m_data[i];
			}

			return *this;
		}

		//-------------------
		// Accessor Operators
		//-------------------

		bit_reference operator[](size_t i)
		{
			crstl_assert(i < BitCount);
			return bit_reference(m_data[i >> kBitsPerWordShift], (i & kBitsPerWordMask));
		}

		bool operator[](size_t i) const
		{
			crstl_assert(i < BitCount);
			return (m_data[i >> kBitsPerWordShift] & ((word_type)1 << i)) != 0;
		}

		//---------------------
		// Comparison Operators
		//---------------------

		bool operator == (const bitset& other) const
		{
			for (size_t w = 0; w < kWordCount; ++w)
			{
				if (m_data[w] != other.m_data[w])
				{
					return false;
				}
			}

			return true;
		}

		bool operator != (const bitset& other) const
		{
			for (size_t w = 0; w < kWordCount; ++w)
			{
				if (m_data[w] != other.m_data[w])
				{
					return true;
				}
			}

			return false;
		}
	};
};