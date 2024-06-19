#pragma once

#include "dhcpserver.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QSettings>
#include <QRegExp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent (QCloseEvent *event) override;

private:
    void createTrayIcon();
    void populateNetworkInterfaces();
    void setUiEnable(const bool editable);
    void makeSettingSuggestion();
    void saveSettings();
    void hideMe();
    void showMe();

    Ui::MainWindow *ui;
    QSystemTrayIcon *mpTrayIcon;
    QMenu *mpTrayIconMenu;
    QAction *mpShowHideAction;
    QAction *mpQuitAction;
    QSettings* mpSettings;
    QStandardItemModel emptyDataModel;
    QHostAddress mSelectedInterfaceAddress;
    DhcpServer *mpServer;

    const QString defaultTrayIconText;

    static const QString exitMenuText;
    static const QString minimizeMenuText;
    static const QString showMenuText;
    static const QRegExp ipAddressRegex;
    static const int defaultLeaseTime;
    static const QString selectedInterfaceIpKey;
    static const QString startAddressKey;
    static const QString endAddressKey;
    static const QString netmaskKey;
    static const QString leaseTimeKey;

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowHideAction();
    void onQuitAction();
    void onSelectedNetworkInterfaceChanged(int index);
    void onServerStateChange(bool newState);
    void onAssignmentsTableViewDoubleClicked(const QModelIndex &index);
    void onCheckShowDebugOutputChanged(int state);
};
