QT       += core network gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    assignments/assignmentpool.cpp \
    assignments/assignmentsmanager.cpp \
    assignments/dhcpassignment.cpp \
    assignments/dynamicassignmentspool.cpp \
    assignments/staticassignmentspool.cpp \
    dhcpserver.cpp \
    protocol/bootp.cpp \
    protocol/dhcpmessage.cpp \
    protocol/dhcpoption.cpp \
    protocol/macaddress.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    assignments/assignmentpool.h \
    assignments/assignmentsmanager.h \
    assignments/dhcpassignment.h \
    assignments/dynamicassignmentspool.h \
    assignments/staticassignmentspool.h \
    dhcpserver.h \
    protocol/bootp.h \
    protocol/dhcpmessage.h \
    protocol/dhcpoption.h \
    protocol/macaddress.h \
    mainwindow.h \

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Pack all necessary modules into output directory on release build
CONFIG(release, debug|release): {
    win32: {
        RC_ICONS = Connected.ico
	QMAKE_POST_LINK += $(QTDIR)/bin/windeployqt.exe --no-translations --no-system-d3d-compiler --no-webkit2 --no-opengl-sw --no-angle --no-virtualkeyboard $$OUT_PWD/release
    }
}

DISTFILES +=

RESOURCES += \
    Icons.qrc

VERSION = 1.0.0.0
