#pragma once

#include <rpp/schedulers/details/worker.hpp>

#include <uvw/loop.h>
#include <uvw/timer.h>

namespace rpp_uvw::schedulers {

class main_thread_scheduler final : public rpp::schedulers::details::scheduler_tag {
private:
    class worker_strategy;
    using main_thread_schedulable = rpp::schedulers::schedulable_wrapper<worker_strategy>;

    class worker_strategy {
    public:
        worker_strategy(const rpp::subscription_base& sub)
            : m_sub{sub} {}

        bool is_subscribed() const { return m_sub.is_subscribed(); }

        void defer_at(rpp::schedulers::time_point time_point, rpp::schedulers::constraint::schedulable_fn auto&& fn) const {
            defer_at(time_point, main_thread_schedulable{*this, time_point, std::forward<decltype(fn)>(fn)});
        }

        void defer_at(rpp::schedulers::time_point time_point, main_thread_schedulable&& fn) const {
            if (!m_sub.is_subscribed())
                return;

            auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
            timer->on<uvw::timer_event>([fn = std::move(fn)](const auto&, auto &handle) {
                    const_cast<main_thread_schedulable&&>(fn)();
                    handle.stop();
                    handle.close();
                });
            const auto duration = std::max(std::chrono::milliseconds{0}, std::chrono::duration_cast<std::chrono::milliseconds>(time_point - now()));
            timer->start(duration, uvw::timer_handle::time{0});
        }

        static rpp::schedulers::time_point now() { return rpp::schedulers::clock_type::now(); }
    private:
        rpp::subscription_base m_sub;
    };

public:
    static auto create_worker(const rpp::subscription_base& sub = {}) {
        return rpp::schedulers::worker<worker_strategy>{sub};
    }
};

} // namespace rpp_uvw::schedulers
