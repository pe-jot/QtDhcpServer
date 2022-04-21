#include "assignments/dynamicassignmentspool.h"

DynamicAssignmentsPool::DynamicAssignmentsPool(const QHostAddress& firstAddress, const QHostAddress& lastAddress, const QVector<DhcpAssignment*> staticDhcpPool, QObject *parent)
    : AssignmentPool(parent)
    , mFirstAddress(firstAddress)
    , mLastAddress(lastAddress)
{
    auto baseAddress = firstAddress.toIPv4Address();
    if (baseAddress > lastAddress.toIPv4Address())
    {
        return;
    }

    // Get static IP addresses out of their pool.
    // Unfortunately there is no sleek query syntax for doing this, so we have to stick with this clumsy loop.
    QList<QHostAddress> staticAddresses;
    for (auto it = staticDhcpPool.constBegin(); it != staticDhcpPool.constEnd(); ++it)
    {
        staticAddresses.append((*it)->ip());
    }

    unsigned int addressCount = lastAddress.toIPv4Address() - baseAddress + 1;
    for (unsigned int i = 0; i < addressCount; i++)
    {
        QHostAddress newAddress = QHostAddress(baseAddress + i);
        if (staticAddresses.contains(newAddress))
        {
            continue;
        }
        addNewAssignment(newAddress);
    }
}
