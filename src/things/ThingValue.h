#pragma once

#include <string>
#include <variant>
#include <QJsonValue>

/*
class Value : public std::variant<bool, double, std::string> {
public:
    static Value fromQJsonValue(const QJsonValue& value);
    QJsonValue toJsonValue() const;
    using std::variant<bool, double, std::string>::variant;

    double toDouble() const;
};
*/

using Value = std::variant<bool, int, double, std::string>;

template <typename T0, typename ... Ts>
std::ostream& operator<<(std::ostream& s, std::variant<T0, Ts...> const& v){
    std::visit([&](auto && arg) { s << arg; }, v); return s;
}

Value fromQJsonValue(const QJsonValue& value);
QJsonValue toJsonValue(const Value& value);
double toDouble(const Value& value);
