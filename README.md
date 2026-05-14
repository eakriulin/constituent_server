# Constituent Server

C++23 HTTP server backing [constituent.it](https://constituent.it) — an Italian vocabulary companion. Users save Italian words and phrases ("constituents"), get their meaning and example sentences enriched by an LLM, and authenticate from a device using a passwordless Ed25519 challenge–response flow.

It is a production-shaped backend in modern C++ without leaning on a heavyweight web framework — only `cpp-httplib` for the HTTP transport, `libpqxx` for PostgreSQL, `nlohmann/json` for JSON, `jwt-cpp` for tokens, and OpenSSL for cryptography. Everything else (pooling, validation, rate limiting, logging, request context, migrations) is implemented from scratch so the architectural decisions are visible in the code rather than hidden behind a framework.

## Tech stack

- **Language:** C++23
- **Build:** CMake 4.1+
- **HTTP:** [`cpp-httplib`](https://github.com/yhirose/cpp-httplib) (header-only, thread-pooled)
- **Database:** PostgreSQL via [`libpqxx`](https://github.com/jtv/libpqxx)
- **JSON:** [`nlohmann/json`](https://github.com/nlohmann/json)
- **JWT:** [`jwt-cpp`](https://github.com/Thalhammer/jwt-cpp)
- **Crypto:** OpenSSL (Ed25519 signature verification, secure random)
- **LLM:** OpenAI API

## Project layout

```
src/
├── server.cpp                          # Composition root for the HTTP server
├── migrate.cpp                         # Composition root for the migration runner
├── shared/
│   └── env.{h,cpp}                     # .env loader + typed Env struct
├── migrations/
│   ├── init_database.{h,cpp}           # Bootstrap: create DB + _migrations table
│   ├── register_migrations.{h,cpp}     # Schema migrations registered in order
│   ├── run_migrations.{h,cpp}          # Idempotent runner (skips applied ones)
│   └── utils/Migration.{h,cpp}         # Migration value type + registry
└── app/
    ├── db/connection_pool.{h,cpp}      # Thread-safe pqxx connection pool + RAII guard
    ├── errors/http_error.{h,cpp}       # HttpError exception type (status + message)
    ├── middlewares/
    │   ├── cors_middleware.{h,cpp}     # CORS preflight + response headers
    │   ├── pre_request_middleware.{h,cpp}  # Request context init + JWT auth
    │   ├── errors_middleware.{h,cpp}   # Global exception → JSON response handler
    │   └── rate_limit_middleware.{h,cpp}   # Per-route token-bucket rate limiter
    ├── controllers/                    # Route registration (auth, constituents)
    ├── services/                       # Business logic (auth, constituents, llm, rate_limits)
    ├── repos/                          # SQL queries
    ├── validators/                     # Request → DTO parsing + field validation
    ├── types/                          # DTOs + domain structs + JSON (de)serializers
    └── utils/
        ├── log.{h,cpp}                 # Thread-safe async logger
        ├── request_context.{h,cpp}     # Per-request thread-local context
        ├── crypto.{h,cpp}              # Ed25519 verify wrapper around OpenSSL
        ├── datetime.{h,cpp}            # Timestamp helpers
        └── strings.{h,cpp}             # Trim / split helpers
dependencies/                           # Vendored header-only libs (httplib, json, jwt-cpp)
```

Build emits two executables — `server` (HTTP server) and `migrate` (migration runner).

## Request lifecycle

```
HTTP request
  → cors_middleware           (preflight + CORS headers)
  → pre_request_middleware    (init thread-local context, verify JWT, set user_id)
  → rate_limit_middleware     (per-route token-bucket check)
  → controller                (route handler)
      → validator             (parse + validate body/params → DTO)
      → service               (business logic, may call LLM)
      → repo                  (SQL via pooled connection)
  → JSON response
errors_middleware catches any exception thrown along the way.
```

The layering — **controller → validator → service → repo** — keeps SQL out of business logic and HTTP concerns out of services. Dependencies are passed in via constructors (`std::shared_ptr`), wired up once in `server.cpp`.

## Highlighted components

### 1. PostgreSQL connection pool — `app/db/connection_pool.{h,cpp}`

A fixed-size pool of `pqxx::connection` objects, guarded by a `std::mutex` and a `std::condition_variable`. Workers acquire a connection (blocking up to 30 s, then throwing on timeout) and release it via the `ConnectionGuard` RAII wrapper, which puts the connection back on the stack in its destructor and notifies one waiter. The pool is non-copyable and non-movable to prevent accidental duplication of the underlying connection set.

```cpp
const auto db = std::make_shared<ConnectionPool>(connection_string, env.pool_size);
// ...
const ConnectionGuard guard{db, db->acquire()};
pqxx::nontransaction ntx{*guard.connection};
```

### 2. Repository pattern — `app/repos/*`

Each aggregate (`constituents`, `auth`) has a repo class whose only job is to translate DTOs into SQL and rows back into domain structs. Repos own a `shared_ptr<ConnectionPool>` and acquire a connection per call. Dynamic queries (e.g., `EditConstituentDto`, where any subset of fields may be present) are built with `pqxx::params` and `std::format` so values stay parameterised — no string concatenation of user input into SQL. See `repos/constituents.cpp::edit` for a representative example.

### 3. Token-bucket rate limiting — `app/services/rate_limits.{h,cpp}` + `app/middlewares/rate_limit_middleware.{h,cpp}`

Rate limits are stored in an **unlogged** PostgreSQL table `rate_limits(key, tokens, refilled_at)` and updated in a single `INSERT … ON CONFLICT DO UPDATE` statement that:

1. Computes how many tokens to refill based on elapsed seconds since `refilled_at` and the configured rate.
2. Caps tokens at the configured limit (`LEAST(limit, tokens + refilled)`).
3. Subtracts 1 for the current request.
4. Updates `refilled_at` only if at least one whole token was refilled.

A `CHECK (tokens >= 0)` constraint causes the update to throw `pqxx::check_violation` when the bucket is empty; the service catches that exception and returns `false`. The middleware wraps a route handler — keying the bucket by `METHOD:path:user_id` — and short-circuits with `429 Too Many Requests` when the bucket is exhausted.

This puts the entire algorithm in one atomic SQL statement, so concurrent requests for the same key cannot race past each other.

### 4. Input validation layer — `app/validators/*`

Each controller delegates to a validator that turns the raw `httplib::Request` into a typed DTO and throws `HttpError(400, ...)` on malformed input. Validators check field presence, size bounds (e.g., `MAX_VALUE_SIZE`, `MAX_EXAMPLE_CONTENT_SIZE`), and shape constraints (e.g., exactly `N_EXAMPLES` examples). Services and repos therefore never see un-validated data.

### 5. App-wide error handling — `app/errors/http_error.{h,cpp}` + `app/middlewares/errors_middleware.{h,cpp}`

`HttpError` is a `std::runtime_error` carrying a status code and a message. Any layer can `throw HttpError(404, "Constituent not found")`; `errors_middleware` installs a `cpp-httplib` exception handler that:

- Catches `HttpError` → returns its status with a `{"error_message": ...}` body and logs at `WARN`.
- Catches any other `std::exception` → returns `500` with a generic body and logs at `ERROR` (the underlying message stays in the log, never reaches the client).

This means business logic never has to assemble HTTP responses for failure cases — it just throws.

### 6. Thread-safe async logging — `app/utils/log.{h,cpp}`

A single background thread drains a `std::queue<std::string>` guarded by a mutex and a condition variable. Producers (`log::info / warn / error`) format a line with timestamp, level, message, and the current request context, push it onto the queue, and notify the worker. The worker swaps the queue under the lock, then writes outside the lock so producers are never blocked on stdout. `log::shutdown()` is called from `main` after `app.listen` returns so no log lines are lost on `SIGINT` / `SIGTERM`.

### 7. Per-request context — `app/utils/request_context.{h,cpp}`

Each worker thread holds a `thread_local Context` containing: a 32-hex-char request id (from a `thread_local mt19937_64`), client IP, method, path, params, body, and `user_id`. `set_defaults(req)` is called by `pre_request_middleware` at the start of every request; `set_user_id(...)` is set after JWT verification. The logger automatically appends `request_context::to_string()` to every line, so any log entry — even from deep inside a repo — is correlatable to the originating request without threading a context object through every function signature.

### 8. LLM service / OpenAI API client — `app/services/llm.{h,cpp}`

`LLMService::enrich` posts to `https://api.openai.com/v1/responses` using `cpp-httplib`'s TLS client (`CPPHTTPLIB_OPENSSL_SUPPORT`), referencing a server-side prompt by id. Two pieces worth pointing at:

- **`with_retry`** — retries up to 3 times on transient network errors (`Connection`, `ConnectionTimeout`, `Read`, `Write`, `ResourceExhaustion`, …) and on 5xx responses; logs each attempt; throws `HttpError(500, …)` if all attempts fail.
- **`to_constituent`** — parses the LLM's structured response into the `Constituent` domain type, validating that the `message` block contains the expected six lines (value, meaning, two example/translation pairs). Malformed responses are logged with the raw body and surfaced as a 500 — never trust an external API to keep its contract.

Request and response bodies are logged at `INFO` for traceability.

### 9. Schema migrations — `src/migrations/*` + `migrate` executable

A second executable, `migrate`, exists solely to evolve the database schema:

1. **`init_database`** — connects to the `postgres` system database, creates the application database if missing, then creates the `_migrations(name, finished_at)` ledger table if missing.
2. **`register_migrations`** — each migration is a `Migration{name, std::function<void(pqxx::work&)>}` appended to a static registry, then sorted by name. Names follow `NNN_description` (e.g., `001_create_users_table`) so lexicographic order is execution order.
3. **`run_migrations`** — iterates the registry; for each migration, checks the ledger inside a transaction; if unapplied, runs the migration lambda and inserts the ledger row in the same transaction; commits. Already-applied migrations are skipped.

The result is reproducible schema evolution: `./migrate` is safe to run on any environment at any time, and adding a new migration is a single registry entry.

## Authentication (Ed25519 challenge–response)

Devices register a public key. To log in, the client requests a challenge (a 32-byte random value, base64-encoded), signs it locally with its private key, and posts the signature back. The server verifies the signature with OpenSSL's `EVP_DigestVerify` Ed25519 implementation (`app/utils/crypto.{h,cpp}`) and, on success, issues a 7-day HS256 JWT carrying the user id as `sub`. Challenges expire after 60 s and are cached for ~50 s so a repeated challenge request within the window returns the same value.

The middleware decodes and verifies the `Authorization: Bearer …` token on every non-public route and stores `user_id` in the thread-local request context for use by repos.

## Build & run

Prerequisites (macOS):

```sh
brew install cmake libpqxx openssl postgresql
```

Configure environment in a `.env` at the repository root (loaded by `shared/env.cpp`):

```
DB_USERNAME=...
DB_PASSWORD=...
DB_HOST=localhost
DB_PORT=5432
DB_NAME=constituent
APP_HOST=0.0.0.0
APP_PORT=8080
POOL_SIZE=8
QUEUE_SIZE=64
JWT_SECRET=...
ALLOWED_ORIGIN=https://constituent.it
LLM_API_KEY=...
LLM_PROMPT_ID=...
```

Build both executables:

```sh
cmake -S . -B build
cmake --build build
```

Apply migrations, then start the server:

```sh
./build/migrate
./build/server
```

## HTTP API (summary)

| Method | Path                          | Auth | Purpose                                  |
|--------|-------------------------------|------|------------------------------------------|
| POST   | `/auth/devices/register`      | —    | Register a device with its public key    |
| POST   | `/auth/devices/challenge`     | —    | Get a challenge to sign                  |
| POST   | `/auth/devices/response`      | —    | Submit signed challenge, receive JWT     |
| DELETE | `/auth/me`                    | JWT  | Wipe all user data                       |
| GET    | `/constituents`               | JWT  | List user's constituents (keyset paged)  |
| POST   | `/constituents`               | JWT  | Add a constituent                        |
| PATCH  | `/constituents/:public_id`    | JWT  | Edit a constituent (partial update)      |
| DELETE | `/constituents/:public_id`    | JWT  | Remove a constituent                     |
| POST   | `/constituents/enrich`        | JWT  | LLM-generate meaning + examples for a word (rate-limited: 60 / hour) |
