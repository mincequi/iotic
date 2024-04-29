#pragma once

#include <rpp/schedulers/details/worker.hpp>

#include <uvw/loop.h>
#include <uvw/timer.h>

namespace rpp_uvw::schedulers {

class main_thread_scheduler final {
private:
    class worker_strategy {
    public:
        template<rpp::schedulers::constraint::schedulable_handler Handler, typename... Args, rpp::schedulers::constraint::schedulable_fn<Handler, Args...> Fn>
        static void defer_for(rpp::schedulers::duration duration, Fn&& fn, Handler&& handler, Args&&... args) {
            auto timer = uvw::loop::get_default()->resource<uvw::timer_handle>();
            timer->on<uvw::timer_event>([fn = std::forward<Fn>(fn), handler = std::forward<Handler>(handler), ... args = std::forward<Args>(args)] (const auto&, auto &handle) {
                if (!handler.is_disposed())
                    invoke(std::move(fn), std::move(handler), std::move(args)...);
                handle.stop();
                handle.close();
            });
            timer->start(std::chrono::duration_cast<std::chrono::milliseconds>(duration), uvw::timer_handle::time{0});
        }

        static constexpr rpp::schedulers::details::none_disposable get_disposable() { return {}; }

        static rpp::schedulers::time_point now() { return rpp::schedulers::clock_type::now(); }

    private:
        template<rpp::schedulers::constraint::schedulable_handler Handler, typename... Args, rpp::schedulers::constraint::schedulable_delay_from_now_fn<Handler, Args...> Fn>
        static void invoke(Fn&& fn, Handler&& handler, Args&&... args) {
            if (const auto new_duration = fn(handler, args...))
                defer_for(new_duration->value, std::forward<Fn>(fn), std::forward<Handler>(handler), std::forward<Args>(args)...);
        }

        template<rpp::schedulers::constraint::schedulable_handler Handler, typename... Args, rpp::schedulers::constraint::schedulable_delay_from_this_timepoint_fn<Handler, Args...> Fn>
        static void invoke(Fn&& fn, Handler&& handler, Args&&... args) {
            const auto now = rpp::schedulers::clock_type::now();
            if (const auto new_duration = fn(handler, args...))
                defer_for(now + new_duration->value - rpp::schedulers::clock_type::now(), std::forward<Fn>(fn), std::forward<Handler>(handler), std::forward<Args>(args)...);
        }

        template<rpp::schedulers::constraint::schedulable_handler Handler, typename... Args, rpp::schedulers::constraint::schedulable_delay_to_fn<Handler, Args...> Fn>
        static void invoke(Fn&& fn, Handler&& handler, Args&&... args) {
            if (const auto new_tp = fn(handler, args...))
                defer_for(new_tp->value - rpp::schedulers::clock_type::now(), std::forward<Fn>(fn), std::forward<Handler>(handler), std::forward<Args>(args)...);
        }
    };

public:
    static auto create_worker() {
        return rpp::schedulers::worker<worker_strategy>{};
    }
};

} // namespace rpp_uvw::schedulers
