/**============================================================================
Name        : SgnalSlotTrading.cpp
Created on  : 16.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SgnalSlotTrading.cpp
============================================================================**/

#include "SignalSlot.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
    template<typename... Args>
    class Signal
    {
    private:
        struct SlotEntry
        {
            std::function<void(Args...)> callback;
            bool connected = true;
        };

    public:
        class Connection
        {
        public:
            Connection() = default;

            void disconnect() const noexcept
            {
                if (const std::shared_ptr<SlotEntry> entry = slot.lock()) {
                    entry->connected = false;
                }
            }

            [[nodiscard]]
            bool isConnected() const noexcept
            {
                if (const std::shared_ptr<SlotEntry> entry = slot.lock()) {
                    return entry->connected;
                }
                return false;
            }

        private:
            friend class Signal;

            explicit Connection(const std::shared_ptr<SlotEntry>& entry) noexcept
                : slot(entry)
            {
            }

            std::weak_ptr<SlotEntry> slot;
        };

        class ScopedConnection
        {
        public:
            explicit ScopedConnection(Connection connection) noexcept
                : connection(std::move(connection))
            {
            }

            ScopedConnection(const ScopedConnection&) = delete;
            ScopedConnection& operator=(const ScopedConnection&) = delete;

            ScopedConnection(ScopedConnection&& other) noexcept
                : connection(std::move(other.connection))
            {
            }

            ScopedConnection& operator=(ScopedConnection&& other) noexcept
            {
                if (this != &other)
                {
                    disconnect();
                    connection = std::move(other.connection);
                }

                return *this;
            }

            ~ScopedConnection()
            {
                disconnect();
            }

            void disconnect() noexcept
            {
                connection.disconnect();
            }

        private:
            Connection connection;
        };

        using Slot = std::function<void(Args...)>;

        [[nodiscard]]
        Connection connect(Slot slot)
        {
            std::erase_if(slots, [](const std::shared_ptr<SlotEntry>& entry)
            {
                return !entry->connected;
            });

            const std::shared_ptr<SlotEntry> entry = std::make_shared<SlotEntry>(std::move(slot));
            slots.push_back(entry);

            return Connection(entry);
        }

        void emit(Args... args) const
        {
            const std::vector<std::shared_ptr<SlotEntry>> snapshot = slots;

            for (const std::shared_ptr<SlotEntry>& entry : snapshot)
            {
                if (entry->connected)
                    entry->callback(args...);
            }
        }

    private:
        std::vector<std::shared_ptr<SlotEntry>> slots;
    };

    enum class Side : std::uint8_t
    {
        Buy,
        Sell
    };

    struct BookUpdate
    {
        Side side = Side::Buy;
        std::uint64_t price = 0;
        std::uint64_t quantity = 0;
    };

    struct Trade
    {
        std::uint64_t price = 0;
        std::uint64_t quantity = 0;
    };

    class MarketDataFeed
    {
    public:
        Signal<const BookUpdate&> onBookUpdate;
        Signal<const Trade&> onTrade;

        void publishBookUpdate(const BookUpdate& update) const
        {
            onBookUpdate.emit(update);
        }

        void publishTrade(const Trade& trade) const
        {
            onTrade.emit(trade);
        }
    };

    class OrderBookBuilder
    {
    public:
        explicit OrderBookBuilder(MarketDataFeed& feed):
            connection(feed.onBookUpdate.connect([this](const BookUpdate& update)
        {
            applyUpdate(update);
        }))
        {
        }

        [[nodiscard]]
        std::uint64_t getBestBid() const noexcept{
            return bids.empty() ? 0 : bids.rbegin()->first;
        }

        [[nodiscard]]
        std::uint64_t getBestAsk() const noexcept {
            return asks.empty() ? 0 : asks.begin()->first;
        }

        [[nodiscard]]
        std::size_t getBidLevelsCount() const noexcept {
            return bids.size();
        }

        [[nodiscard]]
        std::size_t getAskLevelsCount() const noexcept {
            return asks.size();
        }

    private:
        void applyUpdate(const BookUpdate& update)
        {
            std::map<std::uint64_t, std::uint64_t>& levels =
                update.side == Side::Buy ? bids : asks;

            if (update.quantity == 0)
                levels.erase(update.price);
            else
                levels.insert_or_assign(update.price, update.quantity);
        }

        std::map<std::uint64_t, std::uint64_t> bids;
        std::map<std::uint64_t, std::uint64_t> asks;

        Signal<const BookUpdate&>::ScopedConnection connection;
    };

    class MarketDataLogger
    {
    public:
        explicit MarketDataLogger(MarketDataFeed& feed) :
            connection(feed.onBookUpdate.connect([this](const BookUpdate& update){ log(update);}))
        {
        }

    private:
        void log(const BookUpdate& update) const
        {
            const char* side = update.side == Side::Buy ? "BUY" : "SELL";
            std::cout << "[Logger] " << side << " Price: " << update.price << " Quantity: " << update.quantity << '\n';
        }

        Signal<const BookUpdate&>::ScopedConnection connection;
    };

    class MetricsCollector
    {
    public:
        explicit MetricsCollector(MarketDataFeed& feed):
            bookConnection(feed.onBookUpdate.connect([this](const BookUpdate& update) {
                ++bookUpdates;
            })),
            tradeConnection(feed.onTrade.connect([this](const Trade& trade) {
                ++trades;
                tradedVolume += trade.quantity;
            }))
        {
        }

        void print() const
        {
            std::cout << "\n[Metrics]\n";
            std::cout << "Book updates: " << bookUpdates << '\n';
            std::cout << "Trades: " << trades << '\n';
            std::cout << "Traded volume: " << tradedVolume << '\n';
        }

    private:
        std::uint64_t bookUpdates = 0;
        std::uint64_t trades = 0;
        std::uint64_t tradedVolume = 0;

        Signal<const BookUpdate&>::ScopedConnection bookConnection;
        Signal<const Trade&>::ScopedConnection tradeConnection;
    };
}

void SignalSlot::SignalSlotTrading::demo()
{
    MarketDataFeed feed;

    OrderBookBuilder orderBook(feed);
    MarketDataLogger logger(feed);
    MetricsCollector metrics(feed);

    constexpr std::uint64_t BidPrice = 100;
    constexpr std::uint64_t AskPrice = 102;
    constexpr std::uint64_t Quantity = 10;

    std::cout << "--- Market Data Updates ---\n";

    feed.publishBookUpdate({.side = Side::Buy, .price = BidPrice, .quantity = Quantity});
    feed.publishBookUpdate({.side = Side::Sell, .price = AskPrice, .quantity = Quantity});

    feed.publishTrade({BidPrice, 5});
    feed.publishTrade({AskPrice, 3});

    std::cout << "\n--- OrderBook ---\n";
    std::cout << "Best Bid: " << orderBook.getBestBid() << '\n';
    std::cout << "Best Ask: " << orderBook.getBestAsk() << '\n';
    std::cout << "Bid Levels: " << orderBook.getBidLevelsCount() << '\n';
    std::cout << "Ask Levels: " << orderBook.getAskLevelsCount() << '\n';

    metrics.print();
}
