#include "dhcpmessage.h"
#include "dhcpoption.h"

#include <QDataStream>

DhcpMessage::DhcpMessage(QObject *parent)
    : QObject(parent)
{}

DhcpMessage::DhcpMessage(QByteArray messageData, QObject *parent)
    : QObject(parent)
{
    auto preamble = messageData.left(240);
    messageData.remove(0, 240);

    auto headerOk = mHeader.deserialize(preamble);
    if (!headerOk)
    {
        qDebug() << "Couldn't deserialize header!";
        return;
    }

    if (mHeader.ciaddr > 0)
    {
        mAddressClient = QHostAddress(mHeader.ciaddr);
    }

    parseOption(messageData);
}

DhcpMessage::DhcpMessage(const DhcpMessage &other)
    : QObject(other.parent())
    , mHeader(other.mHeader)
    , mAddressClient(other.mAddressClient)
    , mAddressYours(other.mAddressYours)
    , mClientHostname(other.mClientHostname)
    , mRequestType(other.mRequestType)
    , mOptions(other.mOptions)
{
}

DhcpMessage& DhcpMessage::operator=(const DhcpMessage &other)
{
    mHeader = other.mHeader;
    mAddressClient = other.mAddressClient;
    mAddressYours = other.mAddressYours;
    mClientHostname = other.mClientHostname;
    mRequestType = other.mRequestType;
    mOptions = other.mOptions;
    return *this;
}

void DhcpMessage::parseOption(QByteArray& data)
{
    qDebug() << "***********************************";
    qDebug() << "[PARSING OPTIONS]";
    while(data.size())
    {
        auto optionType = DhcpOption::getOption(data.at(0));
        qDebug() << "--->[" << DhcpOption::toString(data.at(0)) << "]";
        data.remove(0, 1);
        if (optionType == DhcpOption::PAD)
        {
            continue;
        }
        else if (optionType == DhcpOption::END)
        {
            qDebug() << "padding=" << data.size();
            data.clear();
        }
        else if (optionType == DhcpOption::DHCP_MAX_SIZE)
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);
            data.remove(0, optionLength);
        }
        else if (optionType == DhcpOption::PARAMETER_REQUEST_LIST)
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);

            qDebug() << "requesting " << optionLength << " items";
            for(int i = 0; i < optionLength; i++)
            {
                quint8 requested_parameter = data.at(0);
                data.remove(0, 1);
                qDebug() << "requested: " << DhcpOption::toString(requested_parameter);
            }
        }
        else if (optionType == DhcpOption::REQUESTED_IP)
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);
            if (optionLength != 4)
            {
                qDebug() << "some problem with length: " << optionLength;
            }
            quint8 byte1 = data.at(0);
            quint8 byte2 = data.at(1);
            quint8 byte3 = data.at(2);
            quint8 byte4 = data.at(3);

            auto ip = QString("%0.%1.%2.%3")
                    .arg(byte1)
                    .arg(byte2)
                    .arg(byte3)
                    .arg(byte4);

            QHostAddress address(ip);
            mAddressClient = QHostAddress(ip);
            qDebug() << "requesting address " << address.toString();
            data.remove(0, 4);
        }
        else if (optionType == DhcpOption::DHCP_MESSAGE_TYPE)
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);
            if (optionLength != 1)
            {
                qDebug() << "invalid length for opt";
            }
            quint8 type = data.at(0);
            mRequestType = DhcpRequestType(type);
            qDebug() << "type: " << QVariant::fromValue(mRequestType).toString().toLower();
            data.remove(0, optionLength);
        }
        else if (optionType == DhcpOption::HOSTNAME)
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);

            mClientHostname = data.left(optionLength);
            qDebug() << "hostname " << mClientHostname;
            data.remove(0, optionLength);
        }
        else if (optionType == DhcpOption::CLIENT_ID)
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);

            quint8 type = data.at(0);
            data.remove(0, 1);
            optionLength--;

            QByteArray clientId = data.left(optionLength);
            data.remove(0, optionLength);
            if (static_cast<BootpHwType>(type) == BootpHwType::ETHERNET)
            {
                setClientMAC(clientId);
                qDebug() << "Client ID: " << clientMac().toString();
            }
            else
            {
                qDebug() << "Client ID type unknown: " << type;
            }
        }
        else
        {
            quint8 optionLength = data.at(0);
            data.remove(0, 1);

            qDebug() << "Removing " << int(optionLength) << "Bytes";
            data.remove(0, optionLength);
        }
    }
    qDebug() << "***********************************";
}

