#include "SunSpecStatsModel.h"

#include "SunSpecModel.h"
#include "SunSpecTypes.h"

namespace sunspec {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

StatsModel::StatsModel() {
}

StatsModel& StatsModel::operator=(const Model& model) {
    m_modelId = model.modelId();
    for (const auto& kv : model.values()) {
        m_values[kv.first] = kv.second;
    }

    return *this;
}

uint16_t StatsModel::modelId() const {
    return m_modelId;
}

bool StatsModel::isDirty() const {
    for (const auto& kv : m_values) {
        if (kv.second.isDirty()) {
            return true;
        }
        auto isDirty =
                std::visit(
                    overload{
                        [](uint32_t)    { return false; },
                        [](InverterEvents)    { return false; },
                        [](MeasuredValue<InverterOperatingState> v) { return v.isDirty(); },
                        [](MeasuredValue<int32_t> v)  { return v.isDirty(); },
                        [](MeasuredValue<double> v)  { return v.isDirty(); },
                        [](const std::vector<Block<MeasuredValue<double>>>& v) {
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
