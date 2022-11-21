#include "AppFrontend.h"

#include "WQApplication.h"

class PrivateAppFrontend : public Wt::WQApplication {
public:
    PrivateAppFrontend(const Wt::WEnvironment& env) :
        Wt::WQApplication(env, true) {
    }

private:
    void create() override {
    }

    void destroy() override {
    }
};

AppFrontend::AppFrontend() {
}

int AppFrontend::run(int argc, char* argv[]) {
    std::string applicationPath = argv[0];
    std::vector<std::string> args(argv + 1, argv + argc);
    args.insert(args.end(), { "--docroot", "." });
    args.insert(args.end(), { "--http-address", "0.0.0.0" });
    args.insert(args.end(), { "--http-port", "1234" });

    return Wt::WRun(applicationPath, args, [](const Wt::WEnvironment& env) {
        return std::make_unique<PrivateAppFrontend>(env);
    });
}
