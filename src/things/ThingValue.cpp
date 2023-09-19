#include <things/ThingValue.h>

Value Value::fromQJsonValue(const QJsonValue& value) {
    if (value.isBool()) return value.toBool();
    if (value.isString()) return value.toString().toStdString();
    return value.toDouble();
}

QJsonValue Value::toJsonValue() const {
    if (std::holds_alternative<double>(*this)) {
        return std::get<double>(*this);
    } else if (std::holds_alternative<bool>(*this)) {
        return std::get<bool>(*this);
    } else if (std::holds_alternative<std::string>(*this)) {
        return QString::fromStdString(std::get<std::string>(*this));
    }
    return {};
}

double Value::toDouble() const {
    if (std::holds_alternative<double>(*this)) {
        return std::get<double>(*this);
    } else if (std::holds_alternative<bool>(*this)) {
        return std::get<bool>(*this) ? 1.0 : 0.0;
    }
    return 0.0;
}
