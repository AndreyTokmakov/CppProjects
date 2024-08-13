/**============================================================================
Name        : MatchingEngineWithAlloc.cpp
Created on  : 12.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : MatchingEngineWithAlloc.cpp
============================================================================**/

#include "Utilities.h"
#include "Order.h"

#include <iostream>
#include <numeric>
#include <list>
#include <forward_list>
#include <vector>
#include <unordered_map>
#include <cstdint>

#include <boost/container/flat_map.hpp>

namespace
{
    uint64_t getNextOrderID()
    {
        static uint64_t id { 1'000 };
        return id++;
    }
}

namespace Memory
{
    template <typename Ty, typename Allocator = std::allocator<Ty>>
    class ObjectPool final
    {
        using object_type = Ty;
        using pointer = object_type*;
        using size_type = typename std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

    private:
        std::vector<pointer> pool;
        std::vector<pointer> available;

        static constexpr size_type DEFAULT_CHUNK_SIZE { 5 };
        static constexpr size_type GROWTH_STRATEGY { 2 };

        size_type _new_block_size { DEFAULT_CHUNK_SIZE };
        size_type _size { 0 };
        size_type _capacity { 0 };

        void addChunk()
        {
            // Allocate a new chunk of uninitialized memory
            pointer newBlock { m_allocator.allocate(_new_block_size) };

            // Keep all allocated blocks in 'pool' to delete them later:
            pool.push_back(newBlock);

            available.resize(_new_block_size);
            std::iota(std::begin(available), std::end(available), newBlock);

            _capacity += _new_block_size;
            _new_block_size *= GROWTH_STRATEGY;
        }

        // The allocator to use for allocating and deallocating chunks.
        Allocator m_allocator;

    public:

        struct Deleter final
        {
            ObjectPool* pool {nullptr};

            void operator()(pointer object) const noexcept
            {
                std::destroy_at(object);

                // Return object mem pointer back to pool
                pool->available.push_back(object);
                --pool->_size;
            }
        };

    public:
        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

    public:
        ObjectPool() = default;

        explicit ObjectPool(const Allocator& allocator) : m_allocator{ allocator } {
            // Trivial
        }

        virtual ~ObjectPool()
        {   // Note: this implementation assumes that all objects handed out by this
            // pool have been returned to the pool before the pool is destroyed.
            // The following statement asserts if that is not the case.
            assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

            // Deallocate all allocated memory.
            size_t chunkSize{ DEFAULT_CHUNK_SIZE };
            for (auto* chunk : pool) {
                m_allocator.deallocate(chunk, chunkSize);
                chunkSize *= GROWTH_STRATEGY;
            }
        }

        // Allow move construction and move assignment.
        ObjectPool(ObjectPool&& src) noexcept = default;
        ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

        // Prevent copy construction and copy assignment.
        ObjectPool(const ObjectPool& src) = delete;
        ObjectPool& operator=(const ObjectPool& rhs) = delete;

        // Reserves and returns an object from the pool. Arguments can be
        // provided which are perfectly forwarded to a constructor of T.
        template<typename... Args>
        std::unique_ptr<object_type, Deleter> acquireObject(Args... args)
        {
            // If there are no free objects, allocate a new chunk.
            if (available.empty()) {
                addChunk();
            }

            // Get a free object.
            const pointer objectPtr { available.back() };

            // Initialize, i.e. construct, an instance of T in an uninitialized block of memory
            // using placement new, and perfectly forward any provided arguments to the constructor.
            pointer obj = new (objectPtr) object_type { std::forward<Args>(args)... };

            // Remove the object from the list of free objects.
            available.pop_back();
            ++_size;

            // Wrap the initialized object and return it.
            return std::unique_ptr<object_type, Deleter> { objectPtr, Deleter{this}};
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return _size;
        }

        [[nodiscard]]
        size_type capacity() const noexcept {
            return _capacity;
        }
    };
}

namespace MatchingEngine_NO_WithAllocator
{
    using namespace Common;

    struct OrderMatchingEngine
    {
        using OrderIter = typename std::list<Order>::iterator;
        using PriceOrderList = std::list<OrderIter>;
        using PriceOrderIter = typename PriceOrderList::iterator;

        struct ReferencesBlock
        {
            OrderIter orderIter;
            PriceOrderIter priceOrderIter;
            PriceOrderList* priceLevelOrderList;
        };

        Memory::ObjectPool<Order> ordersPool;
        std::list<Order> orders {};

        std::unordered_map<Order::IDType, ReferencesBlock> orderByIDMap;

        boost::container::flat_map<Order::Price, PriceOrderList, std::less<>> buyOrders;
        boost::container::flat_map<Order::Price, PriceOrderList, std::greater<>> sellOrders;

        void processOrder(Order& order)
        {
            switch (order.action)
            {
                case OrderActionType::NEW:
                    return handleOrderNew(order);
                case OrderActionType::CANCEL:
                    return handleOrderCancel(order);
                case OrderActionType::AMEND:
                    return handleOrderAmend(order);
                default:
                    return;
            }
        }

        unsigned long long matchOrder(Order& order)
        {
            // TODO:
            return 1;
        }

        template<typename OrderSideMap>
        void matchOrder(Order& order, OrderSideMap& oppositeSideOrdersPriceMap)
        {
            // TODO:
        }


