/**============================================================================
Name        : RandomizedSet.cpp
Created on  : 28.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : RandomizedSet.cpp
============================================================================**/

#include "RandomizedSet.hpp"

#include <cstddef>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>


namespace randomized_set
{
    template<typename T>
    class RandomizedSet
    {
        using value_type = T;
        using size_type  = std::size_t;

        std::vector<T> values;
        std::unordered_map<T, size_type> indices;

        static inline std::random_device rd{};
        static inline std::mt19937 randomEngine = std::mt19937 {rd()};

    public:
        [[nodiscard]]
        bool insert(const T& value)
        {
            const auto [iter, inserted] = indices.try_emplace(value);
            if (!inserted) {
                return false;
            }

            values.push_back(value);
            iter->second = values.size();
            return true;
        }

        [[nodiscard]]
        bool remove(const T& value)
        {
            const auto iterator = indices.find(value);
            if (iterator == indices.end()) {
                return false;
            }

            const size_type index = iterator->second;
            const size_type lastIndex = values.size() - 1;

            if (index != lastIndex)
            {
                indices[values.back()]= index;
                values[index] = std::move(values.back());
            }

            values.pop_back();
            indices.erase(iterator);

            return true;
        }

        [[nodiscard]]
        bool contains(const T& value) const
        {
            return indices.contains(value);
        }

        [[nodiscard]]
        std::optional<T> getRandom()
        {
            if (values.empty()) {
                return std::nullopt;
            }

            std::uniform_int_distribution<size_type> distribution { 0, values.size() - 1 };
            return values[distribution(randomEngine)];
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return values.size();
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return values.empty();
        }
    };
}

void randomized_set::TestAll()
{

}
