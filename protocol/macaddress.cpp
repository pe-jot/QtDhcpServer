#include "protocol/macaddress.h"

#include <QStringList>
#include <QDebug>

const int MacAddress::MAC_ADDRESS_LENGTH = 6;

MacAddress::MacAddress(const QByteArray& data)
    : mRawAddress(data.mid(0, MAC_ADDRESS_LENGTH))
{
}

MacAddress::MacAddress(const QString& macAddressString)
{
    auto macAddressParts = macAddressString.split(':', Qt::SkipEmptyParts);

    if (macAddressParts.count() != MAC_ADDRESS_LENGTH)
    {
        return;
    }

    mRawAddress.resize(MAC_ADDRESS_LENGTH);

    auto parseStatus = false;
    for(auto i = 0; i < MAC_ADDRESS_LENGTH; i++)
    {
        mRawAddress[i] = quint8(macAddressParts[i].toUInt(&parseStatus, 16));
        if (!parseStatus)
        {
            mRawAddress.clear();
            return;
        }
    }
}

QString MacAddress::toString() const
{
    if (mRawAddress.length() != MAC_ADDRESS_LENGTH)
    {
        return QString();
    }

    return QString("%0:%1:%2:%3:%4:%5")
            .arg(QString::number(quint8(mRawAddress[0]), 16), 2, '0')
            .arg(QString::number(quint8(mRawAddress[1]), 16), 2, '0')
            .arg(QString::number(quint8(mRawAddress[2]), 16), 2, '0')
            .arg(QString::number(quint8(mRawAddress[3]), 16), 2, '0')
            .arg(QString::number(quint8(mRawAddress[4]), 16), 2, '0')
            .arg(QString::number(quint8(mRawAddress[5]), 16), 2, '0')
    ;
}

QByteArray MacAddress::data() const
{
    return mRawAddress;
}

QByteArray MacAddress::OUI() const
{
    return mRawAddress.left(3);
}

bool MacAddress::isEmpty() const
{
    return mRawAddress.isEmpty();
}

bool MacAddress::isMasked() const
{
    if (mRawAddress.length() < 6)
    {
        return false;
    }
    return (mRawAddress[3] == 0 && mRawAddress[4] == 0 && mRawAddress[5] == 0);
}

void MacAddress::clear()
{
    mRawAddress.clear();
}
