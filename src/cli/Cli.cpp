#include "Cli.h"

#include <iostream>

#include <clipp.h>

#include "../AppMode.h"

using namespace std;
using namespace clipp;

auto _selectedMode = mode::help;

Cli::Cli() {
}

mode Cli::parseCommandLine(int argc, char* argv[]) {
    if (argc < 2) return mode::daemon;

    auto daemonMode = command("daemon").set(_selectedMode, mode::daemon);
    auto discoverMode = command("discover").set(_selectedMode, mode::discover);
    auto helpMode = ( command("help").set(_selectedMode, mode::help) );

    auto cli = (daemonMode | discoverMode | helpMode);

    if (parse(argc, argv, cli)) {
        switch (_selectedMode) {
            case mode::help:
            cout << make_man_page(cli, "iotic");
        default:
            return _selectedMode;
        }
    } else {
        cout << usage_lines(cli, "iotic") << '\n';
    }

    return mode::help;
}
