#include "connection_pool.h"

ConnectionPool::ConnectionPool(const std::string& connection_string, const int8_t pool_size) {
    for (uint8_t i = 0; i < pool_size; i++) {
        this->stack.emplace_back(std::make_unique<pqxx::connection>(connection_string));
    }
}

std::unique_ptr<pqxx::connection> ConnectionPool::acquire() {
    std::unique_lock<std::mutex> lock(this->mu);

    if (!this->cv.wait_for(lock, std::chrono::seconds(this->CONNECTION_TIMEOUT), [this] {return !this->stack.empty();})) {
        throw std::runtime_error("connection pool timeout");
    }

    auto connection = std::move(this->stack.back());
    this->stack.pop_back();

    return connection;
}

void ConnectionPool::release(std::unique_ptr<pqxx::connection> connection) {
    if (connection == nullptr) {
        return;
    }

    std::unique_lock<std::mutex> lock(this->mu);

    this->stack.emplace_back(std::move(connection));

    this->cv.notify_one();
}

ConnectionGuard::ConnectionGuard(std::shared_ptr<ConnectionPool> db, std::unique_ptr<pqxx::connection> connection)
    : db(std::move(db)), connection(std::move(connection)) {}

ConnectionGuard::~ConnectionGuard() {
    this->db->release(std::move(this->connection));
}
