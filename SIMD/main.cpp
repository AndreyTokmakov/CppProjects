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

#include "PerfUtilities.hpp"

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

    void sumVectors_Floats_128()
    {
        float a[4] __attribute__((aligned(16))) = {1.0f, 2.0f, 3.0f, 4.0f};
        float b[4] __attribute__((aligned(16))) = {5.0f, 6.0f, 7.0f, 8.0f};
        float res[4] __attribute__((aligned(16))) {};

        const __m128 va = _mm_load_ps(a);
        const __m128 vb = _mm_load_ps(b);

        // Perform addition
        const __m128 vres = _mm_add_ps(va, vb);

        // Store result
        _mm_store_ps(res, vres);

        for (const auto v: res) {
            std::cout << v << " ";
        }

        // 6 8 10 12
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

namespace load_data
{
    constexpr int ARRAY_SIZE = 8;
    constexpr int TEST_ITERATIONS = 10'000'000;

    void demo()
    {
        // Allocate aligned and unaligned memory
        float* aligned_data = aligned_alloc<float>(ARRAY_SIZE, 32);  // 32-byte alignment for AVX
        float* unaligned_data = new float[ARRAY_SIZE + 1];  // +1 to ensure we can create unaligned pointer
        float* unaligned_ptr = unaligned_data + 1;  // Offset by 1 to ensure unalignment

        // Initialize data
        for (int i = 0; i < ARRAY_SIZE; i++) {
            aligned_data[i] = static_cast<float>(i + 1);
            unaligned_ptr[i] = static_cast<float>(i + 1);
        }

        const __m256 aligned_vec   = _mm256_load_ps(aligned_data);   // Demonstrate aligned load
        const __m256 unaligned_vec = _mm256_loadu_ps(unaligned_ptr); // Demonstrate unaligned load

        print_m256(aligned_vec, "Aligned load result");
        print_m256(unaligned_vec, "Unaligned load result");

        // Aligned   load result : [1, 2, 3, 4, 5, 6, 7, 8]
        // Unaligned load result : [1, 2, 3, 4, 5, 6, 7, 8]
    }

    void benchmark_Data_Load()
    {
        const float* aligned_data = aligned_alloc<float>(ARRAY_SIZE, 32);  // 32-byte alignment for AVX
        const float* unaligned_data = new float[ARRAY_SIZE + 1];  // +1 to ensure we can create unaligned pointer
        const float* unaligned_ptr = unaligned_data + 1;  // Offset by 1 to ensure unalignment

        const __m256 aligned_vec   = _mm256_load_ps(aligned_data);   // Demonstrate aligned load
        const __m256 unaligned_vec = _mm256_loadu_ps(unaligned_ptr); // Demonstrate unaligned load

        // Benchmark aligned load
        auto aligned_load = [&]() {
            __m256 result;
            for (int i = 0; i < TEST_ITERATIONS; i++) {
                result = _mm256_load_ps(aligned_data);
            }
            return result;
        };

        // Benchmark unaligned load
        auto unaligned_load = [&]() {
            __m256 result;
            for (int i = 0; i < TEST_ITERATIONS; i++) {
                result = _mm256_loadu_ps(unaligned_ptr);
            }
            return result;
        };

        {
            PerfUtilities::ScopedTimer timer { "aligned load"};
            aligned_load();
        }
        {
            PerfUtilities::ScopedTimer timer { "unaligned load"};
            unaligned_load();
        }
    }

    void masked_Store ()
    {
        float* aligned_data = aligned_alloc<float>(ARRAY_SIZE, 32);
        const __m256 test_vec = _mm256_set_ps(16.0f, 14.0f, 12.0f, 10.0f,
                                               8.0f, 6.0f, 4.0f, 2.0f);
        // Create a mask to store only elements 1, 3, 5, and 7
        const __m256i mask = _mm256_set_epi32(-1, 0, -1, 0, -1, 0, -1, 0);

        // Perform masked store
        _mm256_maskstore_ps(aligned_data, mask, test_vec);

        std::cout << "Masked store result (odd indices only): [";
        for (int i = 0; i < ARRAY_SIZE - 1; i++) {
            std::cout << aligned_data[i] << ", ";
        }
        std::cout << aligned_data[ARRAY_SIZE - 1] << "]" << std::endl;

        // Masked store result (odd indices only): [0, 4, 0, 8, 0, 12, 0, 16]
    }
}


namespace math
{
    // INFO: https://github.com/yuninxia/hands-on-simd-programming/blob/main/src/02_Computations/01_simple_maths/main.cpp

    void addition()
    {
        constexpr float data1[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        constexpr float data2[8] = {8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f};

        // Load data into SIMD vectors
        const __m256 vector1 = _mm256_loadu_ps(data1);
        const __m256 vector2 = _mm256_loadu_ps(data2);

        print_m256(vector1, "Vector 1");
        print_m256(vector2, "Vector 2");

        const __m256 add_result = _mm256_add_ps(vector1, vector2);
        print_m256(add_result, "Addition Result (Vector 1 + Vector 2)");

        // Vector 1: [1, 2, 3, 4, 5, 6, 7, 8]
        // Vector 2: [8, 7, 6, 5, 4, 3, 2, 1]
        // Addition Result (Vector 1 + Vector 2): [9, 9, 9, 9, 9, 9, 9, 9]
    }

    void subtraction()
    {
        constexpr float data1[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
        constexpr float data2[8] = {8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f};

        // Load data into SIMD vectors
        const __m256 vector1 = _mm256_loadu_ps(data1);
        const __m256 vector2 = _mm256_loadu_ps(data2);

        print_m256(vector1, "Vector 1");
        print_m256(vector2, "Vector 2");

        const __m256 add_result = _mm256_sub_ps(vector1, vector2);
        print_m256(add_result, "Subtraction Result (Vector 1 - Vector 2)");

        // Vector 1: [1, 2, 3, 4, 5, 6, 7, 8]
        // Vector 2: [8, 7, 6, 5, 4, 3, 2, 1]
        // Subtraction Result (Vector 1 - Vector 2): [-7, -5, -3, -1, 1, 3, 5, 7]
    }
}

namespace examples
{
    void filtering_positive_values()
    {
        float* data = aligned_alloc<float>(8);
        {
            data[0] = -1.0f;
            data[1] = 4.0f;
            data[2] = 9.0f;
            data[3] = -16.0f;
            data[4] = 25.0f;
            data[5] = -36.0f;
            data[6] = 49.0f;
            data[7] = -64.0f;
        }

        const __m256 vector = _mm256_load_ps(data);
        const __m256 zero = _mm256_setzero_ps();
        const __m256 positiveMask = _mm256_cmp_ps(vector, zero, _CMP_GT_OQ);

        // Print the mask (all bits set for true, all bits clear for false)
        const float8 maskValues(positiveMask);
        std::cout << "Positive mask (as floats): [";
        for (int i = 0; i < 7; i++) {
            std::cout << maskValues.a[i] << ", ";
        }
        std::cout << maskValues.a[7] << "]" << std::endl;
    }
}


namespace search
{
    bool contains_simple(const std::string& str, const char target)
    {
        for (const char i : str) {
            if (i == target) {
                return true;
            }
        }
        return false;
    }

    bool contains_128(const std::string& str, const char target)
    {
        static constexpr uint32_t blockSize = 16;

        // Load the targets characters into ab SSE register
        const __m128i targetVec = _mm_set1_epi8(target);

        for (uint32_t i = 0; i < str.size(); i += blockSize)
        {
            // Load a block of the string into a AVX register
            const __m128i strVec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(str.data() + i));

            // Compare the block of the string with the target characters using SSE register
            const __m128i resVec = _mm_cmpeq_epi8(strVec, targetVec);

            // Extract the result of the comparison
            const int32_t result = _mm_movemask_epi8(resVec);
            if (result)
                return true;
        }
        return false;
    }

    bool contains_256(const std::string& str, const char target)
    {
        static constexpr uint32_t blockSize = 32;

        // Load the targets characters into ab AVX register
        const __m256i  targetVec = _mm256_set1_epi8(target);

        for (uint32_t i = 0; i < str.size(); i += blockSize )
        {
            // Load a block of the string into a AVX register
            const __m256i strVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(str.data() + i));

            // Compare the block of the string with the target characters using AVX register
            const __m256i resVec = _mm256_cmpeq_epi8(strVec, targetVec);

            // Extract the result of the comparison
            const int32_t result = _mm256_movemask_epi8(resVec);
            if (result)
                return true;
        }
        return false;
    }

    void contains()
    {
        const std::string str = "abcdefghabcdefghabcdefghabcdefghabcdefghabcdefghabcdefghabcdefgh";
        constexpr char target = 'a';

        {
            const bool result = contains_simple(str, target);
            std::cout << std::boolalpha << result << std::endl;
        }
        {
            const bool result = contains_128(str, target);
            std::cout << std::boolalpha << result << std::endl;
        }

        {
            const bool result = contains_256(str, target);
            std::cout << std::boolalpha << result << std::endl;
        }
    }
}

namespace search
{
    int findFirst(const std::span<const int> data, const int target)
    {
        const __m256i vec = _mm256_set1_epi32(target);
        for (int i = 0 ; i < std::ssize(data); i += 8)
        {
            const __m256i arrVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data.data() + i));
            const __m256i mask = _mm256_cmpeq_epi32(arrVec, vec);
            if (const unsigned int maskInt = _mm256_movemask_epi8(mask); 0 != maskInt) {
                for (int j = 0; j < 8; j++) {
                    if ((maskInt >> (j * 4)) & 0xF) {
                        return i + j;
                    }
                }
            }
        }
        return -1;
    }


    void findFirst()
    {
        std::vector<int> values {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
        const int idx = findFirst(values, 13);
        std::cout  << values[idx] << std::endl;
    }
}

