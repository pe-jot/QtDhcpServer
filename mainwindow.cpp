#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPushButton>


const QString MainWindow::exitMenuText = QString("Exit DHCP Server");
const QString MainWindow::minimizeMenuText = QString("Minimize to tray");
const QString MainWindow::showMenuText = QString("Open DHCP Server");
const QRegExp MainWindow::ipAddressRegex = QRegExp("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
const int MainWindow::defaultLeaseTime = 24 * 60 * 60; // 1 day

const QString MainWindow::selectedInterfaceIpKey = QString("SelectedInterfaceIP");
const QString MainWindow::startAddressKey = QString("StartAddress");
const QString MainWindow::endAddressKey = QString("EndAddress");
const QString MainWindow::netmaskKey = QString("Netmask");
const QString MainWindow::leaseTimeKey = QString("LeaseTime");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mpSettings(new QSettings())
    , defaultTrayIconText(QCoreApplication::applicationName())
{
    ui->setupUi(this);
    ui->debugOutputText->hide();

    ui->lineEditSubnetmask->setValidator(new QRegExpValidator(ipAddressRegex));
    ui->lineEditStartAddress->setValidator(new QRegExpValidator(ipAddressRegex));
    ui->lineEditEndAddress->setValidator(new QRegExpValidator(ipAddressRegex));

    createTrayIcon();
    populateNetworkInterfaces();

    connect(ui->buttonServerState, &QPushButton::toggled, this, &MainWindow::onServerStateChange);
    connect(ui->buttonRestoreDefaultSettings, &QPushButton::clicked, this, &MainWindow::makeSettingSuggestion);
    connect(ui->buttonSaveSettings, &QPushButton::clicked, this, &MainWindow::saveSettings);
    connect(ui->dynamicAssignmentsTableView, &QAbstractItemView::doubleClicked, this, &MainWindow::onAssignmentsTableViewDoubleClicked);
    connect(ui->staticAssignmentsTableView, &QAbstractItemView::doubleClicked, this, &MainWindow::onAssignmentsTableViewDoubleClicked);
    connect(ui->chkShowDebugOutput, &QCheckBox::stateChanged, this, &MainWindow::onCheckShowDebugOutputChanged);
}

MainWindow::~MainWindow()
{
    mpSettings->setValue(selectedInterfaceIpKey, ui->comboBoxInterface->currentData());
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *event)
{
#ifndef QT_DEBUG
    // Close means minimize to tray
    hideMe();
    event->ignore();
#else
    // Don't use this when debugging, because it's annoying
    Q_UNUSED(event)
#endif
}


void MainWindow::populateNetworkInterfaces()
{
    const QString preselectedInterfaceIp = mpSettings->value(selectedInterfaceIpKey).toString();
    int preselectedInterfaceIndex = -1;

    auto interfaces = QNetworkInterface::allInterfaces();
    for (auto interface = interfaces.constBegin(); interface != interfaces.constEnd(); ++interface)
    {
        if (interface->addressEntries().isEmpty())
        {
            continue;
        }

        auto flags = interface->flags();
        if (flags.testFlag(QNetworkInterface::CanBroadcast) && !flags.testFlag(QNetworkInterface::IsLoopBack))
        {
            auto addressEntries = interface->addressEntries();
            for (auto address = addressEntries.constBegin(); address != addressEntries.constEnd(); ++address)
            {
                if (address->ip().protocol() != QAbstractSocket::IPv4Protocol)
                {
                    continue;
                }

                auto ipString = address->ip().toString();
                auto name = QString("%1 | %2").arg(interface->humanReadableName(), ipString);

                if (ipString == preselectedInterfaceIp)
                {
                    preselectedInterfaceIndex = ui->comboBoxInterface->count();
                }

                // We identify the interface by its IP address later on, so we add this information to the user data
                ui->comboBoxInterface->addItem(name, ipString);
            }
        }
    }

    // Need to set the current index to something invalid to get an onSelectedNetworkInterfaceChanged() later on...
    ui->comboBoxInterface->setCurrentIndex(-1);

    // Connect signal after filling list to get some data when currentIndexChanged is fired.
    connect(ui->comboBoxInterface, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MainWindow::onSelectedNetworkInterfaceChanged);

    if (ui->comboBoxInterface->count() > 0)
    {
        ui->comboBoxInterface->setCurrentIndex((preselectedInterfaceIndex == -1) ? 0 : preselectedInterfaceIndex);
    }
}

void MainWindow::makeSettingSuggestion()
{
    auto interfaces = QNetworkInterface::allInterfaces();
    QNetworkAddressEntry selectedAddress;

    for (auto interface = interfaces.constBegin(); interface != interfaces.constEnd(); ++interface)
    {
        auto addressEntries = interface->addressEntries();
        for (auto address = addressEntries.constBegin(); address != addressEntries.constEnd(); ++address)
        {
            if (address->ip() == mSelectedInterfaceAddress)
            {
                selectedAddress = *address;
                break;
            }
        }
    }

    // Make some automatic IP range suggestions based on our selected network interface
    auto dhcp_start_address = QHostAddress(selectedAddress.ip().toIPv4Address() + 1);
    ui->lineEditStartAddress->setText(dhcp_start_address.toString());

    auto dhcp_end_address = QHostAddress(selectedAddress.ip().toIPv4Address() + 10);
    ui->lineEditEndAddress->setText(dhcp_end_address.toString());

    // Netmask from interface netmask make sense to be in the same subnet
    ui->lineEditSubnetmask->setText(selectedAddress.netmask().toString());

    ui->spinBoxLeasetime->setValue(defaultLeaseTime);
}

void MainWindow::saveSettings()
{
    // Windows: see Registry: Computer\HKEY_CURRENT_USER\SOFTWARE\<username>\DHCP Server
    mpSettings->beginGroup(mSelectedInterfaceAddress.toString());
    mpSettings->setValue(startAddressKey, ui->lineEditStartAddress->text());
    mpSettings->setValue(endAddressKey, ui->lineEditEndAddress->text());
    mpSettings->setValue(netmaskKey, ui->lineEditSubnetmask->text());
    mpSettings->setValue(leaseTimeKey, ui->spinBoxLeasetime->value());
    mpSettings->endGroup();
}


void MainWindow::createTrayIcon()
{
    mpTrayIcon = new QSystemTrayIcon(QIcon(":/Connected.ico"), this);

    mpTrayIcon->setToolTip(defaultTrayIconText);
    connect(mpTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));

    mpQuitAction = new QAction(exitMenuText, mpTrayIcon);
    connect(mpQuitAction, SIGNAL(triggered()), this, SLOT(onQuitAction()));

    mpShowHideAction = new QAction(minimizeMenuText, mpTrayIcon);
    connect(mpShowHideAction, SIGNAL(triggered()), this, SLOT(onShowHideAction()));

    mpTrayIconMenu = new QMenu();
    mpTrayIconMenu->addAction(mpShowHideAction);
    mpTrayIconMenu->addAction(mpQuitAction);

    mpTrayIcon->setContextMenu(mpTrayIconMenu);
    mpTrayIcon->show();
}

