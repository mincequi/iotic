#include <QCoreApplication>

#include "AppBackend.h"

#include <common/Logger.h>

#include "LibUvEventDispatcher.h"

int main(int argc, char *argv[]) {

    iz::Eventing::LibUvEventDispatcher dispatcher;
    QCoreApplication::setEventDispatcher(&dispatcher);

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Setup main loop
    AppBackend backend(dispatcher.rawLoop());

    return a.exec();
}
