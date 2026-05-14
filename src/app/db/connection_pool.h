#ifndef CONSTITUENT_CONNECTION_POOL_H
#define CONSTITUENT_CONNECTION_POOL_H

#include <vector>
#include <mutex>
#include <condition_variable>
#include <pqxx/pqxx>
#include <memory>

class ConnectionPool {
private:
    const int8_t CONNECTION_TIMEOUT = 30;

    std::vector<std::unique_ptr<pqxx::connection>> stack;
    std::mutex mu;
    std::condition_variable cv;
public:
    explicit ConnectionPool(const std::string& connection_string, int8_t pool_size);

    ~ConnectionPool() = default; // Use default destructor

    ConnectionPool(const ConnectionPool&) = delete; // Don't allow copy constructor
    ConnectionPool& operator=(const ConnectionPool&) = delete; // Don't allow copy assignment

    ConnectionPool(ConnectionPool&&) = delete; // Don't allow move constructor
    ConnectionPool& operator=(ConnectionPool&&) = delete; // Don't allow move assignment

    [[nodiscard]] std::unique_ptr<pqxx::connection> acquire();
    void release(std::unique_ptr<pqxx::connection>);
};

class ConnectionGuard {
private:
    std::shared_ptr<ConnectionPool> db;
public:
    std::unique_ptr<pqxx::connection> connection;

    explicit  ConnectionGuard(std::shared_ptr<ConnectionPool>, std::unique_ptr<pqxx::connection> connection);
    ~ConnectionGuard();
};

#endif //CONSTITUENT_CONNECTION_POOL_H