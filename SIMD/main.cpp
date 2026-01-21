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


namespace
{
    union float8
    {
        __m256 v;
        float a[8] {};

        explicit float8(__m256 _v) : v(_v) {
        }

        float8() : v(_mm256_setzero_ps()) {
        }
    };

    union double4
    {
        __m256d v;
        double a[4] {};

        explicit double4(__m256d _v) : v(_v) {
        }

        double4() : v(_mm256_setzero_pd()) {
        }
    };

    union int8
    {
        __m256i v;
        int a[8] {};

        explicit int8(__m256i _v) : v(_v) {
        }

        int8() : v(_mm256_setzero_si256()) {
        }
    };


    void print_m256(const __m256& v, const std::string& label = "")
    {
        const float8 tmp(v);
        if (!label.empty()) {
            std::cout << label << ": ";
        }
        std::cout << "[";
        for (int i = 0; i < 7; i++) {
            std::cout << tmp.a[i] << ", ";
        }
        std::cout << tmp.a[7] << "]" << std::endl;
    }

    void print_m256d(const __m256d& v, const std::string& label = "")
    {
        const double4 tmp(v);
        if (!label.empty()) {
            std::cout << label << ": ";
        }
        std::cout << "[";
        for (int i = 0; i < 3; i++) {
            std::cout << tmp.a[i] << ", ";
        }
        std::cout << tmp.a[3] << "]" << std::endl;
    }

    void print_m256i(const __m256i& v, const std::string& label = "")
    {
        const int8 tmp(v);
        if (!label.empty()) {
            std::cout << label << ": ";
        }
        std::cout << "[";
        for (int i = 0; i < 7; i++) {
            std::cout << tmp.a[i] << ", ";
        }
        std::cout << tmp.a[7] << "]" << std::endl;
    }

    template<typename T>
    T* aligned_alloc(size_t size, size_t alignment = 32)
    {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, alignment, size * sizeof(T)) != 0) {
            throw std::bad_alloc();
        }
        return static_cast<T*>(ptr);
    }
}

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


// INFO: https://github.com/yuninxia/hands-on-simd-programming/blob/main/src/01_Basics/01_importing_simd/main.cpp
namespace basics
{
    void sumVectors()
    {
        // Initialize two SIMD vectors with 8 float values each
        __m256 a = _mm256_set_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        __m256 b = _mm256_set_ps(8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f);

        // Add the vectors element-wise
        __m256 c = _mm256_add_ps(a, b);

        // Print the vectors using our utility function
        print_m256(a, "Vector A");
        print_m256(b, "Vector B");
        print_m256(c, "A + B");

        // Vector A: [8, 7, 6, 5, 4, 3, 2, 1]
        // Vector B: [1, 2, 3, 4, 5, 6, 7, 8]
        // A + B: [9, 9, 9, 9, 9, 9, 9, 9]
    }

    void sumVectors_AndStore()
    {
        // Initialize two SIMD vectors with 8 float values each
        __m256 a = _mm256_set_ps(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f);
        __m256 b = _mm256_set_ps(8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f);

        // Add the vectors element-wise
        __m256 c = _mm256_add_ps(a, b);

        // Print the vectors using our utility function
        print_m256(a, "Vector A");
        print_m256(b, "Vector B");
        print_m256(c, "A + B");

        // Example 2: Storing SIMD results back to memory
        std::cout << std::endl;
        std::cout << "Example 2: Storing SIMD Results" << std::endl;

        // Allocate aligned memory for results
        float* result = aligned_alloc<float>(8);

        // Store the SIMD vector to memory
        _mm256_store_ps(result, c);

        // Print the results from memory
        std::cout << "Result array: [";
        for (int i = 0; i < 7; i++) {
            std::cout << result[i] << ", ";
        }
        std::cout << result[7] << "]" << std::endl;

        // Vector A: [8, 7, 6, 5, 4, 3, 2, 1]
        // Vector B: [1, 2, 3, 4, 5, 6, 7, 8]
        // A + B: [9, 9, 9, 9, 9, 9, 9, 9]
        //
        // Example 2: Storing SIMD Results
        // Result array: [9, 9, 9, 9, 9, 9, 9, 9]
    }

