#include "assignmentsmanager.h"

#include <QMutableVectorIterator>

AssignmentsManager::AssignmentsManager(const QHostAddress& firstDynamicAddress, const QHostAddress& lastDynamicAddress, QObject *parent)
    : QObject(parent)
{
    mpStaticAssignments = new StaticAssignmentsPool();
    mpDynamicAssignments = new DynamicAssignmentsPool(firstDynamicAddress, lastDynamicAddress, mpStaticAssignments->assignments());
}

QStandardItemModel* AssignmentsManager::dynamicAssignmentsItemModel() const
{
    return mpDynamicAssignments ? mpDynamicAssignments->model() : nullptr;
}

QStandardItemModel* AssignmentsManager::staticAssignmentsItemModel() const
{
    return mpStaticAssignments ? mpStaticAssignments->model() : nullptr;
}

QHostAddress AssignmentsManager::offerAssignment(const MacAddress &clientMac)
{
    // Search all static assignments first
    auto ip = mpStaticAssignments->offerAssignment(clientMac);
    if (ip != QHostAddress::Null)
    {
        return ip;
    }

    // Then, try dynamic pool
    return mpDynamicAssignments->offerAssignment(clientMac);
}

void AssignmentsManager::acquireAssignment(const MacAddress &clientMac)
{
    // Try dynamic assignment pool first, then static
    if (!mpDynamicAssignments->acquireAssignment(clientMac))
    {
        mpStaticAssignments->acquireAssignment(clientMac);
    }
}

void AssignmentsManager::releaseAssignment(const MacAddress &clientMac)
{
    // Try dynamic assignment pool first, then static
    if (!mpDynamicAssignments->releaseAssignment(clientMac))
    {
        mpStaticAssignments->releaseAssignment(clientMac);
    }
}
