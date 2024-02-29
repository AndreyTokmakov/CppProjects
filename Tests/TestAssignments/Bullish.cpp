/**============================================================================
Name        : Bullish.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Bullish
============================================================================**/

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>


namespace TestAssignment
{
    struct Result
    {
        int output1[100];
        int output2{};
        int output3{};

        Result() : output1() {
        }
    };

    struct account_balance
    {
        int account_id;
        int balance;
    };

    struct transfer
    {
        int from;
        int to;
        int amount;
    };


    std::ostream& operator<<(std::ostream& stream, const account_balance& acct)
    {
        stream << "Acct(" << acct.account_id << ", " << acct.balance << ")";
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<account_balance>& balances)
    {
        std::cout << "Balances:\n";
        for (const auto& x: balances)
            stream << "\tAcct(" << x.account_id << ", " << x.balance << ")\n";
        return stream;
    }

    using transaction = std::vector<transfer>;

    struct Database
    {
        using TransactionEntry = std::pair<transaction, size_t>;

        std::unordered_map<int, int> accounts;
        std::vector<TransactionEntry> unsettledTransaction;
        std::vector<size_t> appliedTransactions;
        size_t transactionId { 0 };

        explicit Database(const std::vector<account_balance>& acctBalances)
        {
            accounts.reserve(acctBalances.size());
            for (const account_balance& accInfo: acctBalances) {
                accounts[accInfo.account_id] = accInfo.balance;
            }
        }

        // push a transaction to the database
        void push_transaction(const transaction& transaction)
        {
            unsettledTransaction.push_back({transaction, transactionId++});
            for (const transfer& transfer: transaction)
            {
                accounts[transfer.from] -= transfer.amount;
                accounts[transfer.to] += transfer.amount;
                std::cout << "Handing Transfer(from: " << transfer.from
                          << ", to: " << transfer.to << ", amount: " << transfer.amount << ")\n";
            }
        }

        [[nodiscard]]
        bool hasOverdrawn() const
        {
            for (const auto& [acctId, balance]: accounts)
                if (0 > balance)
                    return true;
            return false;
        }

        // settle the database such that invariant is maintained and best state of the database is present
        void settle()
        {
            int idx = unsettledTransaction.size() - 1;
            while (hasOverdrawn()) {
                const TransactionEntry &transaction = unsettledTransaction[idx--];
                for (const auto &transfer: transaction.first) {
                    accounts[transfer.from] += transfer.amount;
                    accounts[transfer.to] -= transfer.amount;

                    std::cout << "\tRollback(from: " << transfer.from << ", to: "
                              << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }

            unsettledTransaction.resize(idx + 1);
            for (const TransactionEntry& transactionEntry: unsettledTransaction)
                appliedTransactions.push_back(transactionEntry.second);

            unsettledTransaction.clear();
        }

        // return a listing of all balances in any order
        [[nodiscard]]
        std::vector<account_balance> get_balances() const
        {
            std::vector<account_balance> balances;
            balances.reserve(accounts.size());

            for (const auto& [acctId, balance]: accounts)
                balances.push_back({acctId, balance});

            return balances;
        }

        // Given the logical sequence of transactions constructed by the ordered sequence of calls
        // to push_transaction AND having called settle()
        //
        // return the 0-based indices of the surviving transactions in that sequence which, when
        // applied to the initial balances produce a state where the invariant is maintained
        //

        [[nodiscard]]
        std::vector<size_t> get_applied_transactions() const
        {
            return appliedTransactions;
        }

        void stats()
        {
            std::cout << "Balances:\n";
            for (const auto& [acctId, balance]: accounts)
                std::cout << "\tAcct(" << acctId << ", " << balance << ")\n";


            std::cout << "Applied:\n";
            for (size_t idx: appliedTransactions)
                std::cout << idx << ", ";
            std::cout << std::endl;

            /*
            for (const TransactionEntry& transaction: unsettledTransaction)
            {
                for (const transfer& transfer: transaction.first)
                {
                    std::cout << "\tfrom: " << transfer.from<< ", to: "
                        << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }
             */
        }
    };

    Database create_database(const std::vector<account_balance> &initial_balance) {
        return Database{initial_balance};
    }

    void Test()
    {
        const std::vector<account_balance> initialBalances {
                {1, 10},
                {2, 5}
        };

        const transaction& transaction {
                {1, 2, 5}
        };


        Database db = create_database(initialBalances);
        db.push_transaction(transaction);

        std::cout << db.get_balances() << std::endl;

        db.settle();

        std::cout << db.get_balances() << std::endl;

        db.stats();
    }
}

namespace TestAssignment2
{
    struct Result
    {
        int output1[100];
        int output2{};
        int output3{};

