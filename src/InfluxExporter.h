#ifndef INFLUXEXPORTER_H
#define INFLUXEXPORTER_H

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include <InfluxDB.h>

namespace sunspec {
class Model;
class SunSpecThing;
}

class InfluxExporter {
public:
    class Builder {
    public:
        operator std::optional<InfluxExporter>() const;

        Builder& host(const std::string& host);
        Builder& port(uint16_t port);

    private:
        Builder(const std::string& db);

        std::string m_host = "localhost";
        uint16_t m_port = 8086;
        std::string m_db;

        friend class InfluxExporter;
    };

    static Builder build(const std::string& db);

    void exportLiveData(const sunspec::SunSpecThing& thing, const sunspec::Model& model, uint32_t timestamp);

private:
    explicit InfluxExporter(const std::string& db, const std::string& host, uint16_t port);

    std::unique_ptr<influxdb::InfluxDB> m_db;
};

#endif // INFLUXEXPORTER_H
