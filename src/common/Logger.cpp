#include "Logger.h"

#include <vector>

#include "Util.h"

void Logger::init(int& argc, char* argv[]) {
    /* Everything with a verbosity equal or greater than g_stderr_verbosity will be
        written to stderr. You can set this in code or via the -v argument.
        Set to loguru::Verbosity_OFF to write nothing to stderr.
        Default is 0, i.e. only log ERROR, WARNING and INFO are written to stderr.
        */
    loguru::g_stderr_verbosity  = 0;
    loguru::g_colorlogtostderr  = true; // True by default.
    loguru::g_flush_interval_ms = 0; // 0 (unbuffered) by default.
    loguru::g_preamble          = true; // Prefix each log line with date, time etc? True by default.

    /* Specify the verbosity used by loguru to log its info messages including the header
        logged when logged::init() is called or on exit. Default is 0 (INFO).
        */
    loguru::g_internal_verbosity = 3;

    loguru::g_preamble_date    = false;
    loguru::g_preamble_time    = true; // The time of the current day
    loguru::g_preamble_uptime  = false; // The time since init call
    loguru::g_preamble_thread  = false; // The logging thread
    loguru::g_preamble_file    = true; // The file from which the log originates from
    loguru::g_preamble_verbose = true; // The verbosity field
    loguru::g_preamble_pipe    = true; // The pipe symbol right before the message

    loguru::init(argc, argv);
}

Logger::Logger() {
}

std::ostream& operator<<(std::ostream& os, const QString& qstr) {
    return os << qstr.toStdString();
}
