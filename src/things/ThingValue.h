#pragma once

#include <variant>
#include <QJsonValue>

class ThingValue : public std::variant<bool, double> {
public:
    static ThingValue fromQJsonValue(const QJsonValue& value);
    using std::variant<bool, double>::variant;

    double toDouble() const;
};
