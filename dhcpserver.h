#pragma once

#include "assignments/assignmentsmanager.h"
#include "protocol/dhcpmessage.h"

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>

class DhcpServer : public QObject
{
    Q_OBJECT

public:
    DhcpServer(const QHostAddress& serverAddress, const QHostAddress& firstDynamicAddress, const QHostAddress& lastDynamicAddress, const QHostAddress& subnetmask, const int& leaseTime, QObject *parent = nullptr);
    ~DhcpServer();

    QStandardItemModel* staticAssignmentsItemModel() const;
    QStandardItemModel* dynamicAssignmentsItemModel() const;

signals:

public slots:

private slots:
    void readPendingDatagrams();

private:
    void performOffer(DhcpMessage& request);
    void performAcknowledge(DhcpMessage& request);

    AssignmentsManager *mpAssignmentsManager = nullptr;
    QUdpSocket *mpSocket = nullptr;
    QHostAddress mServerAddress;
    QHostAddress mSubnetMask;
    int mLeaseTime;
};
