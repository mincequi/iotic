#include "SunSpecStatsModel.h"

#include "SunSpecModel.h"

namespace sunspec {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

StatsModel::StatsModel() {
}

StatsModel& StatsModel::operator=(const Model& model) {
    m_modelId = model.modelId();
    for (const auto& kv : model.values()) {
        std::visit(overload {
                       [&](uint32_t v) { m_values[kv.first] = StatsValue(v); },
                       [&](int32_t v)  { m_values[kv.first] = SunSpecMeasuredValue<int32_t>(v); },
                       [&](double v)   { m_values[kv.first] = SunSpecMeasuredValue<double>(v); },
                       [&](const std::vector<Block<double>>& v) {
                           auto pOut = std::get_if<std::vector<Block<SunSpecMeasuredValue<double>>>>(&m_values[kv.first]);
                           if (!pOut) {
                               m_values[kv.first] = std::vector<Block<SunSpecMeasuredValue<double>>>(v.size());
                           }
                           auto& out = std::get<std::vector<Block<SunSpecMeasuredValue<double>>>>(m_values[kv.first]);
                           out.resize(v.size());
                           for (size_t i = 0; i < v.size(); ++i) {
                               out[i] = v[i];
                           }
                       }
                   }, kv.second);
    }

    return *this;
}

uint16_t StatsModel::modelId() const {
    return m_modelId;
}

bool StatsModel::isDirty() const {
    for (const auto& kv : m_values) {
        auto isDirty =
                std::visit(
                    overload{
                        [](uint32_t)    { return false; },
                        [](SunSpecMeasuredValue<int32_t> v)  { return v.isDirty(); },
                        [](SunSpecMeasuredValue<double> v)  { return v.isDirty(); },
                        [](const std::vector<Block<SunSpecMeasuredValue<double>>>& v) {
                            for (const auto& b : v) {
                                for (const auto& kv : b.data()) {
                                    if (kv.second.isDirty()) {
                                        return true;
                                    }
                                }
                            }
                            return false;
                        }
                    }, kv.second);

        if (isDirty) {
            return true;
        }
    }

    return false;
}

const std::map<DataPoint, StatsValue>& StatsModel::values() const {
    return m_values;
}

} // namespace sunspec