        Result() : output1() {
        }
    };

    struct account_balance
    {
        int account_id;
        int balance;
    };

    struct transfer
    {
        int from;
        int to;
        int amount;
    };


    std::ostream& operator<<(std::ostream& stream, const account_balance& acct)
    {
        stream << "Acct(" << acct.account_id << ", " << acct.balance << ")";
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<account_balance>& balances)
    {
        std::cout << "Balances:\n";
        for (const auto& x: balances)
            stream << "\tAcct(" << x.account_id << ", " << x.balance << ")\n";
        return stream;
    }

    using transaction = std::vector<transfer>;

    struct Database
    {
        using TransactionEntry = std::pair<transaction, size_t>;

        std::unordered_map<int, int> accounts;
        std::vector<TransactionEntry> unsettledTransaction;
        std::vector<size_t> appliedTransactions;
        size_t transactionId { 0 };

        explicit Database(const std::vector<account_balance>& acctBalances)
        {
            accounts.reserve(acctBalances.size());
            for (const account_balance& accInfo: acctBalances) {
                accounts[accInfo.account_id] = accInfo.balance;
            }
        }

        // push a transaction to the database
        void push_transaction(const transaction& transaction)
        {
            const size_t transId = transactionId++;
            for (const transfer& transfer: transaction) {
                accounts[transfer.from] -= transfer.amount;
                accounts[transfer.to] += transfer.amount;
            }

            if (hasSucceeded()) {
                unsettledTransaction.clear();
                appliedTransactions.push_back(transId);
            }
            else {
                unsettledTransaction.emplace_back(transaction, transId);
            }
        }

        [[nodiscard]]
        bool hasSucceeded() const
        {
            return std::all_of(accounts.cbegin(), accounts.cend(), [](const auto& acct) {
                return acct.second >= 0;
            });
        }

        // settle the database such that invariant is maintained and best state of the database is present
        void settle()
        {
            for (const auto& transaction: unsettledTransaction){
                for (const auto &transfer: transaction.first) {
                    accounts[transfer.from] += transfer.amount;
                    accounts[transfer.to] -= transfer.amount;
                }
            }

            unsettledTransaction.clear();
        }

        // return a listing of all balances in any order
        [[nodiscard]]
        std::vector<account_balance> get_balances() const
        {
            std::vector<account_balance> balances;
            balances.reserve(accounts.size());

            for (const auto& [acctId, balance]: accounts)
                balances.push_back({acctId, balance});

            return balances;
        }

        // Given the logical sequence of transactions constructed by the ordered sequence of calls
        // to push_transaction AND having called settle()
        //
        // return the 0-based indices of the surviving transactions in that sequence which, when
        // applied to the initial balances produce a state where the invariant is maintained
        //

        [[nodiscard]]
        std::vector<size_t> get_applied_transactions() const
        {
            return appliedTransactions;
        }

        void stats()
        {
            std::cout << "Balances:\n";
            for (const auto& [acctId, balance]: accounts)
                std::cout << "\tAcct(" << acctId << ", " << balance << ")\n";


            std::cout << "Applied:\n";
            for (size_t idx: appliedTransactions)
                std::cout << idx << ", ";
            std::cout << std::endl;

            /*
            for (const TransactionEntry& transaction: unsettledTransaction)
            {
                for (const transfer& transfer: transaction.first)
                {
                    std::cout << "\tfrom: " << transfer.from<< ", to: "
                        << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }
             */
        }
    };

    Database create_database(const std::vector<account_balance> &initial_balance) {
        return Database{initial_balance};
    }

    void Test()
    {
        const std::vector<account_balance> initialBalances {
                {1, 10},
                {2, 5}
        };

        const transaction& transaction {
                {1, 2, 3}
        };


        Database db = create_database(initialBalances);
        db.push_transaction(transaction);
        db.push_transaction(transaction);

        db.settle();
        db.stats();

        db.push_transaction(transaction);
        db.push_transaction(transaction);

        std::cout << "--------------\n";

        db.stats();

        db.settle();
        db.stats();
    }
}

namespace TestAssignment3
{
    struct Result
    {
        int output1[100];
        int output2{};
        int output3{};

        Result() : output1() {
        }
    };