    void sumInts()
    {
        // Integer SIMD operations
        __m256i int_a = _mm256_set_epi32(1, 2, 3, 4, 5, 6, 7, 8);
        __m256i int_b = _mm256_set_epi32(8, 7, 6, 5, 4, 3, 2, 1);
        __m256i int_sum = _mm256_add_epi32(int_a, int_b);

        print_m256i(int_a, "Integer Vector A");
        print_m256i(int_b, "Integer Vector B");
        print_m256i(int_sum, "A + B (Integer)");

        // Integer Vector A: [8, 7, 6, 5, 4, 3, 2, 1]
        // Integer Vector B: [1, 2, 3, 4, 5, 6, 7, 8]
        // A + B (Integer): [9, 9, 9, 9, 9, 9, 9, 9]
    }

    void sumDoubles()
    {
        // Double precision SIMD operations (4 doubles in a 256-bit register)
        const __m256d double_a = _mm256_set_pd(1.0, 2.0, 3.0, 4.0);
        const __m256d double_b = _mm256_set_pd(4.0, 3.0, 2.0, 1.0);
        const __m256d double_sum = _mm256_add_pd(double_a, double_b);

        print_m256d(double_a, "Double Vector A");
        print_m256d(double_b, "Double Vector B");
        print_m256d(double_sum, "A + B (Double)");

        // Double Vector A: [4, 3, 2, 1]
        // Double Vector B: [1, 2, 3, 4]
        // A + B (Double): [5, 5, 5, 5]
    }
}

namespace data_initialization
{

    void init_Simd_Arrays()
    {
        const __m256 simd_float_vec = _mm256_setzero_ps();
        const __m256i simd_int_vec = _mm256_setzero_si256();
        const __m256d simd_double_vec = _mm256_setzero_pd();

        print_m256(simd_float_vec, "Zero-initialized float vector");
        print_m256i(simd_int_vec, "Zero-initialized integer vector");
        print_m256d(simd_double_vec, "Zero-initialized double vector");

        // Zero-initialized float vector: [0, 0, 0, 0, 0, 0, 0, 0]
        // Zero-initialized integer vector: [0, 0, 0, 0, 0, 0, 0, 0]
        // Zero-initialized double vector: [0, 0, 0, 0]
    }

    void init_Simd_Arrays_and_FILL()
    {
        const __m256d simd_double_vec =  _mm256_set1_pd(10.0);
        const __m256 simd_float_vec = _mm256_set1_ps(42.0f);
        const __m256i simd_int_vec = _mm256_set1_epi32(100);

        print_m256d(simd_double_vec, "Broadcast-initialized double vector (10.0)");
        print_m256(simd_float_vec, "Broadcast-initialized float vector (42.0)");
        print_m256i(simd_int_vec, "Broadcast-initialized integer vector (100)");

        // Broadcast-initialized double vector (10.0): [10, 10, 10, 10]
        // Broadcast-initialized float vector (42.0): [42, 42, 42, 42, 42, 42, 42, 42]
        // Broadcast-initialized integer vector (100): [100, 100, 100, 100, 100, 100, 100, 100]
    }


    void init_Simd_Arrays_as_InitList()
    {
        const __m256i simd_int_vec = _mm256_set_epi32(8, 7, 6, 5, 4, 3, 2, 1);
        const __m256 simd_float_vec = _mm256_set_ps(8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f);
        const __m256d simd_double_vec = _mm256_set_pd(7.0, 6.0, 5.0, 4.0);

        print_m256i(simd_int_vec, "Individually-initialized integer vector");
        print_m256(simd_float_vec, "Individually-initialized float vector");
        print_m256d(simd_double_vec, "Individually-initialized double vector");

        // Individually-initialized integer vector: [1, 2, 3, 4, 5, 6, 7, 8]
        // Individually-initialized float vector: [1, 2, 3, 4, 5, 6, 7, 8]
        // Individually-initialized double vector: [4, 5, 6, 7]
    }

}

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // Vector_Addition::demo();
    // Multiplication::demo();

    // basics::sumVectors();
    // basics::sumVectors_AndStore();
    // basics::sumInts();
    // basics::sumDoubles();

    // data_initialization::init_Simd_Arrays();
    // data_initialization::init_Simd_Arrays_and_FILL();
    data_initialization::init_Simd_Arrays_as_InitList();

    return EXIT_SUCCESS;
}
