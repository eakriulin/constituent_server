#include "json.hpp"
#include "constituents.h"
#include "../errors/http_error.h"
#include "../utils/request_context.h"

ConstituentsRepo::ConstituentsRepo(std::shared_ptr<ConnectionPool> db)
    : db(std::move(db)) {}

std::vector<Constituent> ConstituentsRepo::list(ListConstituentDto &&dto) const {
    const auto connection_guard = ConnectionGuard{this->db, this->db->acquire()};
    pqxx::nontransaction ntx{*connection_guard.connection};

    std::vector<Constituent> constituents;
    int after_id = std::numeric_limits<int>::max();

    if (dto.after.has_value()) {
        auto row_id = ntx.query01<int>(R"(
                SELECT id
                FROM constituents
                WHERE user_id = $1 AND public_id = $2
                LIMIT 1
            )", pqxx::params(request_context::get_user_id(), dto.after));

        if (!row_id.has_value()) {
            throw HttpError(400, "Invalid cursor");
        }

        auto [id] = row_id.value();
        after_id = id;
    }

    const auto rows = ntx.query<
        std::string,
        std::string,
        std::string,
        std::string
    >(R"(
        SELECT public_id, value, meaning, examples
        FROM constituents
        WHERE user_id = $1 AND id < $2
        ORDER BY id DESC
        LIMIT 100
    )", pqxx::params(request_context::get_user_id(), after_id));

    for (auto& [public_id, value, meaning, examples] : rows) {
        constituents.emplace_back(
            std::nullopt,
            public_id,
            std::nullopt,
            value,
            meaning,
            nlohmann::json::parse(examples).get<std::vector<ConstituentExample>>()
        );
    }

    return std::move(constituents);
}

Constituent ConstituentsRepo::add(AddConstituentDto &&dto) const {
    const auto connection_guard = ConnectionGuard(this->db, this->db->acquire());
    pqxx::nontransaction ntx{*connection_guard.connection};

    auto [public_id, value, meaning, examples] = ntx.query1<
        std::string,
        std::string,
        std::string,
        std::string
    >(R"(
        INSERT INTO constituents (user_id, value, meaning, examples) VALUES
        ($1, $2, $3, $4)
        RETURNING public_id, value, meaning, examples
    )", pqxx::params(
        request_context::get_user_id(),
        dto.value,
        dto.meaning,
        nlohmann::json(dto.examples).dump()
    ));

    return Constituent{
        std::nullopt,
        public_id,
        std::nullopt,
        value,
        meaning,
        nlohmann::json::parse(examples).get<std::vector<ConstituentExample>>()
    };
}

Constituent ConstituentsRepo::edit(EditConstituentDto &&dto) const {
    pqxx::params params;
    std::string set_query;

    if (dto.value.has_value()) {
        params.append(dto.value.value());
        set_query += std::format("value = ${}", params.size());
    }

    if (dto.meaning.has_value()) {
        if (!set_query.empty()) set_query += ", ";
        params.append(dto.meaning.value());
        set_query += std::format("meaning = ${}", params.size());
    }

    if (dto.examples.has_value()) {
        if (!set_query.empty()) set_query += ", ";
        params.append(nlohmann::json(dto.examples.value()).dump());
        set_query += std::format("examples = ${}", params.size());
    }

    if (set_query.empty()) {
        throw HttpError(400, "No fields to update");
    }

    params.append(request_context::get_user_id());
    auto where_query = std::format("user_id = ${}", params.size());

    params.append(dto.public_id);
    where_query += std::format(" AND public_id = ${}", params.size());

    const auto connection_guard = ConnectionGuard{this->db, this->db->acquire()};
    pqxx::nontransaction ntx{*connection_guard.connection};

    auto row = ntx.query01<std::string, std::string, std::string, std::string>(std::format(R"(
        UPDATE constituents
        SET {}
        WHERE {}
        RETURNING public_id, value, meaning, examples
    )", set_query, where_query), params);

    if (!row.has_value()) {
        throw HttpError(404, "Constituent not found");
    }

    auto [public_id, value, meaning, examples] = row.value();

    return Constituent{
        std::nullopt,
        public_id,
        std::nullopt,
        value,
        meaning,
        nlohmann::json::parse(examples).get<std::vector<ConstituentExample>>(),
    };
}

void ConstituentsRepo::remove(RemoveConstituentDto &&dto) const {
    const auto connection_guard = ConnectionGuard{this->db, this->db->acquire()};
    pqxx::nontransaction ntx{*connection_guard.connection};

    const auto result = ntx.exec(R"(
        DELETE FROM constituents
        WHERE user_id = $1 AND public_id = $2
    )", pqxx::params(request_context::get_user_id(), dto.public_id));

    if (result.affected_rows() == 0) {
        throw HttpError(404, "Constituent not found");
    }
}

void ConstituentsRepo::remove_all_for_user() const {
    const auto connection_guard = ConnectionGuard{this->db, this->db->acquire()};
    pqxx::work tx{*connection_guard.connection};

    tx.exec(R"(
        DELETE FROM constituents
        WHERE user_id = $1
    )", pqxx::params(request_context::get_user_id()));

    tx.commit();
}