        void matchOrderList(Order& order,
                            PriceOrderList& matchedOrderList)
        {
            // TODO:
        }

        void handleOrderNew(Order& order)
        {
            if (0 == matchOrder(order)) {
                return;
            }

            const auto [iterOrderMap, inserted] = orderByIDMap.emplace(
                    order.orderId, ReferencesBlock{});
            if (inserted)
            {
                auto& [orderIter, priceOrderIter, priceLevelOrderList] =
                        iterOrderMap->second;
                orderIter = orders.insert(orders.end(), order);
                priceLevelOrderList = (OrderSide::BUY == order.side) ?
                                      &buyOrders[order.price] : &sellOrders[order.price];
                priceOrderIter = priceLevelOrderList->insert(priceLevelOrderList->end(), orderIter);
            }
        }

        void handleOrderCancel(Order& order)
        {
            // TODO:
        }

        void handleOrderAmend(Order& order)
        {
            // TODO:
        }

        void info(bool printTrades = true)
        {
            // TODO:
        }
    };
}

namespace MatchingEngine_WithAllocator
{
    using namespace Common;


    struct OrderMatchingEngine
    {
        using OrderPtr = std::unique_ptr<Order, Memory::ObjectPool<Order>::Deleter>;
        using OrderPtrIter = typename std::list<OrderPtr>::iterator;
        using PriceOrderList = std::list<OrderPtrIter>;
        using PriceOrderIter = typename PriceOrderList::iterator;

        struct ReferencesBlock
        {
            OrderPtrIter orderIter;
            PriceOrderIter priceOrderIter;
            PriceOrderList* priceLevelOrderList;
        };

        Memory::ObjectPool<Order> ordersPool;
        std::list<OrderPtr> orders {};

        std::unordered_map<Order::IDType, ReferencesBlock> orderByIDMap;

        boost::container::flat_map<Order::Price, PriceOrderList, std::less<>> buyOrders;
        boost::container::flat_map<Order::Price, PriceOrderList, std::greater<>> sellOrders;

        OrderPtr makeOrder() {
            return ordersPool.acquireObject();
        }

        void processOrder(OrderPtr&& order)
        {
            switch (order->action)
            {
                case OrderActionType::NEW:
                    return handleOrderNew(std::move(order));
                case OrderActionType::CANCEL:
                    return handleOrderCancel(std::move(order));
                case OrderActionType::AMEND:
                    return handleOrderAmend(std::move(order));
                default:
                    return;
            }
        }

        unsigned long long matchOrder(Order& order)
        {
            // TODO:
            return 1;
        }

        template<typename OrderSideMap>
        void matchOrder(Order& order, OrderSideMap& oppositeSideOrdersPriceMap)
        {
            // TODO:
        }


        void matchOrderList(Order& order,
                            PriceOrderList& matchedOrderList)
        {
            // TODO:
        }

        void handleOrderNew(OrderPtr&& orderIn)
        {
            Order& order = *orderIn.get();
            if (0 == matchOrder(order)) {
                return;
            }

            const auto [iterOrderMap, inserted] = orderByIDMap.emplace(
                    order.orderId, ReferencesBlock{});
            if (inserted)
            {
                auto& [orderIter, priceOrderIter, priceLevelOrderList] =
                        iterOrderMap->second;
                orderIter = orders.insert(orders.end(), std::move(orderIn));
                priceLevelOrderList = (OrderSide::BUY == order.side) ?
                                      &buyOrders[order.price] : &sellOrders[order.price];
                priceOrderIter = priceLevelOrderList->insert(priceLevelOrderList->end(), orderIter);
            }
        }

        void handleOrderCancel(OrderPtr&& order)
        {
            // TODO:
        }

        void handleOrderAmend(OrderPtr&& order)
        {
            // TODO:
        }

        void info(bool printTrades = true)
        {
            // TODO:
        }
    };
}


namespace MatchingEngine_NO_WithAllocator::Tests
{

    void Trade_SELL()
    {
        OrderMatchingEngine engine;

        Utilities::ScopedTimer timer { "TEST"};
        for (int i = 0, price = 10; i < 8'000'000; ++i)
        {
            if (price > 16)
                price = 10;

            Order order;
            order.side = OrderSide::BUY;
            order.price = price+=2;
            order.quantity = 3;
            order.orderId = getNextOrderID();

            engine.processOrder(order);
        }

        std::cout << engine.ordersPool.size() << std::endl;

    }
}

namespace MatchingEngine_WithAllocator::Tests
{
    void Trade_SELL()
    {
        OrderMatchingEngine engine;

        Utilities::ScopedTimer timer { "TEST"};
        for (int i = 0, price = 10; i < 8'000'000; ++i)
        {
            if (price > 16)
                price = 10;

            OrderMatchingEngine::OrderPtr order = engine.makeOrder();
            order->side = OrderSide::BUY;
            order->price = price+=2;
            order->quantity = 3;
            order->orderId = getNextOrderID();

            engine.processOrder(std::move(order));
        }

        std::cout << engine.ordersPool.size() << std::endl;

    }
}


void MatchingEngine_WithAllocator_Tests()
{
    // using namespace MatchingEngine_NO_WithAllocator::Tests;
    using namespace MatchingEngine_WithAllocator::Tests;

    Trade_SELL();
}