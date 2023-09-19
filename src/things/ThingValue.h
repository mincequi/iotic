#pragma once

#include <string>
#include <variant>
#include <QJsonValue>

class ThingValue : public std::variant<bool, double, std::string> {
public:
    static ThingValue fromQJsonValue(const QJsonValue& value);
    QJsonValue toJsonValue() const;
    using std::variant<bool, double, std::string>::variant;

    double toDouble() const;
};
