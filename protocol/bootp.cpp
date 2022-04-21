#include "protocol/bootp.h"

#include <QDataStream>

const quint16 bootp::DHCP_SERVER_PORT = 67;
const quint16 bootp::DHCP_CLIENT_PORT = 68;
const QByteArray bootp::COOKIE { "\x63\x82\x53\x63" };
const quint8 bootp::CHADDR_FIELD_SIZE = 16;
const quint8 bootp::SNAME_FIELD_SIZE = 64;
const quint8 bootp::FILE_FIELD_SIZE = 128;

bootp::bootp()
{
}

bool bootp::deserialize(QByteArray data)
{
    QDataStream stream(data);

    quint8 op, htype;
    quint32 ciaddr, yiaddr, siaddr, giaddr;

    stream >>
        op >> htype >> hardwareAddressLength >> hops >>
        transactionId >>
        secs >> flags >>
        ciaddr >>
        yiaddr >>
        siaddr >>
        giaddr
    ;

    QByteArray chaddr;
    chaddr.resize(CHADDR_FIELD_SIZE);
    stream.readRawData(chaddr.data(), CHADDR_FIELD_SIZE);

    QByteArray sname;
    sname.resize(SNAME_FIELD_SIZE);
    stream.readRawData(sname.data(), SNAME_FIELD_SIZE);

    QByteArray file;
    file.resize(FILE_FIELD_SIZE);
    stream.readRawData(file.data(), FILE_FIELD_SIZE);

    QByteArray cookie;
    cookie.resize(COOKIE.size());
    stream.readRawData(cookie.data(), cookie.size());
    if (cookie != COOKIE)
    {
        qDebug() << "bad cookie";
        qDebug() << cookie;
        qDebug() << COOKIE;
        return false;
    }

    hardwareAddressType = static_cast<BootpHwType>(htype);
    operation = static_cast<BootpOpType>(op);

    if (hardwareAddressType == BootpHwType::ETHERNET && hardwareAddressLength == 6)
    {
        hardwareAddressClient = QByteArray(chaddr);
    }

    return true;
}

QByteArray bootp::serialize() const
{
    QByteArray outputArray;

    QDataStream stream(&outputArray, QIODevice::WriteOnly);
    stream <<
        static_cast<quint8>(operation) << static_cast<quint8>(hardwareAddressType) << hardwareAddressLength << hops <<
        transactionId <<
        secs << flags <<
        ciaddr <<
        yiaddr <<
        siaddr <<
        giaddr
    ;

    QByteArray chaddr;
    chaddr.fill(0, CHADDR_FIELD_SIZE);
    chaddr.replace(0, hardwareAddressClient.data().length(), hardwareAddressClient.data());
    stream.writeRawData(chaddr.data(), CHADDR_FIELD_SIZE);

    QByteArray sname;
    sname.fill(0, SNAME_FIELD_SIZE);
    auto length = serverHostname.length();
    if (length > SNAME_FIELD_SIZE - 1)
    {
        length = SNAME_FIELD_SIZE - 1;
    }
    sname.replace(0, length, serverHostname);
    stream.writeRawData(sname.data(), SNAME_FIELD_SIZE);

    QByteArray file;
    file.fill(0, FILE_FIELD_SIZE);
    stream.writeRawData(file.data(), FILE_FIELD_SIZE);

    stream.writeRawData(COOKIE.data(), COOKIE.size());

    return outputArray;
}
