#include "SunSpecDataValue.h"

namespace sunspec {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

StatsValue& StatsValue::operator=(const LiveValue& v) {
    std::visit(overload {
                   [&](uint32_t v) { m_variant = v; },
                   [&](InverterEvents v) { *this |= v; },
                   [&](InverterOperatingState v) { *this |= v; },
                   [&](int32_t v)  { m_variant = MeasuredValue<double>(v); },
                   [&](double v)   { m_variant = MeasuredValue<double>(v); },
                   [&](const std::vector<Block<double>>& v) {
                       auto pOut = std::get_if<std::vector<Block<MeasuredValue<double>>>>(&m_variant);
                       if (!pOut) {
                           m_variant = std::vector<Block<MeasuredValue<double>>>(v.size());
                       }
                       auto& out = std::get<std::vector<Block<MeasuredValue<double>>>>(m_variant);
                       out.resize(v.size());
                       for (size_t i = 0; i < v.size(); ++i) {
                           out[i] = v[i];
                       }
                   },
                   [&](const std::string&) {
                   }
               }, v);

    return *this;
}

StatsValue& StatsValue::operator|=(const InverterOperatingState& v) {
    if (!std::holds_alternative<std::set<InverterOperatingState>>(m_variant)) {
        m_variant.emplace<std::set<InverterOperatingState>>();
    }

    auto& states = std::get<std::set<InverterOperatingState>>(m_variant);
    m_isDirty = !states.count(v);

    states.insert(v);

    return *this;
}

StatsValue& StatsValue::operator|=(const InverterEvents& v) {
    if (!std::holds_alternative<InverterEvents>(m_variant)) {
        m_variant.emplace<InverterEvents>(v);
        m_isDirty = true;
        return *this;
    }

    auto& prev = std::get<InverterEvents>(m_variant);
    if (prev == (prev | v)) {
        m_isDirty = false;
    }
    prev |= v;

    return *this;
}

bool StatsValue::isDirty() const {
    return m_isDirty;
}

double StatsValue::minValue() const {
    if (std::holds_alternative<MeasuredValue<double>>(m_variant)) {
        const auto val = std::get<MeasuredValue<double>>(m_variant);
        if (val.data().count(min)) {
            return val.data().at(min);
        }
    }

    return 0.0;
}

double StatsValue::maxValue() const {
    if (std::holds_alternative<MeasuredValue<double>>(m_variant)) {
        const auto val = std::get<MeasuredValue<double>>(m_variant);
        if (val.data().count(min)) {
            return val.data().at(min);
        }
    }

    return 0.0;
}

} // namespace sunspec
