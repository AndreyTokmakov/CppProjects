/**============================================================================
Name        : MySQL.cpp
Created on  : 05.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "MySQL.h"

#include <memory>
#include <chrono>
#include <functional>
#include <iostream>
#include <mysql/mysql.h>

namespace MySqlClientLegacy
{
    const std::string usersTableName = "users";
    const std::string TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

    constexpr int32_t retriesCountMax = 2;
    constexpr int32_t connectionRestoreMax = 5;

    enum class SQLError
    {
        SUCCESS = 0,
        TOO_MANY_CONNECTIONS = 1040,
        SEVER_SHUTDOWN_IN_PROGRESS = 1053,
        UNKNOWN_ERROR = 2000,
        SOCKET_CREATE_ERROR = 2001,
        CONNECTION_ERROR = 2002,      //  Can't connect to local MySQL server through socket
        CONNECTION_HOST_ERROR = 2003, //  Can't connect to MySQL serve
        IP_SOCK_ERROR = 2004,
        UNKNOWN_HOST = 2005,
        SERVER_GONE_ERROR = 2006,
        SERVER_HANDSHAKE_ERR = 2012,
        SERVER_LOST = 2013,            //  Lost connection to MySQL server during query
        SSL_CONNECTION_ERROR = 2026,
        LOST_CONNECTION_TO_MySQL = 2055
    };


    bool operator==(const SQLError sqlError, const int32_t errorCode)
    {
        return static_cast<int>(sqlError) == errorCode;
    }

    bool operator!=(const SQLError sqlError, const int32_t errorCode)
    {
        return static_cast<int>(sqlError) != errorCode;
    }

    bool operator==(const int32_t errorCode, const SQLError sqlError)
    {
        return sqlError == errorCode;
    }

    bool operator!=(const int32_t errorCode, const SQLError sqlError)
    {
        return sqlError != errorCode;
    }

    /**
     * This local static function here just to encapsulate this not-so-good way to
     * cast/convert MYSQL_ROW cell entry to INT using atoi()
     * atoi() kind of not the best way to do that. Need to use std::from_chars(...) from the C++17
    **/
    int32_t getAsInt(const MYSQL_ROW& row,
                     const uint32_t idx)
    {
        return std::stoi(row[idx]);
    }

    uint64_t getAsULong(const MYSQL_ROW& row,
                        const uint32_t idx)
    {
        return std::stoull(row[idx]);
    }

    struct BindParam
    {
        enum_field_types buffer_type { enum_field_types::MYSQL_TYPE_DECIMAL };
        void *buffer { nullptr };
        size_t length { 0 };

        BindParam(const enum_field_types buffer_type, void *buffer, const size_t length):
                buffer_type {buffer_type}, buffer {buffer}, length { length } {
        }
    };

    std::vector<MYSQL_BIND> createSqlBinding(std::vector<BindParam>& bindings)
    {
        std::vector<MYSQL_BIND> params(bindings.size());
        for (size_t idx = 0; idx < params.size(); ++idx)
        {
            params[idx].buffer_type = bindings[idx].buffer_type;
            params[idx].buffer = bindings[idx].buffer;
            params[idx].length = &(bindings[idx].length);
        }
        return params;
    }


    struct UserDTO
    {
        explicit UserDTO(std::string name, std::string description):
            name { std::move(name) }, description { std::move(description) } {
        }

        uint32_t id {0};
        std::string name {};
        std::string description {};
        std::chrono::system_clock::time_point timestamp { std::chrono::system_clock::now() };

        std::vector<std::string> dataParts;
    };

    class SQLStorage
    {
        struct ResultSetDeleter
        {
            void operator()(MYSQL_RES *resultSet) const {
                mysql_free_result(resultSet);
            }
        };

        struct ConnectionDeleter
        {
            void operator()(MYSQL* connection) const {
                mysql_close(connection);
            }
        };

        struct StatementDeleter
        {
            void operator()(MYSQL_STMT* statement) const {
                mysql_stmt_close(statement);
            }
        };

        template<typename CommitFunc, typename RollbackFunc, class ObjectType>
        struct CommitAdapter
        {
            CommitAdapter(ObjectType* ptrObj, CommitFunc commitFunc, RollbackFunc rollbackFunc) :
                    objectPtr { ptrObj }, commitCallback { commitFunc }, rollbackCallback { rollbackFunc }, succeeded { false } {
            }

            ~CommitAdapter()
            {
                if (succeeded)
                    std::invoke(commitCallback, objectPtr);
                else
                    std::invoke(rollbackCallback, objectPtr);
            }

            void success() noexcept {
                succeeded = true;
            }

        private:

            ObjectType* objectPtr { nullptr };
            CommitFunc commitCallback {};
            RollbackFunc rollbackCallback {};
            bool succeeded { false };
        };

        void establishSQLConnection();

        [[nodiscard]]
        bool commitChanges() const;

        [[nodiscard]]
        bool rollbackChanges() const;

        template<typename CommitFunc = decltype(&SQLStorage::commitChanges),
                 typename RollbackFunc = decltype(&SQLStorage::rollbackChanges),
                 typename ObjectType = SQLStorage>
        CommitAdapter<CommitFunc, RollbackFunc, ObjectType> getTransaction()
        {
            return CommitAdapter {this,
                                  &SQLStorage::commitChanges,
                                  &SQLStorage::rollbackChanges
            };
        }

        [[nodiscard]]
        bool restoreConnection() noexcept;

        [[nodiscard]]
        std::unique_ptr<MYSQL_STMT, StatementDeleter> createStatement() const;

        [[nodiscard]]
        std::unique_ptr<MYSQL_STMT, StatementDeleter> createStatement(const std::string& stmtString);

        static std::unique_ptr<MYSQL_RES, ResultSetDeleter> wrapResultSet(MYSQL_RES* resultSetPtr);

        [[nodiscard]]
        static MYSQL_ROW fetchRow(MYSQL_RES *result) ;

        [[nodiscard]]
        uint32_t getInsertedUserId() const;

        bool createTable(const std::string& tableName,
                         const std::string& ddlString);
        bool createUsersTable();

        [[nodiscard]]
        bool isTableExists(const std::string& tableName) const;

        [[nodiscard]]
        bool isConnected() const noexcept;

        [[nodiscard]]
        std::unique_ptr<MYSQL_RES, ResultSetDeleter> executeQuery( const std::string& query) const;

        static bool executeStatement(MYSQL_STMT* statement);

        static bool executePreparedStatement(MYSQL_STMT* statement,
                                      std::vector<MYSQL_BIND>& params);

    public:
        explicit SQLStorage(std::string hostname,
                            std::string user,
                            std::string pass,
                            std::string schema);
        ~SQLStorage();


    public:

        bool insertUser(UserDTO& user);

    private:

        std::string host;
        std::string username;
        std::string password;
        std::string database;

        std::unique_ptr<MYSQL, ConnectionDeleter> sqlConnection;
    };

    SQLStorage::SQLStorage(std::string hostname,
                           std::string user,
                           std::string pass,
                           std::string schema) :
            host { std::move(hostname) },
            username { std::move(user) },
            password { std::move(pass) },
            database { std::move(schema) }
    {
        if (SQLError::SUCCESS != mysql_library_init(0, nullptr, nullptr)) {
            throw std::runtime_error("Failed to initialize MySQL client library");
        }

        establishSQLConnection();

        if (!isTableExists(usersTableName) && !createUsersTable()) {
            throw std::runtime_error("Failed to create a " + usersTableName + " table");
        }
    }

    SQLStorage::~SQLStorage() {
        mysql_library_end();
    }

    bool SQLStorage::isConnected() const noexcept {
        return SQLError::SUCCESS == mysql_ping(sqlConnection.get());
    }

    /**
     *  Tries to establish a connection with the SQL server.
     *  @Throws: an exception is thrown in case of an error
     *  @Return: None
     */
    void SQLStorage::establishSQLConnection()
    {
        std::unique_ptr<MYSQL, ConnectionDeleter> conn (mysql_init(nullptr), ConnectionDeleter{});
        if (!conn) {
            throw std::runtime_error("Failed to initialize the MYSQL object");
        }
        if (!mysql_real_connect(conn.get(), host.c_str(),username.c_str(),password.c_str(),
                                database.c_str(),0,nullptr,CLIENT_FOUND_ROWS))
        {
            throw std::runtime_error("Failed to establish a MySQL connection");
        }
        if (SQLError::SUCCESS != mysql_autocommit(conn.get(), false))
        {
            throw std::runtime_error("Failed to disable AutoCommit");
        }

        /*
        const bool optValue = true;
        if (SQLError::SUCCESS != mysql_options(conn.get(), MYSQL_OPT_RECONNECT, &optValue)) {
            throw std::runtime_error("Failed to enable AUTO_RECONNECT option ");
        }*/

        sqlConnection = std::move(conn);
    }

    bool SQLStorage::restoreConnection() noexcept
    {
        for (int32_t n = 0; n < connectionRestoreMax; ++n) {
            try {
                establishSQLConnection();
                return true;
            } catch (const std::runtime_error& exc) { /** **/ }
        }
        return false;
    }

    bool SQLStorage::commitChanges() const
    {
        return SQLError::SUCCESS == mysql_commit(sqlConnection.get());
    }

    bool SQLStorage::rollbackChanges() const
    {
        return SQLError::SUCCESS == mysql_rollback(sqlConnection.get());
    }

    MYSQL_ROW SQLStorage::fetchRow(MYSQL_RES *result)
    {
        return mysql_fetch_row(result);
    }

    [[nodiscard]]
    bool SQLStorage::isTableExists(const std::string& tableName) const
    {
        const std::unique_ptr<MYSQL_RES, ResultSetDeleter> resultSet = executeQuery("SHOW TABLES LIKE '" + tableName + "'");
        if (!resultSet) { // TODO: Put error logs
            return false;
        }

        MYSQL_ROW row { nullptr };
        while ((row = fetchRow(resultSet.get()))) {
            if (row[0] == tableName)
                return true;
        }
        return false;
    }

     [[nodiscard]]
    bool SQLStorage::createTable(const std::string& tableName,
                                 const std::string& ddlString)
    {
        const std::unique_ptr<MYSQL_STMT, StatementDeleter> stmtDeleteData = createStatement(ddlString);
        if (!stmtDeleteData)
            return false;
        if (!executeStatement(stmtDeleteData.get()))
            return false;

        return commitChanges();
    }

    std::unique_ptr<MYSQL_RES, SQLStorage::ResultSetDeleter>
    SQLStorage::wrapResultSet(MYSQL_RES* resultSetPtr)
    {
        return std::unique_ptr<MYSQL_RES, ResultSetDeleter> (resultSetPtr, ResultSetDeleter{});
    }

    [[nodiscard]]
    std::unique_ptr<MYSQL_STMT, SQLStorage::StatementDeleter>
    SQLStorage::createStatement() const
    {
        return std::unique_ptr<MYSQL_STMT, StatementDeleter> (
                mysql_stmt_init(sqlConnection.get()), StatementDeleter {}
        );
    }

    [[nodiscard]]
    std::unique_ptr<MYSQL_STMT,  SQLStorage::StatementDeleter>
    SQLStorage::createStatement(const std::string& stmtString)
    {
        std::unique_ptr<MYSQL_STMT, StatementDeleter> statement { nullptr };
        for (int32_t attempt = 0; attempt < retriesCountMax; ++attempt)
        {
            /** Can not create a statement: Only possible reason OUT_OF_MEMORY ... return immediately **/
            statement = createStatement();
            if (!statement)
                return nullptr;
            if (SQLError::SUCCESS != mysql_stmt_prepare(statement.get(), stmtString.c_str(), stmtString.size()))
            {
                /** Client have lost a connection AND we have successfully restored it **/
                if (!isConnected() && restoreConnection()) {
                    continue;
                }
                return nullptr;
            }
        }
        return statement;
    }

    std::unique_ptr<MYSQL_RES, SQLStorage::ResultSetDeleter>
    SQLStorage::executeQuery(const std::string& query) const
    {
        if (SQLError::SUCCESS != mysql_query(sqlConnection.get(), query.c_str())) {
            // TODO: Put error logs
            return nullptr;
        }
        return wrapResultSet(mysql_store_result(sqlConnection.get()));
    }

    bool SQLStorage::executeStatement(MYSQL_STMT* statement)
    {
        // TODO: Could crush with SIGSEGV without exception on some versions
        //       Need to investigate this issue and etc
        if (SQLError::SUCCESS != mysql_stmt_execute(statement)) {
            // TODO: Put error logs
            return false;
        }
        return true;
    }

    bool SQLStorage::executePreparedStatement(MYSQL_STMT* statement,
                                              std::vector<MYSQL_BIND>& params)
    {
        if (SQLError::SUCCESS != mysql_stmt_bind_param(statement, params.data())) {
            // TODO: Put error logs
            return false;
        }
        return executeStatement(statement);
    }

    [[nodiscard]]
    bool SQLStorage::createUsersTable()
    {
        const std::string stmtStr = "CREATE TABLE " + usersTableName + " ("
            "id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,"
            "name TEXT,"
            "description TEXT,"
            "timestamp BIGINT"
            ");";
        return createTable(usersTableName, stmtStr);
    }

    uint32_t SQLStorage::getInsertedUserId() const
    {
        const std::string stmtStr = "SELECT max(id) FROM " + usersTableName + ";";
        const auto resultSet = executeQuery(stmtStr );
        if (!resultSet) { // TODO: Put error logs
            return 0;
        }
        const MYSQL_ROW row = fetchRow(resultSet.get());
        if (row == nullptr) { // TODO:  Put error logs
            return 0;
        }
        return getAsInt(row, 0);
    }

    bool SQLStorage::insertUser(UserDTO& user)
    {
        const std::string stmtStr = std::string("INSERT INTO " + usersTableName +
            " (name,description,timestamp)  VALUES (?,?,?);");
        const std::unique_ptr<MYSQL_STMT, StatementDeleter> statement = createStatement(stmtStr);
        if (!statement) {
            std::cerr << "Failed to create statement | " << stmtStr << std::endl;
            return false;
        }

        int64_t timestamp = user.timestamp.time_since_epoch().count();
        std::vector<BindParam> params {
            { MYSQL_TYPE_STRING, user.name.data(), user.name.size() },
            { MYSQL_TYPE_STRING, user.description.data(), user.description.size() },
            { MYSQL_TYPE_LONGLONG, reinterpret_cast<char*>(&timestamp), sizeof(timestamp) },
        };

        std::vector<MYSQL_BIND> bindings = createSqlBinding(params);
        if (!executePreparedStatement(statement.get(), bindings)) {
            std::cerr << "Failed to execute statement | " << stmtStr
                << user.name.data() << ',' << user.description.data() << ',' << timestamp  << std::endl;
            return false;
        }

        if (!commitChanges()){
            return false;
        }

        user.id = getInsertedUserId();
        return 0 != user.id;
    }
}

namespace Tests
{
    constexpr std::string_view host { "0.0.0.0" };
    constexpr std::string_view username { "admin" };
    constexpr std::string_view password { "qwerty12345" };
    constexpr std::string_view database { "users" };

    void LegacyClientTests()
    {
        using namespace MySqlClientLegacy;

        std::unique_ptr<SQLStorage> storage = std::make_unique<SQLStorage>(
            host.data(), username.data(), password.data(), database.data()
        );


        UserDTO userDto { "admin", "Admin user"};
        const bool result = storage->insertUser(userDto);

        std::cout << std::boolalpha << result << std::endl;
    }
}


/**
>  GRANT ALL ON users.* TO 'admin'@'%';
**/
void MySQL::TestAll()
{
    Tests::LegacyClientTests();
}
