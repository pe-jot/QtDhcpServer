#pragma once

#include <QString>

class DhcpOption
{
public:
    enum Option
    {
        PAD = 0,
        SUBNET_MASK = 1,
        ROUTER = 3,
        DNS = 6,
        HOSTNAME = 12,
        REQUESTED_IP = 50,
        IP_LEASE_TIME = 51,
        DHCP_MESSAGE_TYPE = 53,
        DHCP_SERVER_IDENTIFIER = 54,
        PARAMETER_REQUEST_LIST = 55,
        DHCP_MAX_SIZE = 57,
        CLIENT_ID = 61,
        END = 255,
        UNKNOWN = 256 // out of range intentionally
    };

    static Option getOption(uint8_t rawValue);
    static QString toString(Option option);
    static QString toString(uint8_t rawValue);
};
