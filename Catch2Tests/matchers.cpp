#include "test_controller.h"

#ifdef MATCHERS

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include <algorithm>
#include <random>

TEST_CASE("vector unordered matcher", "[matchers][vector]")
{
    using Catch::Matchers::UnorderedEquals;
    std::vector<int> vec{0, 1, 2, 3, 4};

    std::shuffle(vec.begin(), vec.end(), std::random_device{});

    REQUIRE_THAT(vec, UnorderedEquals<int>({0, 1, 2, 3, 4}));
}

#endif