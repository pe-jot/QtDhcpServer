#include "assignments/assignmentpool.h"

AssignmentPool::AssignmentPool(QObject *parent)
    : QObject(parent)
    , mpItemModel(new QStandardItemModel())
{
    mpItemModel->setHorizontalHeaderLabels(QStringList() << "IP Address" << "MAC Address" << "State");
}

AssignmentPool::~AssignmentPool()
{
    mAssignments.clear();
    mpItemModel->deleteLater();
}

bool AssignmentPool::addNewAssignment(DhcpAssignment* newAssignment)
{
    for (auto it = mAssignments.constBegin(); it != mAssignments.constEnd(); ++it)
    {
        if ((*it)->equals(newAssignment))
        {
            // Either MAC or IP exists already in our current pool
            return false;
        }
    }

    auto newRow = QList<QStandardItem*> {
            new QStandardItem(newAssignment->ip().toString()),
            new QStandardItem(newAssignment->mac().toString()),
            new QStandardItem(QVariant::fromValue(newAssignment->state()).toString().toLower())
    };
    mpItemModel->appendRow(newRow);

    mAssignments.append(newAssignment);
    connect(newAssignment, &DhcpAssignment::changed, this, &AssignmentPool::onAssignmentChanged);
    return true;
}

bool AssignmentPool::addNewAssignment(const QHostAddress& newIpAddress, const MacAddress& newMacAddress)
{
    return addNewAssignment(new DhcpAssignment(newIpAddress, newMacAddress));
}

QHostAddress AssignmentPool::offerAssignment(const MacAddress &mac)
{
    for (auto assignment = mAssignments.begin(); assignment != mAssignments.end(); ++assignment)
    {
        switch ((*assignment)->state())
        {
            case DhcpAssignment::ACQUIRED:
                if ((*assignment)->mac() == mac)
                {
                    // Same client, just set state
                    (*assignment)->setOffered(mac);
                    return (*assignment)->ip();
                }
                break;

            case DhcpAssignment::OFFERED:
                if ((*assignment)->mac() == mac)
                {
                    // Assignment already offered to this client
                    return (*assignment)->ip();
                }
                break;

            default:
                // New offer
                (*assignment)->setOffered(mac);
                return (*assignment)->ip();
        }
    }

    return QHostAddress::Null; // Not found - e.g. out of free assignments
}

bool AssignmentPool::acquireAssignment(const MacAddress &mac)
{
    for (auto assignment = mAssignments.begin(); assignment != mAssignments.end(); ++assignment)
    {
        if ((*assignment)->mac() == mac)
        {
            (*assignment)->setAcquired();
            return true; // Assignment found
        }
    }
    return false; // Not found
}

bool AssignmentPool::releaseAssignment(const MacAddress &mac)
{
    for (auto assignment = mAssignments.begin(); assignment != mAssignments.end(); ++assignment)
    {
        if ((*assignment)->mac() == mac)
        {
            (*assignment)->setReleased();
            return true; // Assignment found
        }
    }
    return false; // Not found
}

int AssignmentPool::count() const
{
    return mAssignments.count();
}

QVector<DhcpAssignment*> AssignmentPool::assignments() const
{
    return mAssignments;
}

QStandardItemModel* AssignmentPool::model() const
{
    return mpItemModel;
}

void AssignmentPool::onAssignmentChanged(DhcpAssignment* changedItem)
{
    // Notification from an assignment pool item that some data has been changed...
    auto ipString = changedItem->ip().toString();
    auto findResult = mpItemModel->findItems(ipString);
    if (findResult.count() != 1)
    {
        return;
    }
    auto changeIndex = findResult.first()->index();

    auto macString = changedItem->mac().toString();
    mpItemModel->setData(changeIndex.siblingAtColumn(1), macString);

    auto stateString = QVariant::fromValue(changedItem->state()).toString().toLower();
    mpItemModel->setData(changeIndex.siblingAtColumn(2), stateString);

    qDebug() << __FUNCTION__ << macString << "@" << ipString << "new state:" << stateString;
}
