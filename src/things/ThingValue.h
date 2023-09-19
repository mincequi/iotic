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

Value fromQJsonValue(const QJsonValue& value);
QJsonValue toJsonValue(const Value& value);
double toDouble(const Value& value);
