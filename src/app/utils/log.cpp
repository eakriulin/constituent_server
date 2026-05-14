#include <iostream>
#include <chrono>
#include <format>
#include "log.h"
#include "request_context.h"

std::atomic<bool> log::should_stop{false};
std::queue<std::string> log::queue;
std::mutex log::mu;
std::condition_variable log::cv;
std::thread log::worker{&log::process_queue};

void log::process_queue() {
    while (true) {
        std::unique_lock lock(mu);
        cv.wait(lock, [] { return should_stop.load() || !queue.empty(); });

        std::queue<std::string> local_queue;
        std::swap(local_queue, queue);
        const bool stop = should_stop.load();
        lock.unlock();

        while (!local_queue.empty()) {
            std::cout << local_queue.front() << "\n";
            local_queue.pop();
        }

        std::cout.flush();

        if (stop) {
            return;
        };
    }
}

void log::info(std::string&& msg) {
    enqueue("INFO", std::move(msg));
}

void log::warn(std::string&& msg) {
    enqueue("WARN", std::move(msg));
}

void log::error(std::string&& msg) {
    enqueue("ERROR", std::move(msg));
}

void log::enqueue(std::string&& level, std::string&& msg) {
    {
        auto now = std::chrono::system_clock::now();
        const std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%S}", now);

        std::lock_guard lock(mu);
        queue.emplace(std::format("[{}] [{}] {} [CONTEXT] {}", timestamp, level, msg, request_context::to_string()));
    }

    cv.notify_one();
}

void log::shutdown() {
    if (should_stop) {
        return;
    }

    should_stop = true;
    cv.notify_one();
    worker.join();
}