void MainWindow::setUiEnable(const bool editable)
{
    ui->comboBoxInterface->setEnabled(editable);
    ui->lineEditStartAddress->setEnabled(editable);
    ui->lineEditEndAddress->setEnabled(editable);
    ui->lineEditSubnetmask->setEnabled(editable);
    ui->spinBoxLeasetime->setEnabled(editable);
    ui->buttonRestoreDefaultSettings->setEnabled(editable);
}

void MainWindow::showMe()
{
    show();
    raise();
    setFocus();
    mpShowHideAction->setText(minimizeMenuText);
}

void MainWindow::hideMe()
{
    hide();
    mpShowHideAction->setText(showMenuText);
}


void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger)
    {
        show();
    }
}

void MainWindow::onQuitAction()
{
    // This is the actual exit of the program
    QApplication::exit();
}

void MainWindow::onShowHideAction()
{
    if (isVisible())
    {
        hideMe();
    }
    else
    {
        showMe();
    }
}

void MainWindow::onSelectedNetworkInterfaceChanged(int index)
{
    // We determine the selected interface by its IP address
    auto selectedInterfaceIpString = ui->comboBoxInterface->itemData(index).toString();
    mSelectedInterfaceAddress = QHostAddress(selectedInterfaceIpString);

    auto settingGroups = mpSettings->childGroups();
    if (settingGroups.contains(selectedInterfaceIpString))
    {
        mpSettings->beginGroup(selectedInterfaceIpString);
        ui->lineEditStartAddress->setText(mpSettings->value(startAddressKey).toString());
        ui->lineEditEndAddress->setText(mpSettings->value(endAddressKey).toString());
        ui->lineEditSubnetmask->setText(mpSettings->value(netmaskKey).toString());
        ui->spinBoxLeasetime->setValue(mpSettings->value(leaseTimeKey, defaultLeaseTime).toInt());
        mpSettings->endGroup();
    }
    else
    {
        makeSettingSuggestion();
    }
}

