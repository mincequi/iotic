#include "AppBackend.h"

#include <common/Logger.h>

#include <QCoreApplication>

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    AppBackend backend;

    return a.exec();
}
