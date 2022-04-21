#include "assignments/dhcpassignment.h"

#include <QVariantMap>
#include <QJsonDocument>

const int DhcpAssignment::OFFER_TIMEOUT_MS = 60 * 1000;

DhcpAssignment::DhcpAssignment(QObject *parent)
    : QObject(parent)
{
}

DhcpAssignment::DhcpAssignment(const QHostAddress ip, const MacAddress mac, const AssignmentState state, QObject *parent)
    : QObject(parent)
    , mMacAddress(mac)
    , mIpAddress(ip)
    , mState(state)
{
    initTimer();
}

DhcpAssignment::DhcpAssignment(const DhcpAssignment &other)
    : QObject(other.parent())
    , mMacAddress(other.mMacAddress)
    , mIpAddress(other.mIpAddress)
    , mState(other.mState)
    , mpOfferTimeout(other.mpOfferTimeout)
{
}

DhcpAssignment& DhcpAssignment::operator=(const DhcpAssignment &other)
{
    mMacAddress = other.mMacAddress;
    mIpAddress = other.mIpAddress;
    mState = other.mState;
    mpOfferTimeout = other.mpOfferTimeout;
    return *this;
}

DhcpAssignment::~DhcpAssignment()
{
}

void DhcpAssignment::initTimer()
{
    // Timeout does occur if client e.g. misses to complete its request. Use this to avoid running out of assignments due to client misbehaviour.
    mpOfferTimeout = new QTimer();
    mpOfferTimeout->setSingleShot(true);
    mpOfferTimeout->setInterval(OFFER_TIMEOUT_MS);
    connect(mpOfferTimeout, &QTimer::timeout, this, &DhcpAssignment::setReleased);
}

void DhcpAssignment::setOffered(const MacAddress mac)
{
    mMacAddress = mac;
    mState = OFFERED;
    mpOfferTimeout->start();
    emit changed(this);
}

void DhcpAssignment::setAcquired()
{
    mState = ACQUIRED;
    mpOfferTimeout->stop();
    emit changed(this);
}

void DhcpAssignment::setReleased()
{
    mMacAddress.clear();
    mState = RELEASED;
    mpOfferTimeout->stop();
    emit changed(this);
}

bool DhcpAssignment::equals(const DhcpAssignment &other) const
{
    if (mMacAddress.isEmpty() || other.mMacAddress.isEmpty())
    {
        return mIpAddress == other.mIpAddress;
    }
    return (mMacAddress == other.mMacAddress || mIpAddress == other.mIpAddress);
}

QHostAddress DhcpAssignment::ip() const
{
    return mIpAddress;
}

MacAddress DhcpAssignment::mac() const
{
    return mMacAddress;
}

DhcpAssignment::AssignmentState DhcpAssignment::state() const
{
    return mState;
}

QJsonObject DhcpAssignment::toJsonObject() const
{
    // Serialize to JSON format for saving static DHCP assignments to file
    return QJsonObject
    {
        { "mac", mMacAddress.toString() },
        { "ip", mIpAddress.toString() }
    };
}
