#ifndef LIVEEXPORTER_H
#define LIVEEXPORTER_H

#include <sstream>

class SunSpecModel;

class LiveExporter {
public:
    LiveExporter();

private:
    std::stringstream m_ss;
};

#endif // LIVEEXPORTER_H
