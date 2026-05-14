#include "auth.h"

#include "../errors/http_error.h"
#include "../utils/log.h"
#include "../utils/request_context.h"

AuthRepo::AuthRepo(std::shared_ptr<ConnectionPool> db)
    : db(std::move(db)) {}

RegisterDeviceResult AuthRepo::register_with_device(DeviceRegisterDto &&dto) const {
    const auto connection_guard = ConnectionGuard(this->db, this->db->acquire());

    pqxx::work tx{*connection_guard.connection};

    const auto existing_device = tx.query01<int>(R"(
        SELECT id
        FROM devices
        WHERE public_key = $1
    )", pqxx::params(dto.public_key));

    if (existing_device.has_value()) {
        log::warn("Duplicate device public key");
        throw HttpError(400, "Request body is invalid");
    }

    auto [user_id] = tx.query1<int>(R"(
        INSERT INTO users
        DEFAULT VALUES
        RETURNING id
    )");

    auto [public_id] = tx.query1<std::string>(R"(
        INSERT INTO devices (user_id, public_key)
        VALUES ($1, $2)
        RETURNING public_id
    )", pqxx::params(user_id, dto.public_key));

    tx.commit();

    return RegisterDeviceResult{public_id};
}

Device AuthRepo::get_device(const std::string& device_id) const {
    const auto connection_guard = ConnectionGuard(this->db, this->db->acquire());
    pqxx::nontransaction ntx{*connection_guard.connection};

    const auto row = ntx.query01<
        std::string,
        int,
        std::string,
        std::optional<std::string>,
        std::optional<std::string>
    >(R"(
        SELECT public_id, user_id, public_key, challenge, challenged_at
        FROM devices
        WHERE public_id = $1
    )", pqxx::params(device_id));

    if (!row.has_value()) {
        throw HttpError(404, "Device not found");
    }

    auto [public_id, user_id, public_key, challenge, challenged_at] = row.value();

    return Device{
        public_id,
        user_id,
        public_key,
        challenge,
        challenged_at
    };
}

void AuthRepo::store_device_challenge(const std::string& device_id, const std::string& challenge) const {
    const auto connection_guard = ConnectionGuard(this->db, this->db->acquire());
    pqxx::nontransaction ntx{*connection_guard.connection};

    ntx.exec(R"(
        UPDATE devices
        SET challenge = $1, challenged_at = NOW()
        WHERE public_id = $2
    )", pqxx::params(challenge, device_id));
}

void AuthRepo::clear_device_challenge(const std::string& device_id) const {
    const auto connection_guard = ConnectionGuard(this->db, this->db->acquire());
    pqxx::nontransaction ntx{*connection_guard.connection};

    ntx.exec(R"(
        UPDATE devices
        SET challenge = NULL, challenged_at = NULL
        WHERE public_id = $1
    )", pqxx::params(device_id));
}

void AuthRepo::remove_all_for_user() const {
    const auto connection_guard = ConnectionGuard(this->db, this->db->acquire());
    pqxx::work tx{*connection_guard.connection};

    tx.exec(R"(
        DELETE FROM devices
        WHERE user_id = $1
    )", pqxx::params(request_context::get_user_id()));

    tx.exec(R"(
        DELETE FROM users
        WHERE id = $1
    )", pqxx::params(request_context::get_user_id()));

    tx.commit();
}
