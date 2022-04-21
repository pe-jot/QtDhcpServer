#pragma once

#include <QByteArray>
#include <QString>

struct MacAddress
{
public:
    MacAddress(const QByteArray& data = QByteArray());
    MacAddress(const QString& macAddressString);

    QString toString() const;
    QByteArray data() const;
    bool isEmpty() const;
    void clear();

    friend inline bool operator==(const MacAddress& a, const MacAddress& b) { return a.mRawAddress == b.mRawAddress; }

private:
    QByteArray mRawAddress;

    static const int MAC_ADDRESS_LENGTH;
};
