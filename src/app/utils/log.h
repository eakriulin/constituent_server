#ifndef CONSTITUENT_LOG_H
#define CONSTITUENT_LOG_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class log {
public:
    static void info(std::string&& msg);
    static void warn(std::string&& msg);
    static void error(std::string&& msg);

    static void shutdown();

private:
    static std::atomic<bool> should_stop;
    static std::queue<std::string> queue;
    static std::mutex mu;
    static std::condition_variable cv;
    static std::thread worker;

    static void process_queue();
    static void enqueue(std::string&& level, std::string&& msg);
};


#endif //CONSTITUENT_LOG_H