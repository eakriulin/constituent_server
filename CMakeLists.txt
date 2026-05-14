cmake_minimum_required(VERSION 4.1)
project(constituent)

set(CMAKE_CXX_STANDARD 23)

execute_process(COMMAND brew --prefix libpqxx OUTPUT_VARIABLE LIBPQXX_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
find_path(PQXX_INCLUDE_DIR pqxx/pqxx HINTS "${LIBPQXX_PREFIX}/include" REQUIRED)
find_library(PQXX_LIBRARY pqxx HINTS "${LIBPQXX_PREFIX}/lib" REQUIRED)

find_package(OpenSSL REQUIRED)

add_executable(server
        src/server.cpp
        dependencies/httplib.h
        dependencies/json.hpp
        src/app/db/connection_pool.cpp
        src/app/db/connection_pool.h
        src/app/types/constituents.h
        src/app/repos/constituents.cpp
        src/app/repos/constituents.h
        src/app/services/constituents.cpp
        src/app/services/constituents.h
        src/app/controllers/constituents.cpp
        src/app/controllers/constituents.h
        src/app/validators/constituents.cpp
        src/app/validators/constituents.h
        src/app/errors/http_error.cpp
        src/app/errors/http_error.h
        src/app/middlewares/cors_middleware.cpp
        src/app/middlewares/cors_middleware.h
        src/app/middlewares/errors_middleware.cpp
        src/app/middlewares/errors_middleware.h
        src/app/utils/log.cpp
        src/app/utils/log.h
        src/app/utils/request_context.cpp
        src/app/utils/request_context.h
        src/app/middlewares/pre_request_middleware.cpp
        src/app/middlewares/pre_request_middleware.h
        src/app/middlewares/rate_limit_middleware.cpp
        src/app/middlewares/rate_limit_middleware.h
        src/shared/env.cpp
        src/shared/env.h
        src/app/types/auth.h
        src/app/controllers/auth.cpp
        src/app/controllers/auth.h
        src/app/validators/auth.cpp
        src/app/validators/auth.h
        src/app/services/auth.cpp
        src/app/services/auth.h
        src/app/repos/auth.cpp
        src/app/repos/auth.h
        src/app/utils/crypto.cpp
        src/app/utils/crypto.h
        src/app/utils/datetime.cpp
        src/app/utils/datetime.h
        src/app/services/llm.cpp
        src/app/services/llm.h
        src/app/utils/strings.cpp
        src/app/utils/strings.h
        src/app/services/rate_limits.cpp
        src/app/services/rate_limits.h
)
target_include_directories(server PRIVATE ${PQXX_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/dependencies ${CMAKE_SOURCE_DIR}/dependencies/jwt-cpp)
target_link_libraries(server PRIVATE ${PQXX_LIBRARY} OpenSSL::SSL OpenSSL::Crypto "-framework CoreFoundation" "-framework Security")

add_executable(migrate
        src/migrate.cpp
        src/migrations/init_database.cpp
        src/migrations/init_database.h
        src/migrations/register_migrations.cpp
        src/migrations/register_migrations.h
        src/migrations/run_migrations.cpp
        src/migrations/run_migrations.h
        src/migrations/utils/Migration.cpp
        src/migrations/utils/Migration.h
        src/shared/env.cpp
        src/shared/env.h
        src/app/types/llm.h
        src/app/types/rate_limits.h
)
target_include_directories(migrate PRIVATE ${PQXX_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/dependencies)
target_link_libraries(migrate PRIVATE ${PQXX_LIBRARY})
