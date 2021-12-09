#include "Util.h"

#include <QNetworkInterface>
#include <QStringList>

namespace util {

bool isMacAddressValid(const QString &address)
{
    QStringList list = address.split(":");

    return ((list.size() == 6) &&
            ((list.at(0).toInt() != 0) ||
             (list.at(1).toInt() != 0) ||
             (list.at(2).toInt() != 0) ||
             (list.at(3).toInt() != 0) ||
             (list.at(4).toInt() != 0) ||
             (list.at(5).toInt() != 0)));
}

QString getMacAddress()
{
    QString macAddress;
    foreach (QNetworkInterface networkInterface, QNetworkInterface::allInterfaces()) {
        // Return only the first non-loopback MAC Address
        if (!(networkInterface.flags() & QNetworkInterface::IsLoopBack)) {
            macAddress = networkInterface.hardwareAddress();
            if (isMacAddressValid(macAddress)) {
                break;
            }
        }
    }
    // TODO: mac address might be "00:00:00:00:00:00", which is illegal
    //qDebug() << __func__ << ": " << macAddress;
    return macAddress;
}

} // namespace util
