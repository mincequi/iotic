#include <things/ThingValue.h>

/*
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
*/

Value fromQJsonValue(const QJsonValue& value) {
    if (value.isBool()) return value.toBool();
    if (value.isString()) return value.toString().toStdString();
    return value.toDouble();
}

QJsonValue toJsonValue(const Value& value) {
    return std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) return QJsonValue(QString::fromStdString(arg));
        else return QJsonValue(arg);
    }, value);
}

double toDouble(const Value& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    }
    return 0.0;
}
