#include "dhcpoption.h"

DhcpOption::Option DhcpOption::getOption(uint8_t rawValue)
{
    switch(rawValue)
    {
        case 0: return DhcpOption::PAD;
        case 12: return DhcpOption::HOSTNAME;
        case 50: return DhcpOption::REQUESTED_IP;
        case 53: return DhcpOption::DHCP_MESSAGE_TYPE;
        case 54: return DhcpOption::DHCP_SERVER_IDENTIFIER;
        case 55: return DhcpOption::PARAMETER_REQUEST_LIST;
        case 57: return DhcpOption::DHCP_MAX_SIZE;
        case 61: return DhcpOption::CLIENT_ID;
        case 255: return DhcpOption::END;
        default: return DhcpOption::UNKNOWN;
    }
}

QString DhcpOption::toString(Option option)
{
    switch(option)
    {
        case PAD: return "PAD";
        case SUBNET_MASK: return "SUBNET_MASK";
        case ROUTER: return "ROUTER";
        case DNS: return "DNS";
        case HOSTNAME: return "HOSTNAME";
        case REQUESTED_IP: return "REQUESTED_IP";
        case IP_LEASE_TIME: return "IP_LEASE_TIME";
        case DHCP_MESSAGE_TYPE: return "DHCP_MESSAGE_TYPE";
        case DHCP_SERVER_IDENTIFIER: return "DHCP_SERVER_IDENTIFIER";
        case PARAMETER_REQUEST_LIST: return "PARAMETER_REQUEST_LIST";
        case DHCP_MAX_SIZE: return "DHCP_MAX_SIZE";
        case CLIENT_ID: return "CLIENT_ID";
        case END: return "END";
        default: return QString("UNKNOWN -- %0").arg(option);
    }
}

QString DhcpOption::toString(uint8_t rawValue)
{
    switch(rawValue)
    {
        case 0: return "PAD";
        case 1: return "SUBNET_MASK";
        case 3: return "ROUTER";
        case 6: return "DNS";
        case 12: return "HOSTNAME";
        case 50: return "REQUESTED_IP";
        case 51: return "IP_LEASE_TIME";
        case 53: return "DHCP_MESSAGE_TYPE";
        case 54: return "DHCP_SERVER_IDENTIFIER";
        case 55: return "PARAMETER_REQUEST_LIST";
        case 57: return "DHCP_MAX_SIZE";
        case 61: return "CLIENT_ID";
        case 255: return "END";
        default: return QString("UNKNOWN -- %0").arg(rawValue);
    }
}
