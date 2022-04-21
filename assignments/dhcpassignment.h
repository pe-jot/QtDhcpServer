#pragma once

#include "protocol/macaddress.h"

#include <QObject>
#include <QHostAddress>
#include <QJsonObject>
#include <QTimer>

class DhcpAssignment : public QObject
{
Q_OBJECT

public:
    enum AssignmentState
    {
        RELEASED = 0,
        OFFERED = 1,
        ACQUIRED = 2
    };
    Q_ENUM(AssignmentState)

    DhcpAssignment(QObject *parent = nullptr);
    DhcpAssignment(const QHostAddress ip, const MacAddress mac = MacAddress(), const AssignmentState state = RELEASED, QObject *parent = nullptr);
    DhcpAssignment(const DhcpAssignment &other);
    DhcpAssignment& operator=(const DhcpAssignment &other);
    ~DhcpAssignment();

    void setOffered(const MacAddress mac);
    void setAcquired();
    void setReleased();
    bool equals(const DhcpAssignment &other) const;

    QHostAddress ip() const;
    MacAddress mac() const;
    AssignmentState state() const;

    QJsonObject toJsonObject() const;

signals:
    void changed(DhcpAssignment* changedItem);

private:
    static const int OFFER_TIMEOUT_MS;

    MacAddress mMacAddress;
    QHostAddress mIpAddress;
    AssignmentState mState;
    QTimer* mpOfferTimeout = nullptr;

    void initTimer();
};

Q_DECLARE_METATYPE(DhcpAssignment)