void DhcpMessage::setRequestType(DhcpRequestType type)
{
    mRequestType = type;
    QByteArray array;
    array.append(static_cast<quint8>(type));
    setOption(DhcpOption::DHCP_MESSAGE_TYPE, array);
}

void DhcpMessage::setClientMAC(MacAddress clientMac)
{
    mHeader.hardwareAddressClient = clientMac;
    mHeader.hardwareAddressLength = 6;
    mHeader.hardwareAddressType = BootpHwType::ETHERNET;
}

void DhcpMessage::setOption(DhcpOption::Option option, QByteArray optionsData)
{
    if (option == DhcpOption::END) return; // end option needs to be at the end of every message... we'll do it manually
    mOptions[option] = optionsData;
}

void DhcpMessage::setOptionRouter(QHostAddress address)
{
    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << address.toIPv4Address();
    }
    setOption(DhcpOption::ROUTER, data);
}

void DhcpMessage::setOptionServerIdentifier(QHostAddress address)
{
    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << address.toIPv4Address();
    }
    setOption(DhcpOption::DHCP_SERVER_IDENTIFIER, data);
}

void DhcpMessage::setOptionLeaseTime(int seconds)
{
    QByteArray leaseTime;
    {
        QDataStream stream(&leaseTime, QIODevice::WriteOnly);
        stream << quint32(seconds);
    }
    setOption(DhcpOption::IP_LEASE_TIME, leaseTime);
}

void DhcpMessage::setOptionSubnet(QHostAddress address)
{
    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << address.toIPv4Address();
    }
    setOption(DhcpOption::SUBNET_MASK, data);
}

void DhcpMessage::seOptionDns(QHostAddress address)
{
    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << address.toIPv4Address();
    }
    setOption(DhcpOption::DNS, data);
}

DhcpMessage::DhcpRequestType DhcpMessage::requestType() const
{
    return mRequestType;
}

int DhcpMessage::transactionId() const
{
    return mHeader.transactionId;
}

QHostAddress DhcpMessage::clientAddress() const
{
    return mAddressClient;
}

MacAddress DhcpMessage::clientMac() const
{
    return mHeader.hardwareAddressClient;
}

QString DhcpMessage::toString() const
{
    return QString("[%1] [%2] [%3]")
            .arg(QVariant::fromValue(mRequestType).toString().toLower(),
                 mClientHostname,
                 mHeader.hardwareAddressClient.toString()
                 );
}

QByteArray DhcpMessage::serialize() const
{
    QByteArray array;

    array.append(mHeader.serialize());

    auto i = mOptions.constBegin();
    while(i != mOptions.constEnd())
    {
        array.append(static_cast<quint8>(i.key()));
        array.append((quint8)i.value().length());
        array.append(i.value());

        ++i;
    }

    array.append(static_cast<quint8>(DhcpOption::END)); // always the last option
    return array;
}

DhcpMessage DhcpMessage::createOffer()
{
    DhcpMessage message;
    message.mHeader.operation = BootpOpType::BOOT_REPLY;
    message.setRequestType(DhcpRequestType::DHCPOFFER);
    return message;
}

DhcpMessage DhcpMessage::createACK()
{
    DhcpMessage message;
    message.mHeader.operation = BootpOpType::BOOT_REPLY;
    message.setRequestType(DhcpRequestType::DHCPACK);
    return message;
}

DhcpMessage DhcpMessage::createNAK()
{
    DhcpMessage message;
    message.setRequestType(DhcpRequestType::DHCPNAK);
    return message;
}

void DhcpMessage::setResponseData(const uint32_t transactionId, const MacAddress clientMac, const QHostAddress offeredAddress, const QHostAddress subnetMask, const quint32 leaseTime, const QHostAddress serverIdentifier, const QString serverHostname)
{
    mHeader.transactionId = transactionId;
    mHeader.serverHostname = serverHostname.toLatin1();
    mHeader.yiaddr = offeredAddress.toIPv4Address();
    mAddressYours = offeredAddress;

    setClientMAC(clientMac);
    setOptionSubnet(subnetMask);
    setOptionLeaseTime(leaseTime);
    setOptionServerIdentifier(serverIdentifier);
}
