#pragma once

#include "assignments/assignmentpool.h"

#include <QObject>
#include <QSettings>

class StaticAssignmentsPool : public AssignmentPool
{
    Q_OBJECT

public:
    StaticAssignmentsPool(QObject *parent = nullptr);

    QHostAddress offerAssignment(const MacAddress& mac) override;

private:
    void save() const;

    static const QString staticAssignmentsKey;
};
