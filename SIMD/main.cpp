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
    /*
    void _mm512_mul_to(float *a, float *b, float *c, int64_t n)
    {
        int epoch = n / 16;
        int remain = n % 16;
        for (int i = 0; i < epoch; i++)
        {
            __m512 v1 = _mm512_loadu_ps(a);
            __m512 v2 = _mm512_loadu_ps(b);
            __m512 v = _mm512_mul_ps(v1, v2);
            _mm512_storeu_ps(c, v);
            a += 16;
            b += 16;
            c += 16;
        }
        if (remain >= 8)
        {
            __m256 v1 = _mm256_loadu_ps(a);
            __m256 v2 = _mm256_loadu_ps(b);
            __m256 v = _mm256_mul_ps(v1, v2);
            _mm256_storeu_ps(c, v);
            a += 8;
            b += 8;
            c += 8;
            remain -= 8;
        }
        for (int i = 0; i < remain; i++)
        {
            c[i] = a[i] * b[i];
        }
    }*/


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



int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    Vector_Addition::demo();

    return EXIT_SUCCESS;
}