namespace search
{
    int32_t max_simd(const std::span<const int32_t> data)
    {
        if (data.empty())
            return std::numeric_limits<int32_t>::min();

        const int32_t* ptr = data.data();
        const size_t size = data.size();

        static constexpr size_t W = 8; // 8 x int32
        size_t i = 0;

        __m256i vmax = _mm256_set1_epi32(std::numeric_limits<int32_t>::min());

        // SIMD loop
        for (; i + W <= size; i += W)
        {
            __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + i));
            vmax = _mm256_max_epi32(vmax, v);
        }

        // reduction
        alignas(32) int32_t tmp[W];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), vmax);

        int32_t result = tmp[0];
        for (int j = 1; j < W; ++j)
            result = result > tmp[j] ? result : tmp[j];

        // tail
        for (; i < size; ++i)
            result = result > ptr[i] ? result : ptr[i];

        return result;
    }

    int32_t findMax2(const std::span<const int> data)
    {
        __m256i maxVec = _mm256_set1_epi32(std::numeric_limits<int32_t>::max());
        for (int i = 0 ; i < std::ssize(data); i += 8)
        {
             __m256i arrVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data.data() + i));
             __m256i cmpVec = _mm256_cmpeq_epi32(arrVec, maxVec);
            // maxVec = _mm256_blend_epi32(maxVec, arrVec, cmpVec);

        }
    }



    void findMax()
    {
        std::vector<int32_t> v = {1, -3, 42, 7, 9, 100, 5};
        std::cout << max_simd(v) << "\n"; // 100
    }

}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // Vector_Addition::demo();
    // Multiplication::demo();

    // basics::sumVectors();
    // basics::sumVectors_Floats_128();
    // basics::sumVectors_AndStore();
    // basics::sumInts();
    // basics::sumDoubles();

    // data_initialization::init_Simd_Arrays();
    // data_initialization::init_Simd_Arrays_and_FILL();
    // data_initialization::init_Simd_Arrays_as_InitList();

    // load_data::demo();
    // load_data::benchmark_Data_Load();
    // load_data::masked_Store();

    // math::addition();
    // math::subtraction();

    // search::contains();
    // search::findFirst();
    search::findMax();

    // examples::filtering_positive_values();

    return EXIT_SUCCESS;
}
