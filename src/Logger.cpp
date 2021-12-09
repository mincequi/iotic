#include "Logger.h"

#include <vector>

#include <magic_enum.hpp>

#include "sunspec/SunSpecMeasuredValue.h"

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

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        os << *it;
        if (it != vec.end()-1) {
            os << ",";
        }
    }
    os << "]";

    return os;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const SunSpecMeasuredValue<T>& value) {
    os << "{";
    for (auto it = value.data().cbegin(); it != value.data().cend(); ++it) {
        os << "\"" << magic_enum::enum_name(it->first) << "\":" << it->second;
        if (std::distance(it, value.data().cend()) > 1) {
            os << ",";
        }
    }
    os << "}";
    return os;
}

std::ostream& operator<< (std::ostream& os, const std::map<sunspec::DataPoint, sunspec::DataValue>& values) {
    os << "{";
    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        os << "\"" << magic_enum::enum_name(it->first) << "\":" << it->second;
        if (std::distance(it, values.cend()) > 1) {
            os << ",";
        }
    }
    os << "}";
    return os;
}

// Explicit template instantiation
template std::ostream& operator<<(std::ostream& os, const std::vector<SunSpecMeasuredValue<uint>>& vec);
template std::ostream& operator<<(std::ostream& os, const SunSpecMeasuredValue<int>& value);
template std::ostream& operator<<(std::ostream& os, const SunSpecMeasuredValue<uint>& value);
