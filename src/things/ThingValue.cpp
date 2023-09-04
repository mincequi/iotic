#include <things/ThingValue.h>

ThingValue ThingValue::fromQJsonValue(const QJsonValue& value) {
    if (value.isBool()) return value.toBool();
    return value.toDouble();
}