void MainWindow::onServerStateChange(bool newState)
{
    if (newState)
    {
        auto startAddress = QHostAddress(ui->lineEditStartAddress->text());
        auto endAddress = QHostAddress(ui->lineEditEndAddress->text());
        auto subnetmask = QHostAddress(ui->lineEditSubnetmask->text());

        if (mSelectedInterfaceAddress.isNull() || startAddress.isNull() || endAddress.isNull() || subnetmask.isNull())
        {
            return;
        }

        auto leaseTime = ui->spinBoxLeasetime->value();

        mpServer = new DhcpServer(mSelectedInterfaceAddress, startAddress, endAddress, subnetmask, leaseTime);

        ui->dynamicAssignmentsTableView->setModel(mpServer->dynamicAssignmentsItemModel());
        ui->staticAssignmentsTableView->setModel(mpServer->staticAssignmentsItemModel());
    }
    else
    {
        auto oldDynamicSelectionModel = ui->dynamicAssignmentsTableView->selectionModel();
        ui->dynamicAssignmentsTableView->setModel(&emptyDataModel);
        oldDynamicSelectionModel->deleteLater();

        auto oldStaticSelectionModel = ui->staticAssignmentsTableView->selectionModel();
        ui->staticAssignmentsTableView->setModel(&emptyDataModel);
        oldStaticSelectionModel->deleteLater();

        mpServer->deleteLater();
    }

    setUiEnable(!newState);

    auto buttonText = QString("%1 server").arg(newState ? "Stop" : "Start");
    ui->buttonServerState->setText(buttonText);

    auto trayiconText = QString("%1 %2").arg(defaultTrayIconText, newState ? "running" : "stopped");
    mpTrayIcon->setToolTip(trayiconText);
}

void MainWindow::onAssignmentsTableViewDoubleClicked(const QModelIndex &index)
{
    if (index.column() != 2)
    {
        // Only a double click on the state column can release the DHCP assignment. Ignore all other columns
        return;
    }

    // This slot is attached to two list boxes, so we have to determine the sender's data
    QStandardItemModel* pItemModel = (sender() == ui->dynamicAssignmentsTableView)
            ? mpServer->dynamicAssignmentsItemModel()
            : mpServer->staticAssignmentsItemModel();

    auto mac = pItemModel->item(index.row(), 1);
    mpServer->releaseClient(mac);
}

QtMessageHandler defaultMessageHandler = nullptr;
QPlainTextEdit *guiLogWindow = nullptr;

void logToGui(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (guiLogWindow)
    {
        const auto message = qFormatLogMessage(type, context, msg);
        guiLogWindow->appendPlainText(message);
    }
    if (defaultMessageHandler)
    {
        (*defaultMessageHandler)(type, context, msg);
    }
}

void MainWindow::onCheckShowDebugOutputChanged(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        ui->debugOutputText->clear();
        ui->debugOutputText->show();
        guiLogWindow = ui->debugOutputText;
        defaultMessageHandler = qInstallMessageHandler(logToGui);
    }
    else
    {
        ui->debugOutputText->hide();
        qInstallMessageHandler(defaultMessageHandler);
        guiLogWindow = nullptr;
        defaultMessageHandler = nullptr;
    }
}
