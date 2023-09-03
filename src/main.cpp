#include "AppBackend.h"

#include <common/Logger.h>
//#include <feed/FeedManager.h>

#include <QCoreApplication>

int main(int argc, char *argv[]) {

    // Create application instance
    QCoreApplication a(argc, argv);

    // Setup logger
    Logger::init(argc, argv);

    // FeedManager - the bridge for data between backend and frontend
    //FeedManager feedManager;

    AppBackend backend;//(feedManager);

    return a.exec();
}
