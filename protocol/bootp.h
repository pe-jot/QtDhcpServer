#pragma once

#include "protocol/macaddress.h"

#include <QByteArray>
#include <QDebug>

enum class BootpOpType : quint8
{
    BOOT_REQUEST = 1,
    BOOT_REPLY = 2
};

enum class BootpHwType : quint8
{
    ETHERNET = 1
};

struct bootp
{
/* See also: https://datatracker.ietf.org/doc/html/rfc951
    struct bootp_header_t {
        uint8_t op;
        uint8_t htype;
        uint8_t hlen;
        uint8_t hops;
        uint32_t xid;
        uint16_t secs;
        uint16_t flags;
        uint32_t ciaddr;
        uint32_t yiaddr;
        uint32_t siaddr;    // IP address of next server to use in bootstrap; returned in DHCPOFFER, DHCPACK by server.
        uint32_t giaddr;    // Relay agent IP address, used in booting via a relay agent.
        uint8_t chaddr[16]; // Client hardware address.
        uint8_t sname[64]; // Optional server host name, null terminated string.
        uint8_t file[128]; // Boot file name, null terminated string; "generic" name or null in DHCPDISCOVER, fully qualified directory-path name in DHCPOFFER
        uint8_t cookie[4];
    };
*/

public:
    bootp();

    BootpOpType operation;
    BootpHwType hardwareAddressType;
    quint8 hardwareAddressLength { 0 };
    quint8 hops { 0 };
    quint32 transactionId { 0 };
    quint16 secs { 0 };
    quint16 flags { 0 };

    quint32 ciaddr { 0 };
    quint32 yiaddr { 0 };
    quint32 siaddr { 0 };
    quint32 giaddr { 0 };

    MacAddress hardwareAddressClient;
    QByteArray serverHostname;

    bool deserialize(QByteArray data);
    QByteArray serialize() const;

    const static quint16 DHCP_SERVER_PORT;
    const static quint16 DHCP_CLIENT_PORT;
    const static QByteArray COOKIE;
    const static quint8 CHADDR_FIELD_SIZE;
    const static quint8 SNAME_FIELD_SIZE;
    const static quint8 FILE_FIELD_SIZE;
};
