#include "SunSpecDataValue.h"

namespace sunspec {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

StatsValue& StatsValue::operator=(const LiveValue& v) {
    std::visit(overload {
                   [&](uint32_t v) { *this = StatsValue(v); },
                   [&](InverterEvents v) { *this |= StatsValue(v); },
                   [&](InverterOperatingState v) { *this = MeasuredValue<InverterOperatingState>(v); },
                   [&](int32_t v)  { *this = MeasuredValue<int32_t>(v); },
                   [&](double v)   { *this = MeasuredValue<double>(v); },
                   [&](const std::vector<Block<double>>& v) {
                       auto pOut = std::get_if<std::vector<Block<MeasuredValue<double>>>>(this);
                       if (!pOut) {
                           *this = std::vector<Block<MeasuredValue<double>>>(v.size());
                       }
                       auto& out = std::get<std::vector<Block<MeasuredValue<double>>>>(*this);
                       out.resize(v.size());
                       for (size_t i = 0; i < v.size(); ++i) {
                           out[i] = v[i];
                       }
                   }
               }, v);

    return *this;
}

StatsValue& StatsValue::operator|=(const StatsValue& v) {
    if (!std::holds_alternative<InverterEvents>(v)) {
        return *this;
    }

    if (!std::holds_alternative<InverterEvents>(*this)) {
        this->emplace<InverterEvents>(std::get<InverterEvents>(v));
        return *this;
    }

    auto& prev = std::get<InverterEvents>(*this);
    auto& next = std::get<InverterEvents>(v);
    if (prev == (prev | next)) {
        m_isDirty = false;
    }
    prev |= next;

    return *this;
}

bool StatsValue::isDirty() const {
    return m_isDirty;
}

} // namespace sunspec
