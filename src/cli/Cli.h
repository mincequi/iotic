#pragma once

#include <memory>

#include "../AppMode.h"

class Cli {
public:
    static mode parseCommandLine(int argc, char* argv[]);

private:
    Cli();
};
