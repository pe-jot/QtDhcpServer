#pragma once

#include "assignments/assignmentpool.h"

#include <QObject>
#include <QHostAddress>

class DynamicAssignmentsPool : public AssignmentPool
{
    Q_OBJECT

public:
    DynamicAssignmentsPool(const QHostAddress& firstAddress, const QHostAddress& lastAddress, const QVector<DhcpAssignment*> staticDhcpPool, QObject *parent = nullptr);

private:
    QHostAddress mFirstAddress;
    QHostAddress mLastAddress;
};
