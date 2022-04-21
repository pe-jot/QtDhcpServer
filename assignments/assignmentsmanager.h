#pragma once

#include "dynamicassignmentspool.h"
#include "staticassignmentspool.h"

#include <QObject>

class AssignmentsManager : public QObject
{
    Q_OBJECT

public:
    AssignmentsManager(const QHostAddress& firstDynamicAddress, const QHostAddress& lastDynamicAddress, QObject *parent = nullptr);

    QHostAddress offerAssignment(const MacAddress& clientMac);
    void acquireAssignment(const MacAddress& clientMac);
    void releaseAssignment(const MacAddress& clientMac);

    QStandardItemModel* dynamicAssignmentsItemModel() const;
    QStandardItemModel* staticAssignmentsItemModel() const;

private:
    DynamicAssignmentsPool *mpDynamicAssignments = nullptr;
    StaticAssignmentsPool *mpStaticAssignments = nullptr;
};

