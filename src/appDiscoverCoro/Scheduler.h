#pragma once

#include <coroutine>
#include <chrono>
#include <vector>
#include <functional>
#include <atomic>
#include <cstdint>

class Scheduler {
public:
    using clock = std::chrono::system_clock;
    using seconds = std::chrono::seconds;

    Scheduler() = default;

    struct Task {
        struct promise_type {
            Task get_return_object() {
                return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
            }
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() {}
            void unhandled_exception() {}
        };

        std::coroutine_handle<promise_type> coro;
        explicit Task(std::coroutine_handle<promise_type> h) : coro(h) {}
        void start() { coro.resume(); }
    };

    struct Awaitable {
        clock::time_point wake_time;
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> h);
        void await_resume() const noexcept {}
    };

    static Awaitable sleepFor(seconds s);

    void add_periodic_task(std::function<Task()> func, uint32_t period_seconds);

    void run();
    void stop();

private:
    struct periodic_entry {
        std::function<Task()> func;
        uint32_t period_seconds;
        std::atomic<bool> running{false};

        periodic_entry(std::function<Task()> f, uint32_t p)
            : func(std::move(f)), period_seconds(p), running(false) {}

        // Explicit move constructor
        periodic_entry(periodic_entry&& other) noexcept
            : func(std::move(other.func)),
            period_seconds(other.period_seconds),
            running(other.running.load()) {}
    };

    static thread_local Scheduler* _self;
    std::vector<periodic_entry> _tasks;
    std::atomic<bool> _running{true};
};
