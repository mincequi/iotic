#include <things/ThingValue.h>

#include <QJsonArray>

Value fromQJsonValue(const QJsonValue& value) {
    if (value.isBool()) return value.toBool();
    if (value.isString()) return value.toString().toStdString();
    return value.toDouble();
}

QJsonValue toJsonValue(const Value& value) {
    return std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>)
                return QJsonValue(QString::fromStdString(arg));
        else if constexpr (std::is_same_v<T, std::array<double, 3>>)
                return QJsonValue(QJsonArray( {arg[0], arg[1], arg[2]} ));
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
