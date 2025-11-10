#include "Scheduler.h"

#include <thread>
#include <iostream>

thread_local Scheduler* Scheduler::_self = nullptr;

void Scheduler::Awaitable::await_suspend(std::coroutine_handle<> h) {
    // Awaitable is only used for inner sleeps, not scheduling
    std::thread([h, until = wake_time]() {
        std::this_thread::sleep_until(until);
        h.resume();
    }).detach();
}

Scheduler::Awaitable Scheduler::sleepFor(seconds s) {
    return Awaitable{clock::now() + s};
}

void Scheduler::add_periodic_task(std::function<Task()> func, uint32_t period_seconds) {
    _tasks.push_back({std::move(func), period_seconds});
}

void Scheduler::run() {
    _self = this;

    while (_running) {
        auto epoch = std::chrono::duration_cast<seconds>(clock::now().time_since_epoch()).count();

        for (auto& entry : _tasks) {
            if (entry.period_seconds == 0)
                continue;

            if (epoch % entry.period_seconds == 0 && !entry.running.load()) {
                entry.running = true;

                // Wrap the task coroutine to clear running flag when done
                auto wrapped = [&entry, &f = entry.func]() -> Task {
                    co_await Scheduler::sleepFor(seconds(0)); // ensure coroutine context
                    auto t = f();
                    t.start();
                    entry.running = false;
                    co_return;
                };

                Task wrapper = wrapped();
                wrapper.start();
            }
        }

        // sleep until next second boundary
        std::this_thread::sleep_until(clock::time_point{seconds{epoch + 1}});
    }
}

void Scheduler::stop() {
    _running = false;
}
