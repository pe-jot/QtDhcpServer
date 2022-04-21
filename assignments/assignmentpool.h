#pragma once

#include "assignments/dhcpassignment.h"

#include <QObject>
#include <QStandardItemModel>
#include <QVector>

class AssignmentPool : public QObject
{
    Q_OBJECT

protected:
    AssignmentPool(QObject* parent = nullptr);
    virtual ~AssignmentPool();

public:
    bool addNewAssignment(const QHostAddress& newIpAddress, const MacAddress& newMacAddress = MacAddress());
    bool addNewAssignment(DhcpAssignment* newAssignment);
    bool acquireAssignment(const MacAddress& mac);
    bool releaseAssignment(const MacAddress& mac);
    virtual QHostAddress offerAssignment(const MacAddress& mac);

    int count() const;
    QStandardItemModel* model() const;
    QVector<DhcpAssignment*> assignments() const;

private slots:
    void onAssignmentChanged(DhcpAssignment* changedItem);

private:
    QStandardItemModel* mpItemModel = nullptr;
    QVector<DhcpAssignment*> mAssignments;
};

