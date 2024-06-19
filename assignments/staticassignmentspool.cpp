#include "assignments/staticassignmentspool.h"

const QString StaticAssignmentsPool::staticAssignmentsKey = QString("StaticAssignments");

StaticAssignmentsPool::StaticAssignmentsPool(QObject* parent)
    : AssignmentPool(parent)
{
    auto settings = new QSettings(); // Should work here also since it's the same application...

    settings->beginGroup(staticAssignmentsKey);
    const auto keys = settings->allKeys();
    for (auto key = keys.constBegin(); key != keys.constEnd(); ++key)
    {
        const auto mac = MacAddress(*key);
        const auto ip = QHostAddress(settings->value(*key).toString());
        if (!ip.isNull() && !mac.isEmpty())
        {
            addNewAssignment(ip, mac);
        }
    }
    settings->endGroup();
}

QHostAddress StaticAssignmentsPool::offerAssignment(const MacAddress &mac)
{
    auto staticAssignments = assignments();
    for (auto assignment = staticAssignments.begin(); assignment != staticAssignments.end(); ++assignment)
    {
        const auto assignmentMac = (*assignment)->mac();
        if (assignmentMac == mac)
        {
            // Partially or fully specified MAC address match
            (*assignment)->setOffered(assignmentMac.isMasked() ? assignmentMac : mac);
            return (*assignment)->ip();
        }

    }
    return QHostAddress::Null; // Not found
}

void StaticAssignmentsPool::save() const
{
    auto settings = new QSettings(); // Should work here also since it's the same application...

    settings->beginGroup(staticAssignmentsKey);
    settings->remove("");
    for (auto i = 0; i < assignments().count(); i++)
    {
        settings->setValue(
                    assignments().at(i)->mac().toString(),
                    assignments().at(i)->ip().toString());
    }
    settings->endGroup();

    settings->deleteLater();
}
