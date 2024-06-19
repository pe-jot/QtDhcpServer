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
    QByteArray OUI() const;     // Organizationally Unique Identifier
    bool isEmpty() const;
    bool isMasked() const;      // True if vendor specific part is 00:00:00
    void clear();

    friend bool operator==(const MacAddress& a, const MacAddress& b)
    {
        if (a.isMasked() || b.isMasked())
        {
            return a.OUI() == b.OUI();
        }
        return a.mRawAddress == b.mRawAddress;
    }

private:
    QByteArray mRawAddress;

    static const int MAC_ADDRESS_LENGTH;
};
