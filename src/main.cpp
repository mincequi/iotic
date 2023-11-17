#include <QCoreApplication>

#include "AppBackend.h"

#include <common/Logger.h>

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // Setup main loop
    AppBackend backend;

    return a.exec();
}
