// Caution: Wt wants to be included before Qt
#include "AppFrontend.h"
#include "AppBackend.h"
#include "Logger.h"

#include <QCoreApplication>
#include <QThread>

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Start backend in dedicated thread
    std::atomic_bool doRun = true;
    auto qThread = QThread::create([&]() {
        AppBackend backend;
        while (doRun) {
            a.processEvents();
            std::this_thread::sleep_for(50ms);
        }
    });
    qThread->start();

    // TODO: Start backend with this, currently noop
    // Caution: always start backend before frontend. Wt requires this.
    AppBackend::run(doRun);

    // Start frontend as main loop
    AppFrontend::run(argc, argv);

    // When frontend stops, stop backend
    doRun = false;
    qThread->wait();
}
