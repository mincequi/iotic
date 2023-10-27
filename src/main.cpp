#include "AppBackend.h"

#include <common/Logger.h>

#include <QCoreApplication>
#include <uvw.hpp>

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Setup main loop
    auto loop = uvw::loop::get_default();

    AppBackend backend(loop->raw());

    auto timer = loop->resource<uvw::timer_handle>();
    timer->on<uvw::timer_event>([&a](const uvw::timer_event& ev, const uvw::timer_handle& handle) {
        a.processEvents();
    });
    timer->on<uvw::error_event>([](const uvw::error_event& ev, const uvw::timer_handle& handle) {
        LOG_S(ERROR) << "error: " << ev.name();
    });
    //auto idle = loop->resource<uvw::idle_handle>();
    //idle->on<uvw::idle_event>([&a](const uvw::idle_event& ev, const uvw::idle_handle& handle) {
    //    a.processEvents();
    //});
    //idle->on<uvw::error_event>([](const uvw::error_event& ev, const uvw::idle_handle& handle) {
    //    LOG_S(ERROR) << "error: " << ev.name();
    //});
    timer->init();
    timer->start(std::chrono::milliseconds(0), std::chrono::milliseconds(50));
    loop->run();
}
