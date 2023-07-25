#include "AppFrontend.h"

#include <Wt/WBootstrap5Theme.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WServer.h>
#include <Wt/WText.h>

#include <common/Logger.h>
#include "feed/FeedManager.h"
#include "ui/HistoryModel.h"
#include "ui/MainWidget.h"

using namespace Wt;

class PrivateAppFrontend : public Wt::WApplication {
public:
    PrivateAppFrontend(const Wt::WEnvironment& env,
                       const FeedManager& feedManager) :
        Wt::WApplication(env) {

        enableUpdates();

        setTheme(std::make_shared<WBootstrap5Theme>());
        setTitle("HarvvestingHouse");

        root()->addWidget(std::make_unique<MainWidget>(feedManager));
    }

    ~PrivateAppFrontend() {
    }
};

AppFrontend::AppFrontend() {
}

int AppFrontend::runBlocking(int argc, char* argv[], const FeedManager& feedManager) {
    std::string applicationPath = argv[0];
    std::vector<std::string> args(argv + 1, argv + argc);
    args.insert(args.end(), { "--docroot", "." });
    args.insert(args.end(), { "--http-address", "0.0.0.0" });
    args.insert(args.end(), { "--http-port", "1234" });

    HistoryModel historyModel;

    return Wt::WRun(applicationPath, args, [&](const Wt::WEnvironment& env) {
        return std::make_unique<PrivateAppFrontend>(env, feedManager);
    });
}
