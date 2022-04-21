#pragma once

#include "dhcpoption.h"
#include "bootp.h"
#include "macaddress.h"

#include <QObject>
#include <QHostAddress>

class DhcpMessage : QObject
{
    Q_OBJECT

public:
    enum DhcpRequestType : quint8
    {
        DHCPDISCOVER = 1,
        DHCPOFFER,
        DHCPREQUEST,
        DHCPDECLINE,
        DHCPACK,
        DHCPNAK,
        DHCPRELEASE,
        DHCPINFORM
    };
    Q_ENUM(DhcpRequestType)

    DhcpMessage(QObject *parent = nullptr);
    DhcpMessage(QByteArray messageData, QObject *parent = nullptr);
    DhcpMessage(const DhcpMessage &other);
    DhcpMessage& operator=(const DhcpMessage &other);

    void setRequestType(DhcpRequestType type);
    void setClientMAC(MacAddress clientMac);
    void setOption(DhcpOption::Option option, QByteArray options_data);
    void setOptionRouter(QHostAddress address);
    void setOptionServerIdentifier(QHostAddress address);
    void setOptionLeaseTime(int seconds);
    void seOptionDns(QHostAddress address);
    void setOptionSubnet(QHostAddress address);
    void setResponseData(const uint32_t transactionId, const MacAddress clientMac, const QHostAddress offeredAddress, const QHostAddress subnetMask, const quint32 leaseTime, const QHostAddress serverIdentifier, const QString serverHostname);

    DhcpRequestType requestType() const;
    int transactionId() const;
    QHostAddress clientAddress() const;
    MacAddress clientMac() const;

    QString toString() const;
    QByteArray serialize() const;

    static DhcpMessage createOffer();
    static DhcpMessage createACK();
    static DhcpMessage createNAK();

private:
    void parseOption(QByteArray& data);

    bootp mHeader;
    QHostAddress mAddressClient{ "0.0.0.0" };
    QHostAddress mAddressYours{ "0.0.0.0" };
    QString mClientHostname;
    DhcpRequestType mRequestType;
    QMap<DhcpOption::Option, QByteArray> mOptions;
};

