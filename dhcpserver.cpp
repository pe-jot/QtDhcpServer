#include "dhcpserver.h"
#include "protocol/dhcpmessage.h"
#include "protocol/bootp.h"

#include <QNetworkDatagram>
#include <QHostInfo>

DhcpServer::DhcpServer(const QHostAddress& serverAddress, const QHostAddress& firstDynamicAddress, const QHostAddress& lastDynamicAddress, const QHostAddress& subnetmask, const int& leaseTime, QObject *parent)
    : QObject(parent)
    , mServerAddress(serverAddress)
    , mSubnetMask(subnetmask)
    , mLeaseTime(leaseTime)
{
    mpAssignmentsManager = new AssignmentsManager(firstDynamicAddress, lastDynamicAddress);
    mpSocket = new QUdpSocket(this);
    connect(mpSocket, &QUdpSocket::readyRead, this, &DhcpServer::readPendingDatagrams);
    auto isBound = mpSocket->bind(mServerAddress, bootp::DHCP_SERVER_PORT, QUdpSocket::ShareAddress);
    qDebug() << "Server socket connected:" << (isBound ? "true" : "false") << "at" << mServerAddress.toString();
}

DhcpServer::~DhcpServer()
{
    if (mpSocket)
    {
        mpSocket->deleteLater();
    }
    if (mpAssignmentsManager)
    {
        mpAssignmentsManager->deleteLater();
    }
}

QStandardItemModel* DhcpServer::dynamicAssignmentsItemModel() const
{
    return mpAssignmentsManager->dynamicAssignmentsItemModel();
}

QStandardItemModel* DhcpServer::staticAssignmentsItemModel() const
{
    return mpAssignmentsManager->staticAssignmentsItemModel();
}

void DhcpServer::readPendingDatagrams()
{
    while (mpSocket->hasPendingDatagrams())
    {
        auto datagram = mpSocket->receiveDatagram();
        try
        {
            DhcpMessage clientRequest(datagram.data());
            qDebug() << clientRequest.toString();

            const auto type = clientRequest.requestType();
            switch(type)
            {
                case DhcpMessage::DHCPDISCOVER:
                    performOffer(clientRequest);
                    break;

                case DhcpMessage::DHCPREQUEST:
                    performAcknowledge(clientRequest);
                    break;

                default:
                    break;
            }
        }
        catch (...)
        {
            continue;
        }
    }
}

void DhcpServer::releaseClient(QStandardItem* pMacAddress)
{
    if (mpAssignmentsManager)
    {
        auto mac = MacAddress(pMacAddress->text());
        if (!mac.isEmpty())
        {
            mpAssignmentsManager->releaseAssignment(mac);
        }
    }
}

void DhcpServer::performOffer(DhcpMessage &request)
{
    /*
    If the 'giaddr' field is zero and the 'ciaddr' field is nonzero, then the server
    unicasts DHCPOFFER and DHCPACK messages to the address in 'ciaddr'.
    If 'giaddr' is zero and 'ciaddr' is zero, and the broadcast bit is
    set, then the server broadcasts DHCPOFFER and DHCPACK messages to
    0xffffffff. If the broadcast bit is not set and 'giaddr' is zero and
    'ciaddr' is zero, then the server unicasts DHCPOFFER and DHCPACK
    messages to the client's hardware address and 'yiaddr' address.    In
    all cases, when 'giaddr' is zero, the server broadcasts any DHCPNAK
    messages to 0xffffffff.
    */

    auto clientMAC = request.clientMac();
    auto offeringAddress = mpAssignmentsManager->offerAssignment(clientMAC);
    qDebug() << "Offering address" << offeringAddress.toString() << "to" << clientMAC.toString();
    if (offeringAddress.isNull())
    {
        // Out of leases
        qDebug() << "Out of free DHCP leases!";
        mpSocket->writeDatagram(DhcpMessage::createNAK().serialize(), QHostAddress::Broadcast, bootp::DHCP_CLIENT_PORT);
        return;
    }

    auto response = DhcpMessage::createOffer();

    response.setResponseData(
                request.transactionId(),
                clientMAC,
                offeringAddress,
                mSubnetMask,
                mLeaseTime,
                mServerAddress,
                QHostInfo::localHostName());

    auto response_data = response.serialize();

    /*
    A server or relay agent sending or relaying a DHCP message directly
    to a DHCP client (i.e., not to a relay agent specified in the
    'giaddr' field) SHOULD examine the BROADCAST bit in the 'flags'
    field.    If this bit is set to 1, the DHCP message SHOULD be sent as
    an IP broadcast using an IP broadcast address (preferably 0xffffffff)
    as the IP destination address and the link-layer broadcast address as
    the link-layer destination address.    If the BROADCAST bit is cleared
    to 0, the message SHOULD be sent as an IP unicast to the IP address
    specified in the 'yiaddr' field and the link-layer address specified
    in the 'chaddr' field.    If unicasting is not possible, the message
    MAY be sent as an IP broadcast using an IP broadcast address
    (preferably 0xffffffff) as the IP destination address and the link-
    layer broadcast address as the link-layer destination address.
    */

    mpSocket->writeDatagram(response_data, QHostAddress::Broadcast, bootp::DHCP_CLIENT_PORT);
}

void DhcpServer::performAcknowledge(DhcpMessage &request)
{
    auto clientMAC = request.clientMac();
    auto clientIP = request.clientAddress();
    auto clientAssignedIP = mpAssignmentsManager->offerAssignment(clientMAC);

    if (clientAssignedIP != clientIP)
    {
        qDebug() << "Release assignment due to IP address mismatch:" << clientAssignedIP.toString() << clientIP.toString();
        mpAssignmentsManager->releaseAssignment(clientMAC);
        mpSocket->writeDatagram(DhcpMessage::createNAK().serialize(), QHostAddress::Broadcast, bootp::DHCP_CLIENT_PORT);
        return;
    }

    mpAssignmentsManager->acquireAssignment(clientMAC);

    auto response = DhcpMessage::createACK();

    response.setResponseData(
                request.transactionId(),
                clientMAC,
                clientAssignedIP,
                mSubnetMask,
                mLeaseTime,
                mServerAddress,
                QHostInfo::localHostName());

    auto response_data = response.serialize();
    mpSocket->writeDatagram(response_data, QHostAddress::Broadcast, bootp::DHCP_CLIENT_PORT);
}
