//============================================================================
// Name        : BitSet.h
// Created on  : 17.05.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : BitSet (custom implementation) src
//============================================================================

#ifndef BITSET_CUSTOM_TESTS__INCL_GUARD_
#define BITSET_CUSTOM_TESTS__INCL_GUARD_

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>

namespace BitSet {

    template<typename StorageType = uint32_t,
             size_t BLOCK_BITS_SIZE = sizeof(StorageType) * 8>
	class BitSet
	{
	private:
		std::vector<StorageType> bits {0};

	public:

		void setBit(const size_t bit, const bool value) noexcept {
			const size_t index = (bit - 1) / BLOCK_BITS_SIZE;
			const size_t scoped_bit = (bit - 1) % BLOCK_BITS_SIZE;

			if (bits.size() <= index) 
				bits.resize(index + 1);

            auto& mask = bits[index];
            if (value)
                mask |= (1 << scoped_bit);
			else
                mask &= (~(1 << scoped_bit));
		}

		[[nodiscard]]
        bool getBit(const size_t bit) const noexcept {
			if (BLOCK_BITS_SIZE * bits.size() < bit) 
				return false;
			
			const size_t index = (bit - 1) / BLOCK_BITS_SIZE;
			const size_t scoped_bit = (bit - 1) % BLOCK_BITS_SIZE;

			return bits[index] & (1 << scoped_bit);
		}
	};

    template<size_t Size = 0,
             typename Type = uint32_t,
             typename StorageType = typename std::conditional<0 == Size, std::vector<Type>, std::array<Type, Size>>::type,
             size_t BLOCK_BITS_SIZE = sizeof(Type) * 8>
    class BitSetEx
    {
    private:
        StorageType bits;

    public:

        void setBit(const size_t bit, const bool value) noexcept {
            const size_t index = (bit - 1) / BLOCK_BITS_SIZE;
            const size_t scoped_bit = (bit - 1) % BLOCK_BITS_SIZE;

            if constexpr (0 == Size) {
                if (bits.size() <= index)
                    bits.resize(index + 1);
            }

            auto& mask = bits[index];
            if (value)
                mask |= (1 << scoped_bit);
            else
                mask &= (~(1 << scoped_bit));
        }

        [[nodiscard]]
        bool getBit(const size_t bit) const noexcept {
            if (BLOCK_BITS_SIZE * bits.size() < bit)
                return false;

            const size_t index = (bit - 1) / BLOCK_BITS_SIZE;
            const size_t scoped_bit = (bit - 1) % BLOCK_BITS_SIZE;

            return bits[index] & (1 << scoped_bit);
        }
    };

	void TEST_ALL();
};

#endif // (!BITSET_CUSTOM_TESTS__INCL_GUARD_)