    struct account_balance
    {
        int account_id;
        int balance;
    };

    struct transfer
    {
        int from;
        int to;
        int amount;
    };


    std::ostream& operator<<(std::ostream& stream, const account_balance& acct)
    {
        stream << "Acct(" << acct.account_id << ", " << acct.balance << ")";
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, const std::vector<account_balance>& balances)
    {
        std::cout << "Balances:\n";
        for (const auto& x: balances)
            stream << "\tAcct(" << x.account_id << ", " << x.balance << ")\n";
        return stream;
    }

    using transaction = std::vector<transfer>;

    struct Database
    {
        using TransactionEntry = std::pair<transaction, size_t>;

        std::unordered_map<int, int> accounts;
        std::vector<TransactionEntry> unsettledTransaction;
        std::vector<size_t> appliedTransactions;
        size_t transactionId { 0 };

        explicit Database(const std::vector<account_balance>& acctBalances)
        {
            accounts.reserve(acctBalances.size());
            for (const account_balance& accInfo: acctBalances) {
                accounts[accInfo.account_id] = accInfo.balance;
            }
        }

        // push a transaction to the database
        void push_transaction(const transaction& transaction)
        {
            unsettledTransaction.emplace_back(transaction, transactionId++);
            for (const transfer& transfer: transaction)
            {
                accounts[transfer.from] -= transfer.amount;
                accounts[transfer.to] += transfer.amount;
            }
        }

        [[nodiscard]]
        bool hasOverdrawn() const
        {
            return std::any_of(accounts.cbegin(), accounts.cend(), [](const auto& acct) {
                return 0 > acct.second;
            });
        }

        // settle the database such that invariant is maintained and best state of the database is present
        void settle()
        {
            // We process all transactions in reverse order until we find the most recent one with a positive balance
            int idx = static_cast<int>(unsettledTransaction.size()) - 1;
            while (hasOverdrawn()) {
                const TransactionEntry &transaction = unsettledTransaction[idx--];
                for (const auto &transfer: transaction.first) {
                    accounts[transfer.from] += transfer.amount;
                    accounts[transfer.to] -= transfer.amount;
                }
            }

            unsettledTransaction.resize(idx + 1);
            for (const TransactionEntry& transactionEntry: unsettledTransaction)
                appliedTransactions.push_back(transactionEntry.second);

            unsettledTransaction.clear();
        }

        // return a listing of all balances in any order
        [[nodiscard]]
        std::vector<account_balance> get_balances() const
        {
            std::vector<account_balance> balances;
            balances.reserve(accounts.size());

            for (auto& [acctId, balance]: accounts)
                balances.emplace_back(acctId, balance);

            return balances;
        }

        // Given the logical sequence of transactions constructed by the ordered sequence of calls
        // to push_transaction AND having called settle()
        //
        // return the 0-based indices of the surviving transactions in that sequence which, when
        // applied to the initial balances produce a state where the invariant is maintained
        //

        [[nodiscard]]
        std::vector<size_t> get_applied_transactions() const
        {
            return appliedTransactions;
        }

        void stats()
        {
            std::cout << "Balances:\n";
            for (const auto& [acctId, balance]: accounts)
                std::cout << "\tAcct(" << acctId << ", " << balance << ")\n";


            std::cout << "Applied:\n";
            for (size_t idx: appliedTransactions)
                std::cout << idx << ", ";
            std::cout << std::endl;

            /*
            for (const TransactionEntry& transaction: unsettledTransaction)
            {
                for (const transfer& transfer: transaction.first)
                {
                    std::cout << "\tfrom: " << transfer.from<< ", to: "
                        << transfer.to << ", amount: " << transfer.amount << ") ID = " << transaction.second << "\n";
                }
            }
             */
        }
    };

    Database create_database(const std::vector<account_balance> &initial_balance) {
        return Database{initial_balance};
    }

    void Test()
    {
        const std::vector<account_balance> initialBalances {
                {1, 10},
                {2, 5}
        };

        const transaction& transaction {
                {1, 2, 3}
        };


        Database db = create_database(initialBalances);
        db.push_transaction(transaction);
        db.push_transaction(transaction);

        db.settle();
        db.stats();

        db.push_transaction(transaction);
        db.push_transaction(transaction);

        std::cout << "--------------\n";

        db.stats();

        db.settle();
        db.stats();
    }
}



void Bullish_Tests()
{
    TestAssignment::Test();
    TestAssignment2::Test();
    TestAssignment3::Test();
}

