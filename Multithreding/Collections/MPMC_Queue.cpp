/**============================================================================
Name        : MPMC_Queue.cpp
Created on  : 14.05.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MPMC_Queue.cpp
============================================================================**/

#include "Collections.h"

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <syncstream>

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>


namespace
{
    // most CPUs use 64-byte cache lines. use the standard constant if available, otherwise just hardcode 64.
#ifdef __cpp_lib_hardware_interference_size
    inline constexpr size_t CACHELINE_SZ = std::hardware_destructive_interference_size;
#else
    inline constexpr size_t CACHELINE_SZ = 64;
#endif

    constexpr uint32_t fast_modulo(const uint32_t n, const uint32_t d) noexcept {
        return n & (d - 1);
    }

    constexpr bool is_pow_of_2(const int value) noexcept {
        return (value && !(value & (value - 1)));
    }
};

namespace
{
    template <typename Ty, size_t Capacity>
    class MPMCQueue
    {
        using size_type  = uint64_t;
        using value_type = Ty;
        using collection_type = std::vector<value_type>;

        static_assert(!std::is_same_v<value_type, void>, "ERROR: Value type can not be void");
        static_assert(is_pow_of_2(Capacity), "ERROR: Capacity must be a power of 2");
        static_assert(Capacity >= 2, "ERROR: Capacity must greater than 2");

        // Each slot gets its own cache line so two threads working on adjacent slots don't step on each other
        struct alignas(CACHELINE_SZ) Slot
        {
            std::atomic<size_type> seq;

            // raw bytes, not T — avoids default ctor
            alignas(value_type) std::array<unsigned char, sizeof(value_type)> buffer{};
        };

        std::array<Slot, Capacity> ringBuffer{};

        // These two get hammered by different threads - Without the alignment they'd
        // end up on the same cache line and every push would slow down every pop and vice versa.
        alignas(CACHELINE_SZ) std::atomic<size_type> writePos {0 };
        alignas(CACHELINE_SZ) std::atomic<size_type> readPos {0 };

    public:

        MPMCQueue() noexcept
        {   // slot i starts with seq=i. this sets up the initial state
            // so that producers can claim slots starting from 0.
            // relaxed is fine — nothing else is running yet.
            for (size_t i = 0; i < Capacity; i++) {
                ringBuffer[i].seq.store(i, std::memory_order_relaxed);
            }
        }

        // only safe to call after all threads are done.
        // walks through any leftover elements and destroys them.
        ~MPMCQueue()
        {
            size_type r = readPos.load(std::memory_order_relaxed);
            const size_type w = writePos.load(std::memory_order_relaxed);
            while (r != w)
            {
                Slot& slot = ringBuffer[r & (Capacity-1)];
                const size_type seq = slot.seq.load(std::memory_order_relaxed);
                // seq == r+1 means producer finished writing here.
                // if it's anything else, producer won the CAS but didn't finish constructing — skip it.
                if (seq == r + 1) {
                    std::launder(reinterpret_cast<value_type*>(slot.buffer.data()))->~value_type();
                }
                r++;
            }
        }

        MPMCQueue(const MPMCQueue&) = delete;
        MPMCQueue& operator=(const MPMCQueue&) = delete;

        template <typename... Args>
        [[nodiscard]]
        bool emplace(Args&&... args) noexcept
        {
            // static_assert(std::is_nothrow_constructible_v<value_type, Args...>);
            size_type pos = writePos.load(std::memory_order_relaxed);
            while (true)
            {
                // TODO: (Capacity - 1) <--- ToMask
                Slot& slot = ringBuffer[pos & (Capacity-1)];
                const size_type seq = slot.seq.load(std::memory_order_acquire);
                const int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(pos);
                if (diff == 0)
                {   // slot is free. try to claim it.
                    //
                    // if we win the CAS, we get exclusive access to this slot.
                    // if we lose, CAS updates pos to whatever wpos_ actually
                    // is right now, so next iteration uses fresh data.
                    //
                    // both orders relaxed because wpos_ isn't the real
                    // synchronization point — the seq store below is.
                    if (writePos.compare_exchange_weak(pos, pos + 1,
                            std::memory_order_relaxed,
                            std::memory_order_relaxed)) {

                        // we won. nobody else will touch this slot until we publish it.
                        //
                        // about the args: they're only evaluated here, after winning the CAS.
                        // if we looped 5 times before winning, args are untouched from all those failed iterations.
                        // no partial-move problem.
                        ::new (slot.buffer.data()) value_type(std::forward<Args>(args)...);

                        // this is what actually makes the data visible.
                        // consumer does an acquire load on seq — once it sees pos+1, it's guaranteed to
                        // see the value_type we just constructed.
                        slot.seq.store(pos + 1, std::memory_order_release);
                        return true;
                    }
                    // lost the CAS. pos is already updated. go again.
                }
                else if (diff < 0) {
                    // consumer hasn't freed this slot yet. queue is full.  we bail immediately instead
                    // of spinning — caller decides what to do (retry, backoff, drop, whatever).
                    return false;
                }
                else {
                    // someone else already claimed this slot, we just had
                    // a stale wpos_. grab the current one and try again.
                    pos = writePos.load(std::memory_order_relaxed);
                }
            }
        }

        template <typename U>
        [[nodiscard]]
        bool push(U&& val) noexcept {
            return emplace(std::forward<U>(val));
        }

