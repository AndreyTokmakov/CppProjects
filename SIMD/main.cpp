/**============================================================================
Name        : main.cpp
Created on  : 13.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SIMD
============================================================================**/

#include <immintrin.h> // Header for AVX intrinsics
#include <vector>
#include <iostream>


namespace Vector_Addition
{
    void add_vectors_simd(const std::vector<float>& a,
                          const std::vector<float>& b,
                          std::vector<float>& result)
    {
        size_t i = 0;
        const size_t size = a.size();

        // Process 8 elements at a time using AVX
        for (; i + 7 < size; i += 8)
        {
            __m256 va = _mm256_loadu_ps(&a[i]); // Load 8 floats from a
            __m256 vb = _mm256_loadu_ps(&b[i]); // Load 8 floats from b
            __m256 vresult = _mm256_add_ps(va, vb); // Add the vectors
            _mm256_storeu_ps(&result[i], vresult); // Store the result
        }

        // Handle remaining elements
        for (; i < size; ++i) {
            result[i] = a[i] + b[i];
        }
    }

    void demo()
    {
        std::vector<float> a = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        std::vector<float> b = {9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
        std::vector<float> result(8);

        add_vectors_simd(a, b, result);

        for (const float value : result) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }

}

namespace Multiplication
{
    void process_with_simd(float* data, const size_t size)
    {
        const __m128 factor = _mm_set1_ps(2.0f); // Set all elements to 2.0

        for (size_t i = 0; i < size; i += 4) {
            __m128 values = _mm_loadu_ps(&data[i]);    // Load 4 floats
            values = _mm_mul_ps(values, factor);    // Multiply by factor
            _mm_storeu_ps(&data[i], values);           // Store result
        }
    }


    void demo()
    {
        float data[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};

        // Process data using SIMD
        process_with_simd(data, 8);

        for (const float value : data) {
            std::cout << value << " "; // Output: 2 4 6 8 10 12 14 16
        }
        std::cout << std::endl;
    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // Vector_Addition::demo();
    Multiplication::demo();

    return EXIT_SUCCESS;
}
