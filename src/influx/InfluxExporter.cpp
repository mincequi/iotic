#include "InfluxExporter.h"

#include <InfluxDBFactory.h>
#include <magic_enum.hpp>
#include <common/Logger.h>
#include <iot/sunspec/SunSpecModel.h>
#include <iot/sunspec/SunSpecThing.h>
#include <iot/sunspec/SunSpecTypes.h>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

InfluxExporter::Builder::Builder(const std::string& db) : m_db(db) {
}

InfluxExporter::Builder& InfluxExporter::Builder::host(const std::string& host) {
    m_host = host;
    return *this;
}

InfluxExporter::Builder& InfluxExporter::Builder::port(uint16_t port) {
    m_port = port;
    return *this;
}

InfluxExporter::Builder InfluxExporter::build(const std::string& db) {
    return Builder(db);
}

InfluxExporter::Builder::operator std::optional<InfluxExporter>() const {
    try {
        InfluxExporter exporter(m_db, m_host, m_port);
        return exporter;
    } catch(const std::exception& e) {
        LOG_S(ERROR) << e.what();
    }

    return {};
}

InfluxExporter::InfluxExporter(const std::string& db, const std::string& host, uint16_t port) {
    m_db = influxdb::InfluxDBFactory::Get("http://" + host + ":" + std::to_string(port) + "?db=" + db);
    m_db->createDatabaseIfNotExists();
}

void InfluxExporter::exportLiveData(const sunspec::SunSpecThing& thing, const sunspec::Model& model, uint32_t timestamp) {
    for (const auto& kv: model.values()) {

        std::optional<std::variant<int, long long int, std::string, double>> influxValue;
        std::visit(overload {
                       [&](sunspec::InverterEvents v) { std::stringstream ss; ss << v; influxValue = ss.str(); },
                       [&](sunspec::InverterOperatingState v) { std::stringstream ss; ss << v; influxValue = ss.str(); },
                       [&](int32_t v)  { influxValue = v; },
                       [&](double v)   { influxValue = v; },
                       [&](const std::vector<sunspec::Block<double>>& v) {
                           for (size_t i = 0; i < v.size(); ++i) {
                               for (const auto& bkv : v.at(i).data()) {
                                   std::string name{magic_enum::enum_name(kv.first)};
                                   name += "." + std::to_string(i) + "." + std::string{magic_enum::enum_name(bkv.first)};
                                   // TODO: as of now, we only write integers for blocks, since they are more efficient.
                                   m_db->write(influxdb::Point{thing.sunSpecId()}
                                               .setTimestamp(std::chrono::system_clock::time_point(std::chrono::seconds(timestamp)))
                                               .addField(name, (int)std::round(bkv.second)));
                               }
                           }
                       },
                       [&](const std::string& v) {influxValue = v;},
                       [&](uint32_t) {}     // ignore timestamp
                       /*[&](const auto& v) {
                           LOG_S(WARNING) << "unknown type: " << typeid(v).name();
                       }*/
                   }, kv.second);

        if (influxValue) {
            //try {
            m_db->write(influxdb::Point{thing.sunSpecId()}
                        .setTimestamp(std::chrono::system_clock::time_point(std::chrono::seconds(timestamp)))
                        .addField(magic_enum::enum_name(kv.first), *influxValue));
            //} catch (const std::exception& e) {
            //}
        }
    }
}