        // same idea as emplace but in reverse.
        // consumers race on readPos instead of wpos_.
        [[nodiscard]]
        bool pop(value_type& out) noexcept
        {
            static_assert(std::is_nothrow_move_assignable_v<value_type>);
            size_type pos = readPos.load(std::memory_order_relaxed);
            while (true)
            {
                Slot& slot = ringBuffer[pos & (Capacity-1)];
                const uint64_t seq = slot.seq.load(std::memory_order_acquire);
                const int64_t diff = static_cast<int64_t>(seq) - static_cast<int64_t>(pos + 1);
                if (diff == 0)
                {
                    // data is here. try to claim it.
                    if (readPos.compare_exchange_weak(pos, pos + 1,
                            std::memory_order_relaxed,
                            std::memory_order_relaxed)) {

                        Ty* ptr = std::launder(reinterpret_cast<value_type*>(slot.buffer.data()));
                        out = std::move(*ptr);
                        ptr->~value_type();

                        // hand the slot back to producers.
                        // pos+N because next time a producer's cursor wraps
                        // around to this slot, its pos will equal this value.
                        slot.seq.store(pos + Capacity, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0) {
                    // producer hasn't written here yet. empty.
                    return false;
                }
                else {
                    pos = readPos.load(std::memory_order_relaxed);
                }
            }
        }

        // these are racy. fine for dashboards and logging. don't make decisions based on them.
        [[nodiscard]]
        size_t size() const noexcept {
            return writePos.load(std::memory_order_relaxed) - readPos.load(std::memory_order_relaxed);
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return size() == 0;
        }

        static constexpr size_t capacity() noexcept {
            return Capacity;
        }
    };
}


namespace tests
{
#define TEST_ASSERT(cond)                                                     \
    do                                                                        \
    {                                                                         \
        if (!(cond))                                                          \
        {                                                                     \
            std::cerr << "[FAILED] " << __FUNCTION__                          \
                      << " | line: " << __LINE__                              \
                      << " | condition: " << #cond << '\n';                   \
            return false;                                                     \
        }                                                                     \
    } while (false)

#define TEST_OK()                                                             \
    do                                                                        \
    {                                                                         \
        std::cout << "[PASSED] " << __FUNCTION__ << '\n';                     \
        return true;                                                          \
    } while (false)

    struct ComplexType
    {
        int id {};
        std::string payload;

        ComplexType() = default;
        ComplexType(int i, std::string p): id(i), payload(std::move(p)) {
        }

        bool operator==(const ComplexType& other) const{
            return id == other.id && payload == other.payload;
        }
    };

    struct MoveOnly
    {
        std::unique_ptr<int> value;
        explicit MoveOnly(int v) noexcept: value(std::make_unique<int>(v)) {
        }

        MoveOnly(MoveOnly&&) noexcept = default;
        MoveOnly& operator=(MoveOnly&&) noexcept = default;

        MoveOnly(const MoveOnly&) = delete;
        MoveOnly& operator=(const MoveOnly&) = delete;
    };

    bool test_basic_push_pop()
    {
        MPMCQueue<int, 8> queue;

        TEST_ASSERT(queue.push(42));
        int value = 0;

        TEST_ASSERT(queue.pop(value));
        TEST_ASSERT(value == 42);
        TEST_OK();
    }

    bool test_pop_empty_queue()
    {
        MPMCQueue<int, 8> queue;
        int value = 0;
        TEST_ASSERT(!queue.pop(value));
        TEST_OK();
    }

    bool test_fill_queue_until_full()
    {
        constexpr size_t capacity = 4;
        MPMCQueue<int, capacity> queue;

        for (int i = 0; i < static_cast<int>(capacity); ++i){
            TEST_ASSERT(queue.push(i));
        }
        TEST_ASSERT(!queue.push(999));
        TEST_OK();
    }

    bool test_fifo_order_single_thread()
    {
        MPMCQueue<int, 16> queue;
        for (int i = 0; i < 10; ++i) {
            TEST_ASSERT(queue.push(i));
        }

        for (int i = 0; i < 10; ++i) {
            int value = -1;
            TEST_ASSERT(queue.pop(value));
            TEST_ASSERT(value == i);
        }
        TEST_OK();
    }

    bool test_wraparound_behavior()
    {
        MPMCQueue<int, 4> queue;
        for (int i = 0; i < 4; ++i) {
            TEST_ASSERT(queue.push(i));
        }

        for (int i = 0; i < 2; ++i){
            int value = -1;
            TEST_ASSERT(queue.pop(value));
            TEST_ASSERT(value == i);
        }

        TEST_ASSERT(queue.push(100));
        TEST_ASSERT(queue.push(101));

        for (constexpr int expected[] = {2, 3, 100, 101}; int const e : expected){
            int value = -1;
            TEST_ASSERT(queue.pop(value));
            TEST_ASSERT(value == e);
        }

        TEST_OK();
    }

    bool test_emplace()
    {
        MPMCQueue<ComplexType, 8> queue;
        TEST_ASSERT(queue.emplace(7, "hello"));

        ComplexType out;

        TEST_ASSERT(queue.pop(out));
        TEST_ASSERT(out.id == 7);
        TEST_ASSERT(out.payload == "hello");
        TEST_OK();
    }

    bool test_move_only_type()
    {
        MPMCQueue<MoveOnly, 8> queue;
        TEST_ASSERT(queue.push(MoveOnly(123)));

        MoveOnly out(0);

        TEST_ASSERT(queue.pop(out));
        TEST_ASSERT(out.value);
        TEST_ASSERT(*out.value == 123);
        TEST_OK();
    }


}



void Collections::mpmc_queue::TestAll()
{
    using namespace tests;

    test_basic_push_pop();
    test_pop_empty_queue();
    test_fill_queue_until_full();
    test_fifo_order_single_thread();
    test_wraparound_behavior();
    test_emplace();
    test_move_only_type();

}
