/**============================================================================
Name        : DatabaseProxy.h
Created on  : 04.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DatabaseProxy
============================================================================**/


#include "Proxy.h"
#include <vector>
#include <unordered_map>

namespace Proxy::DatabaseProxy
{
    struct IStockData
    {
        virtual ~IStockData() = default;
        virtual std::vector<std::string> getSymbols() = 0;
        virtual float getPrice(const std::string& symbol) = 0;
    };

    struct MySQLDatabase final : public IStockData
    {
        std::vector<std::string> getSymbols() override
        {
            // Query the database for the list of symbols here
            std::vector<std::string> symbols = {"AAPL", "MSFT"};
            return symbols;
        }

        float getPrice(const std::string& symbol) override
        {
            // Query the database for the stock price here
            std::cout << "Retrieving stock price for " << symbol << " from MySQL database" << std::endl;
            float price = 0.0f;
            // ...
            return price;
        }
    };

    class StockDataProxy final : public IStockData
    {
        IStockData* storage;
        std::unordered_map<std::string, float> cache;

    public:
        explicit StockDataProxy(IStockData* realSubject) : storage { realSubject } {
        }

        std::vector<std::string> getSymbols() override {
            return storage->getSymbols();
        }

        float getPrice(const std::string& symbol) override
        {
            // Check if the stock price is in the cache
            if (const auto it = cache.find(symbol); it != cache.end()) {
                std::cout << "Retrieving stock price for " << symbol << " from proxy cache" << std::endl;
                return it->second;
            }

            // If the stock price is not in the cache, forward the request to the real subject
            const float price = storage->getPrice(symbol);
            cache[symbol] = price;  // Update the proxy cache
            return price;
        }
    };
}

void Proxy::DatabaseProxyTest()
{
    using namespace Proxy::DatabaseProxy;

    std::unique_ptr<IStockData> stockData = std::make_unique<StockDataProxy>(new MySQLDatabase());

    // Retrieve the price of MSFT twice
    std::cout << "Price of MSFT: " << stockData->getPrice("MSFT") << std::endl;
    std::cout << "Price of MSFT: " << stockData->getPrice("MSFT") << std::endl;
}